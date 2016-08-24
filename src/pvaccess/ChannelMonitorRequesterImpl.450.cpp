// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "ChannelMonitorRequesterImpl.h"

using namespace epics::pvaClient;

PvaPyLogger ChannelMonitorRequesterImpl::logger("ChannelMonitorRequesterImpl");
const double ChannelMonitorRequesterImpl::DefaultTimeout(3.0);

ChannelMonitorRequesterImpl::ChannelMonitorRequesterImpl(const std::string& channelName_) : 
    channelName(channelName_),
    pvObjectQueue(),
    isActive(true)
{
}

ChannelMonitorRequesterImpl::ChannelMonitorRequesterImpl(const ChannelMonitorRequesterImpl& channelMonitor) : 
    channelName(channelMonitor.channelName),
    pvObjectQueue(),
    isActive(true)
{
}

ChannelMonitorRequesterImpl::~ChannelMonitorRequesterImpl()
{
}

void ChannelMonitorRequesterImpl::event(const PvaClientMonitorPtr& monitor)
{
    epics::pvData::MonitorElement::shared_pointer element;
    epics::pvaClient::PvaClientMonitorDataPtr pvaData = monitor->getData();
    epics::pvData::PVDataCreatePtr create(epics::pvData::getPVDataCreate());

    while (isActive) {
        if (!pvObjectQueue.isFull()) {
            if (!monitor->poll()) {;
                break;
            }
            epics::pvData::PVStructurePtr pvStructurePtr(create->createPVStructure(pvaData->getPVStructure())); // copy
            PvObject pvObject(pvStructurePtr);
            pvObjectQueue.pushIfNotFull(pvObject);
            logger.debug("Pushed new monitor element into the queue: %d elements have not been processed.", pvObjectQueue.size());
            monitor->releaseEvent();
        }
        else {
            pvObjectQueue.waitForItemPopped(DefaultTimeout);
        }
    }
}

void ChannelMonitorRequesterImpl::unlisten()
{
    isActive = false;
}

PvObject ChannelMonitorRequesterImpl::getQueuedPvObject(double timeout) throw(ChannelTimeout)
{
    try {
        return pvObjectQueue.frontAndPop(timeout);
    }
    catch (InvalidState& ex) {
        throw ChannelTimeout("No PV changes for channel %s received.", channelName.c_str());
    }
}

void ChannelMonitorRequesterImpl::cancelGetQueuedPvObject()
{
    pvObjectQueue.cancelWaitForItemPushed();
}

void ChannelMonitorRequesterImpl::clearPvObjectQueue()
{
    logger.debug("Clearing pv object monitor queue: %d elements have not been processed.", pvObjectQueue.size());
    try {
        while (!pvObjectQueue.empty()) {
            pvObjectQueue.frontAndPop();
        }
    }
    catch (const InvalidState& ex) {
        // OK, we are done.
    }
}

void ChannelMonitorRequesterImpl::setPvObjectQueueMaxLength(int maxLength)
{
    pvObjectQueue.setMaxLength(maxLength);
}

int ChannelMonitorRequesterImpl::getPvObjectQueueMaxLength() 
{
    return pvObjectQueue.getMaxLength();
}

bool ChannelMonitorRequesterImpl::isPvObjectQueueFull()
{
    return pvObjectQueue.isFull();
}

void ChannelMonitorRequesterImpl::waitForPvObjectQueuePop(double timeout)
{
    pvObjectQueue.waitForItemPopped(timeout);
}

void ChannelMonitorRequesterImpl::stop()
{
    isActive = false;
    pvObjectQueue.cancelWaitForItemPushed();
    clearPvObjectQueue();
}
