#include "ChannelMonitorRequesterImpl.h"
#include "PvObject.h"
#include "ObjectNotFound.h"

ChannelMonitorRequesterImpl::ChannelMonitorRequesterImpl(const std::string& channelName_) : 
    channelName(channelName_),
    monitorMap(),
    monitorMutex()
{
}

ChannelMonitorRequesterImpl::ChannelMonitorRequesterImpl(const ChannelMonitorRequesterImpl& channelMonitor) : 
    channelName(channelMonitor.channelName),
    monitorMap(channelMonitor.monitorMap),
    monitorMutex()
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
        std::map<std::string,boost::python::object>::iterator iter;
        for (iter = monitorMap.begin(); iter != monitorMap.end(); iter++) {
            std::string monitorName = iter->first;
            boost::python::object pyMonitor = iter->second;
            try {
                pyMonitor(pvObject);
            }
            catch(const std::exception& ex) {
                    std::cerr << "Error for monitor " << monitorName << ": " << ex.what();
            }
        }
        monitor->release(element);
    }
}

void ChannelMonitorRequesterImpl::unlisten(const epics::pvData::Monitor::shared_pointer&)
{
}

void ChannelMonitorRequesterImpl::registerMonitor(const std::string& monitorName, const boost::python::object& pyMonitor)
{
    epics::pvData::Lock lock(monitorMutex);
    monitorMap[monitorName] = pyMonitor;
}

void ChannelMonitorRequesterImpl::unregisterMonitor(const std::string& monitorName)
{
    epics::pvData::Lock lock(monitorMutex);
    std::map<std::string,boost::python::object>::const_iterator iterator = monitorMap.find(monitorName);
    if (iterator == monitorMap.end()) {
        throw ObjectNotFound("Monitor " + monitorName + " is not registered.");
    }
    monitorMap.erase(monitorName);
}


