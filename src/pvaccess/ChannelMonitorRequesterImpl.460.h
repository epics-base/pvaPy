// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef CHANNEL_MONITOR_REQUESTER_IMPL_H
#define CHANNEL_MONITOR_REQUESTER_IMPL_H

#include <string>
#include "pv/pvData.h"
#include "pv/pvaClient.h"
#include "PvaPyLogger.h"
#include "ChannelMonitorDataProcessor.h"

class ChannelMonitorRequesterImpl : public epics::pvaClient::PvaClientMonitorRequester
{
public:
    POINTER_DEFINITIONS(ChannelMonitorRequesterImpl);
    ChannelMonitorRequesterImpl(const std::string& channelName, ChannelMonitorDataProcessor* processor);
    ChannelMonitorRequesterImpl(const ChannelMonitorRequesterImpl& channelMonitor);
    virtual ~ChannelMonitorRequesterImpl();
    virtual void event(const epics::pvaClient::PvaClientMonitorPtr& monitor);
    virtual void unlisten();
    virtual void resetCounters() { nReceived = 0; nOverruns = 0; }
    virtual unsigned int getNumReceived() const { return nReceived; }
    virtual unsigned int getNumOverruns() const { return nOverruns; }

private:
    static PvaPyLogger logger;
    std::string channelName;
    ChannelMonitorDataProcessor* processor;
    bool isActive;
    unsigned int nReceived;
    unsigned int nOverruns;
};

#endif // CHANNEL_MONITOR_REQUESTER_IMPL_H
