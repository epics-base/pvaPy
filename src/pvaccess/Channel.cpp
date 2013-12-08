#include <iostream>
#include "Channel.h"
#include "ChannelGetRequesterImpl.h"
#include "ChannelPutRequesterImpl.h"
#include "GetFieldRequesterImpl.h"
#include "RequesterImpl.h"
#include "pv/clientFactory.h"
#include "pv/logger.h"
#include "ChannelTimeout.h"
#include "InvalidRequest.h"

PvaClient Channel::pvaClient;
epics::pvData::Requester::shared_pointer Channel::requester(new RequesterImpl("Channel"));
epics::pvAccess::ChannelProvider::shared_pointer Channel::provider = epics::pvAccess::getChannelAccess()->getProvider("pva");
std::tr1::shared_ptr<ChannelRequesterImpl> Channel::requesterImpl(new ChannelRequesterImpl(true));

Channel::Channel(const epics::pvData::String& channelName) 
    :  channelGetRequester(channelName),
    channel(provider->createChannel(channelName, requesterImpl))
{
}
    
Channel::Channel(const Channel& c) 
    :  channelGetRequester(c.channelGetRequester),
    channel(c.channel)
{
    //epics::pvAccess::pvAccessSetLogLevel(epics::pvAccess::logLevelAll);
}

Channel::~Channel()
{
    channel->destroy();
}
 
PvObject* Channel::get() 
{
    epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvAccess::getCreateRequest()->createRequest(DEFAULT_REQUEST, requester);
    std::tr1::shared_ptr<ChannelRequesterImpl> channelRequesterImpl = std::tr1::dynamic_pointer_cast<ChannelRequesterImpl>(channel->getChannelRequester());

    if (channel->getConnectionState() != epics::pvAccess::Channel::CONNECTED) {
        if (!channelRequesterImpl->waitUntilConnected(DEFAULT_TIMEOUT)) {
            throw ChannelTimeout("Channel %s timed out", channel->getChannelName().c_str());
        }
    }

    std::tr1::shared_ptr<GetFieldRequesterImpl> getFieldRequesterImpl;
    getFieldRequesterImpl.reset(new GetFieldRequesterImpl(channel));
    channel->getField(getFieldRequesterImpl, "");

    if (!getFieldRequesterImpl->waitUntilFieldGet(DEFAULT_TIMEOUT)) {
        throw ChannelTimeout("Channel %s field get timed out", channel->getChannelName().c_str());
    }
            
    if (!getFieldRequesterImpl.get()) {
        throw PvaException("Failed to get introspection data for channel %s", channel->getChannelName().c_str());
    }

	epics::pvData::Structure::const_shared_pointer structure =
	    std::tr1::dynamic_pointer_cast<const epics::pvData::Structure>(getFieldRequesterImpl->getField());
	if (structure.get() == 0 || structure->getField("value").get() == 0) {
	    // fallback to structure
		pvRequest = epics::pvAccess::getCreateRequest()->createRequest("field()", requester);
    }

	std::tr1::shared_ptr<ChannelGetRequesterImpl> getRequesterImpl(new ChannelGetRequesterImpl(channel->getChannelName()));
	epics::pvAccess::ChannelGet::shared_pointer channelGet = channel->createChannelGet(getRequesterImpl, pvRequest);
	if (getRequesterImpl->waitUntilGet(DEFAULT_TIMEOUT)) {
	    return new PvObject(getRequesterImpl->getPVStructure());
    }
    throw ChannelTimeout("Channel %s get request timed out", channel->getChannelName().c_str());
}

void Channel::put(const PvObject& pvObject) 
{
    epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvAccess::getCreateRequest()->createRequest(DEFAULT_REQUEST, requester);
    std::tr1::shared_ptr<ChannelRequesterImpl> channelRequesterImpl = std::tr1::dynamic_pointer_cast<ChannelRequesterImpl>(channel->getChannelRequester());

    if (channel->getConnectionState() != epics::pvAccess::Channel::CONNECTED) {
        if (!channelRequesterImpl->waitUntilConnected(DEFAULT_TIMEOUT)) {
            throw ChannelTimeout("Channel %s timed out", channel->getChannelName().c_str());
        }
    }

	std::tr1::shared_ptr<ChannelPutRequesterImpl> putRequesterImpl(new ChannelPutRequesterImpl(channel->getChannelName()));
	epics::pvAccess::ChannelPut::shared_pointer channelPut = channel->createChannelPut(putRequesterImpl, pvRequest);
	if (putRequesterImpl->waitUntilDone(DEFAULT_TIMEOUT)) {
        epics::pvData::PVStructurePtr pvStructurePtr = putRequesterImpl->getStructure();
        pvStructurePtr << pvObject;
        channelPut->put(false);
	    if (putRequesterImpl->waitUntilDone(DEFAULT_TIMEOUT)) {
	        return;
        }
    }
    throw ChannelTimeout("Channel %s put request timed out", channel->getChannelName().c_str());
}
