#ifndef CHANNEL_GET_REQUESTER_IMPL_H
#define CHANNEL_GET_REQUESTER_IMPL_H

#include <string>
#include "pv/pvData.h"
#include "pv/pvAccess.h"
#include "pv/event.h"
#include "PvaClient.h"

#define DEFAULT_TIMEOUT 3.0
#define DEFAULT_REQUEST "field(value)"

class ChannelGetRequesterImpl : public epics::pvAccess::ChannelGetRequester
{
public:
    
    ChannelGetRequesterImpl(const epics::pvData::String& channelName);
    ChannelGetRequesterImpl(const ChannelGetRequesterImpl& channelGetRequester);
    
    virtual epics::pvData::String getRequesterName();
    virtual void message(const epics::pvData::String& message, epics::pvData::MessageType messageType);
    virtual void channelGetConnect(const epics::pvData::Status& status,
        const epics::pvAccess::ChannelGet::shared_pointer& channelGet,
        const epics::pvData::PVStructure::shared_pointer& pvStructure, 
        const epics::pvData::BitSet::shared_pointer& bitSet);
    virtual void getDone(const epics::pvData::Status& status);

    epics::pvData::PVStructure::shared_pointer getPVStructure();
    bool waitUntilGet(double timeOut);
    std::string getChannelName() const;

private:
    static PvaClient pvaClient;

    epics::pvAccess::ChannelGet::shared_pointer channelGet;
    epics::pvData::PVStructure::shared_pointer pvStructure;
    epics::pvData::BitSet::shared_pointer bitSet;
    epics::pvData::Mutex pointerMutex;
    epics::pvData::Event event;
    epics::pvData::String channelName;
    bool done;
};

inline std::string ChannelGetRequesterImpl::getChannelName() const 
{
    return channelName;
}
 
#endif
