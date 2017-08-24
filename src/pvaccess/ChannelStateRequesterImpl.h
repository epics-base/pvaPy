// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#if PVA_API_VERSION >= 450
#ifndef CHANNEL_STATE_REQUESTER_IMPL_H
#define CHANNEL_STATE_REQUESTER_IMPL_H

#include <iostream>
#include <string>

#include "PvaClient.h"
#include "ChannelMonitorDataProcessor.h"

class ChannelStateRequesterImpl : public epics::pvaClient::PvaClientChannelStateChangeRequester
{
public:
    POINTER_DEFINITIONS(ChannelStateRequesterImpl);

    ChannelStateRequesterImpl(bool& connected_, ChannelMonitorDataProcessor* dataProcessor_) : connected(connected_), dataProcessor(dataProcessor_) {}
    virtual ~ChannelStateRequesterImpl() {}

    bool isConnected() const {return connected;}

    // PvaClientChannelStateChangeRequester interface
    virtual void channelStateChange(const epics::pvaClient::PvaClientChannelPtr& channel, bool isConnected) {
        connected=isConnected;
        if (connected) {
            dataProcessor->onChannelConnect();
        }
        else {
            dataProcessor->onChannelDisconnect();
        }
    }

private:
    bool& connected;
    ChannelMonitorDataProcessor* dataProcessor;
};

#endif // CHANNEL_STATE_REQUESTER_IMPL_H
#endif // if PVA_API_VERSION >= 450
