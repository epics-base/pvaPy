#include "ChannelMonitorRequesterImpl.h"

ChannelMonitorRequesterImpl::ChannelMonitorRequesterImpl(const std::string& channelName_) : 
    channelName(channelName_),
    pvObjectMonitorQueue()
{
}

ChannelMonitorRequesterImpl::ChannelMonitorRequesterImpl(const ChannelMonitorRequesterImpl& channelMonitor) : 
    channelName(channelMonitor.channelName),
    pvObjectMonitorQueue()
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
        PvObject pvObject(element->pvStructurePtr);
        pvObjectMonitorQueue.push(pvObject);
        monitor->release(element);
    }
}

void ChannelMonitorRequesterImpl::unlisten(const epics::pvData::Monitor::shared_pointer&)
{
}

PvObject ChannelMonitorRequesterImpl::getQueuedPvObject(double timeout) throw(ChannelTimeout)
{
    try {
        return pvObjectMonitorQueue.frontAndPop(timeout);
    }
    catch (InvalidState& ex) {
        throw ChannelTimeout("No PV changes for channel %s received.", channelName.c_str());
    }
}

void ChannelMonitorRequesterImpl::cancelGetQueuedPvObject()
{
    pvObjectMonitorQueue.cancelWaitForItem();
}

