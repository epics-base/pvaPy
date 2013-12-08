#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include "ChannelGetRequesterImpl.h"
#include "ChannelRequesterImpl.h"
#include "PvaClient.h"
#include "PvObject.h"

#define DEFAULT_TIMEOUT 3.0
#define DEFAULT_REQUEST "field(value)"

class Channel
{
public:
    
    Channel(const epics::pvData::String& channelName);
    Channel(const Channel& channel);
    virtual ~Channel();

    std::string getName() const;
    PvObject* get();
    void put(const PvObject&);

private:
    static PvaClient pvaClient;
    static epics::pvData::Requester::shared_pointer requester;
    static epics::pvAccess::ChannelProvider::shared_pointer provider;
    static std::tr1::shared_ptr<ChannelRequesterImpl> requesterImpl;

    ChannelGetRequesterImpl channelGetRequester;
    epics::pvAccess::Channel::shared_pointer channel;
};

inline std::string Channel::getName() const
{
    return channelGetRequester.getChannelName();
}

#endif
