#include "ChannelMonitorRequesterImpl.h"

MonitorRequesterImpl::MonitorRequesterImpl(const std::string& channelName_, const boost::python::object& pyMonitor_) : 
    channelName(channelName_),
    pyMonitor(pyMonitor_),
{
}

MonitorRequesterImpl::~MonitorRequesterImpl()
{
}

std::string MonitorRequesterImpl::getRequesterName() const
{
    return "MonitorRequesterImpl";
}

void MonitorRequesterImpl::message(const std::string& message, epics::pvData::MessageType messageType)
{
    std::cerr << "[" << getRequesterName() << "] message(" << message << ", " << getMessageTypeName(messageType) << ")" << std::endl;
}

void MonitorRequesterImpl::monitorConnect(const epics::pvData::Status& status, const epics::pvData::Monitor::shared_pointer& monitor, const epics::pvData::StructureConstPtr&) 
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

void MonitorRequesterImpl::monitorEvent(const epics::pvData::Monitor::shared_pointer& monitor)
{
    epics::pvData::MonitorElement::shared_pointer element;
    while (element = monitor->poll()) {
        epics::pvData::PVField::shared_pointer value = element->pvStructurePtr->getSubField("value");
        PvObject pvObject(value);
        pyMonitor(pvObject);
        monitor->release(element);
    }
}

void MonitorRequesterImpl::unlisten(const epics::pvData::Monitor::shared_pointer&)
{
}


