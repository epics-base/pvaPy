#ifndef MONITOR_REQUESTER_IMPL_H
#define MONITOR_REQUESTER_IMPL_H

#include <string>
#include "pv/pvData.h"
#include "pv/pvAccess.h"
#include "PvaClient.h"

class MonitorRequesterImpl : public epics::pvData::MonitorRequester
{
public:
    MonitorRequesterImpl(const std::string& channelName);
    virtual ~MonitorRequesterImpl();
    virtual std::string getRequesterName() const;
    virtual void message(const std::string& message, epics::pvData::MessageType messageType);
    virtual void monitorConnect(const epics::pvData::Status& status, const epics::pvData::Monitor::shared_pointer& monitor, const epics::pvData::StructureConstPtr& pvStructurePtr);
    virtual void monitorEvent(const epics::pvData::Monitor::shared_pointer& monitor);
    virtual void unlisten(const epics::pvData::Monitor::shared_pointer& monitor);

private:
    std::string channelName;
};

#endif // MONITOR_REQUESTER_IMPL_H
