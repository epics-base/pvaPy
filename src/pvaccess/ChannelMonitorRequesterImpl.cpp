// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "ChannelMonitorRequesterImpl.h"

PvaPyLogger ChannelMonitorRequesterImpl::logger("ChannelMonitorRequesterImpl");

ChannelMonitorRequesterImpl::ChannelMonitorRequesterImpl(const std::string& channelName_) : 
    channelName(channelName_),
    pvObjectQueue()
{
}

ChannelMonitorRequesterImpl::ChannelMonitorRequesterImpl(const ChannelMonitorRequesterImpl& channelMonitor) : 
    channelName(channelMonitor.channelName),
    pvObjectQueue()
{
}

ChannelMonitorRequesterImpl::~ChannelMonitorRequesterImpl()
{
}

std::string ChannelMonitorRequesterImpl::getRequesterName() 
{
    return "ChannelMonitorRequesterImpl";
}

void ChannelMonitorRequesterImpl::message(const std::string& message, epics::pvData::MessageType messageType)
{
    std::cerr << "[" << getRequesterName() << "] message(" << message << ", " << getMessageTypeName(messageType) << ")" << std::endl;
}

void ChannelMonitorRequesterImpl::monitorConnect(const epics::pvData::Status& status, const epics::pvData::Monitor::shared_pointer& monitor, const epics::pvData::StructureConstPtr&) 
{
    if (status.isSuccess()) {
        epics::pvData::Status startStatus = monitor->start();
        if (!startStatus.isSuccess()) {
            std::cerr << "[" << channelName << "] channel monitor start: " << startStatus.getMessage() << std::endl;
        }
    }
    else {
        std::cerr << "monitorConnect(" << status.getMessage() << ")" << std::endl;
    }
}

void ChannelMonitorRequesterImpl::monitorEvent(const epics::pvData::Monitor::shared_pointer& monitor)
{
    epics::pvData::MonitorElement::shared_pointer element;
    while (element = monitor->poll()) {
        epics::pvData::PVStructurePtr pvStructurePtr = epics::pvData::getPVDataCreate()->createPVStructure(element->pvStructurePtr);
        PvObject pvObject(pvStructurePtr); 
        pvObjectQueue.push(pvObject);
        monitor->release(element);
    }
    logger.debug("Pushed new monitor element into the queue: %d elements have not been processed.", pvObjectQueue.size());
}

void ChannelMonitorRequesterImpl::unlisten(const epics::pvData::Monitor::shared_pointer& monitor)
{
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
    pvObjectQueue.cancelWaitForItem();
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

