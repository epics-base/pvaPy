#ifndef CHANNEL_MONITOR_REQUESTER_IMPL_H
#define CHANNEL_MONITOR_REQUESTER_IMPL_H

#include <string>
#include "pv/pvData.h"
#include "pv/pvAccess.h"
#include "PvaClient.h"
#include "boost/python/object.hpp"

class ChannelMonitorRequesterImpl : public epics::pvData::MonitorRequester
{
public:
    ChannelMonitorRequesterImpl(const std::string& channelName, const boost::python::object& pyMonitor);
    virtual ~ChannelMonitorRequesterImpl();
    virtual std::string getRequesterName() const;
    virtual void message(const std::string& message, epics::pvData::MessageType messageType);
    virtual void monitorConnect(const epics::pvData::Status& status, const epics::pvData::Monitor::shared_pointer& monitor, const epics::pvData::StructureConstPtr& pvStructurePtr);
    virtual void monitorEvent(const epics::pvData::Monitor::shared_pointer& monitor);
    virtual void unlisten(const epics::pvData::Monitor::shared_pointer& monitor);

private:
    std::string channelName;
    boost::python::object pyMonitor;
};

#endif // CHANNEL_MONITOR_REQUESTER_IMPL_H
