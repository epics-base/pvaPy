// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

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
    
    ChannelPutRequesterImpl(const std::string& channelName);
    ChannelPutRequesterImpl(const ChannelPutRequesterImpl& channelPutRequester);
    virtual std::string getRequesterName();
    virtual void message(const std::string& message, epics::pvData::MessageType messageType);
    virtual void channelPutConnect(const epics::pvData::Status& status,
        const epics::pvAccess::ChannelPut::shared_pointer& channelPut,
        const epics::pvData::Structure::const_shared_pointer& structure);
    virtual void getDone(const epics::pvData::Status& status, const epics::pvAccess::ChannelPut::shared_pointer& channelPut, const epics::pvData::PVStructure::shared_pointer& pvStructure, const epics::pvData::BitSet::shared_pointer& bitSet);
    virtual void putDone(const epics::pvData::Status& status, const epics::pvAccess::ChannelPut::shared_pointer& channelPut);
    epics::pvData::PVStructure::shared_pointer getStructure();
    epics::pvData::BitSet::shared_pointer getBitSet();
    void resetEvent();
    bool waitUntilDone(double timeOut);

private:
    static PvaClient pvaClient;

    epics::pvAccess::ChannelPut::shared_pointer channelPut;
    epics::pvData::PVStructure::shared_pointer pvStructure;
    epics::pvData::BitSet::shared_pointer bitSet;
    epics::pvData::Mutex pointerMutex;
    epics::pvData::Mutex eventMutex;
    epics::pvData::EventPtr event;
    std::string channelName;
    bool done;

};

#endif

