// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef CHANNEL_REQUESTER_IMPL_H
#define CHANNEL_REQUESTER_IMPL_H

#include <pv/event.h>
#include <pv/pvData.h>
#include <pv/pvAccess.h>

#include <string>

class ChannelRequesterImpl : public epics::pvAccess::ChannelRequester
{
private:
    epics::pvData::Event event;
    bool printOnlyErrors;
    
public:

    ChannelRequesterImpl(bool printOnlyErrors = false);

    virtual std::string getRequesterName();
    virtual void message(const std::string& message, epics::pvData::MessageType messageType);
    virtual void channelCreated(const epics::pvData::Status& status, const epics::pvAccess::Channel::shared_pointer& channel);
    virtual void channelStateChange(const epics::pvAccess::Channel::shared_pointer& channel, epics::pvAccess::Channel::ConnectionState connectionState);
    
    bool waitUntilConnected(double timeOut);
};

#endif
