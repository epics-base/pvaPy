// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef CHANNEL_GET_REQUESTER_IMPL_H
#define CHANNEL_GET_REQUESTER_IMPL_H

#include <string>
#include "pv/pvData.h"
#include "pv/pvAccess.h"
#include "pv/event.h"
#include "PvaClient.h"

class ChannelGetRequesterImpl : public epics::pvAccess::ChannelGetRequester
{
public:
    
    ChannelGetRequesterImpl(const std::string& channelName);
    ChannelGetRequesterImpl(const ChannelGetRequesterImpl& channelGetRequester);
    
    virtual std::string getRequesterName();
    virtual void message(const std::string& message, epics::pvData::MessageType messageType);
    virtual void channelGetConnect(const epics::pvData::Status& status,
        const epics::pvAccess::ChannelGet::shared_pointer& channelGet,
        const epics::pvData::Structure::const_shared_pointer& structure);
    virtual void getDone(const epics::pvData::Status& status,
        const epics::pvAccess::ChannelGet::shared_pointer& channelGet,
        const epics::pvData::PVStructure::shared_pointer& pvStructure,
        const epics::pvData::BitSet::shared_pointer& bitSet);
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
    std::string channelName;
    bool done;
};

inline std::string ChannelGetRequesterImpl::getChannelName() const 
{
    return channelName;
}
 
#endif
