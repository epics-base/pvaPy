// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "RpcChannelProviderImpl.h"
#include "RpcChannelImpl.h"

const epics::pvData::String RpcChannelProviderImpl::ProviderName("rpcService");
const epics::pvData::Status RpcChannelProviderImpl::NoSuchChannelStatus(epics::pvData::Status::STATUSTYPE_ERROR, "No such channel");

RpcChannelProviderImpl::RpcChannelProviderImpl() 
{
}

epics::pvData::String RpcChannelProviderImpl::getProviderName() 
{
    return ProviderName;
}

std::tr1::shared_ptr<epics::pvAccess::ChannelProvider> RpcChannelProviderImpl::getChannelProvider()
{
    return shared_from_this();
}
    
void RpcChannelProviderImpl::cancelChannelFind() 
{
}

void RpcChannelProviderImpl::destroy() 
{
}
    
epics::pvAccess::ChannelFind::shared_pointer RpcChannelProviderImpl::channelFind(const epics::pvData::String& channelName, const epics::pvAccess::ChannelFindRequester::shared_pointer& channelFindRequester)
{
    bool found;
    {
        epics::pvData::Lock guard(mutex);
        found = (rpcServiceMap.find(channelName) != rpcServiceMap.end());
    }
    epics::pvAccess::ChannelFind::shared_pointer thisPtr(shared_from_this());
    channelFindRequester->channelFindResult(epics::pvData::Status::Ok, thisPtr, found);
    return thisPtr;
}

epics::pvAccess::Channel::shared_pointer RpcChannelProviderImpl::createChannel(
    const epics::pvData::String& channelName,
    const epics::pvAccess::ChannelRequester::shared_pointer& channelRequester,
    short /*priority*/)
{
    RPCServiceMap::const_iterator iter;
    {
        epics::pvData::Lock guard(mutex);
        iter = rpcServiceMap.find(channelName);
    }
        
    if (iter == rpcServiceMap.end()) {
        epics::pvAccess::Channel::shared_pointer nullChannel;
        channelRequester->channelCreated(NoSuchChannelStatus, nullChannel);
        return nullChannel;
    }
               
    epics::pvAccess::Channel::shared_pointer rpcChannel(new RpcChannelImpl(
        channelName,
        shared_from_this(),
        channelRequester,
        iter->second));
    channelRequester->channelCreated(epics::pvData::Status::Ok, rpcChannel);
    return rpcChannel;
}

epics::pvAccess::Channel::shared_pointer RpcChannelProviderImpl::createChannel(
    const epics::pvData::String& /*channelName*/,
    const epics::pvAccess::ChannelRequester::shared_pointer& /*channelRequester*/,
    short /*priority*/,
    const epics::pvData::String& /*address*/)
{
    // this will never get called by the pvAccess server
    throw std::runtime_error("Not supported");
}

void RpcChannelProviderImpl::registerService(const epics::pvData::String& serviceName, const epics::pvAccess::RPCService::shared_pointer& rpcService)
{
    epics::pvData::Lock guard(mutex);
    rpcServiceMap[serviceName] = rpcService;
}
    
void RpcChannelProviderImpl::unregisterService(const epics::pvData::String& serviceName)
{
    epics::pvData::Lock guard(mutex);
    rpcServiceMap.erase(serviceName);
}


