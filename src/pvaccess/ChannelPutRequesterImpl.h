#ifndef CHANNEL_PUT_REQUESTER_IMPL_H
#define CHANNEL_PUT_REQUESTER_IMPL_H

#include <string>
#include "pv/pvData.h"
#include "pv/pvAccess.h"
#include "pv/event.h"
#include "PvaClient.h"

class ChannelPutRequesterImpl : public epics::pvAccess::ChannelPutRequester
{
public:
    
    ChannelPutRequesterImpl(const epics::pvData::String& channelName);
    ChannelPutRequesterImpl(const ChannelPutRequesterImpl& channelPutRequester);
    virtual epics::pvData::String getRequesterName();
    virtual void message(const epics::pvData::String& message, epics::pvData::MessageType messageType);
    virtual void channelPutConnect(const epics::pvData::Status& status,
        const epics::pvAccess::ChannelPut::shared_pointer& channelPut,
        const epics::pvData::PVStructure::shared_pointer& pvStructure, 
        const epics::pvData::BitSet::shared_pointer& bitSet);
    virtual void getDone(const epics::pvData::Status& status);
    virtual void putDone(const epics::pvData::Status& status);
    epics::pvData::PVStructure::shared_pointer getStructure();
    void resetEvent();
    bool waitUntilDone(double timeOut);

private:
    static PvaClient pvaClient;

    epics::pvAccess::ChannelPut::shared_pointer channelPut;
    epics::pvData::PVStructure::shared_pointer pvStructure;
    epics::pvData::BitSet::shared_pointer bitSet;
    epics::pvData::Mutex pointerMutex;
    epics::pvData::Event event;
    epics::pvData::String channelName;
    bool done;

};

#endif

