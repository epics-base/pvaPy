#ifndef CHANNEL_MONITOR_REQUESTER_IMPL_H
#define CHANNEL_MONITOR_REQUESTER_IMPL_H

#include <string>
#include <map>
#include "pv/pvData.h"
#include "pv/pvAccess.h"
#include "PvObject.h"
#include "PvaClient.h"
#include "SynchronizedQueue.h"
#include "ChannelTimeout.h"

class ChannelMonitorRequesterImpl : public epics::pvData::MonitorRequester
{
public:
    POINTER_DEFINITIONS(ChannelMonitorRequesterImpl);
    ChannelMonitorRequesterImpl(const std::string& channelName);
    ChannelMonitorRequesterImpl(const ChannelMonitorRequesterImpl& channelMonitor);
    virtual ~ChannelMonitorRequesterImpl();
    virtual std::string getRequesterName();
    virtual void message(const std::string& message, epics::pvData::MessageType messageType);
    virtual void monitorConnect(const epics::pvData::Status& status, const epics::pvData::Monitor::shared_pointer& monitor, const epics::pvData::StructureConstPtr& pvStructurePtr);
    virtual void monitorEvent(const epics::pvData::Monitor::shared_pointer& monitor);
    virtual void unlisten(const epics::pvData::Monitor::shared_pointer& monitor);

    virtual PvObject getQueuedPvObject(double timeout) throw(ChannelTimeout);
    virtual void cancelGetQueuedPvObject();

private:
    std::string channelName;
    SynchronizedQueue<PvObject> pvObjectMonitorQueue;
};

#endif // CHANNEL_MONITOR_REQUESTER_IMPL_H
