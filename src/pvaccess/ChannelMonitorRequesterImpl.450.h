// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef CHANNEL_MONITOR_REQUESTER_IMPL_H
#define CHANNEL_MONITOR_REQUESTER_IMPL_H

#include <string>
#include <map>
#include <utility>
#include "pv/pvData.h"
#include "pv/pvAccess.h"
#include "pv/pvaClient.h"
#include "PvObject.h"
#include "PvaClient.h"
#include "PvaPyLogger.h"
#include "SynchronizedQueue.h"
#include "ChannelTimeout.h"

class ChannelMonitorRequesterImpl : public epics::pvaClient::PvaClientMonitorRequester
{
public:
    static const double DefaultTimeout;

    POINTER_DEFINITIONS(ChannelMonitorRequesterImpl);
    ChannelMonitorRequesterImpl(const std::string& channelName);
    ChannelMonitorRequesterImpl(const ChannelMonitorRequesterImpl& channelMonitor);
    virtual ~ChannelMonitorRequesterImpl();
    virtual void event(const epics::pvaClient::PvaClientMonitorPtr& monitor);
    virtual void unlisten();

    virtual PvObject getQueuedPvObject(double timeout) throw(ChannelTimeout);
    virtual void cancelGetQueuedPvObject();
    virtual void clearPvObjectQueue();

    virtual void setPvObjectQueueMaxLength(int maxLength);
    virtual int getPvObjectQueueMaxLength();

    virtual bool isPvObjectQueueFull();
    virtual void waitForPvObjectQueuePop(double timeout);
    virtual void stop();

private:
    static PvaPyLogger logger;
    std::string channelName;
    SynchronizedQueue<PvObject> pvObjectQueue;
    bool isActive;
};

#endif // CHANNEL_MONITOR_REQUESTER_IMPL_H
