// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef RPC_CHANNEL_PROVIDER_IMPL_H
#define RPC_CHANNEL_PROVIDER_IMPL_H

#include <stdexcept>
#include "pv/pvAccess.h"
#include "pv/rpcServer.h"

class RpcChannelProviderImpl :
    public virtual epics::pvAccess::ChannelProvider, 
    public virtual epics::pvAccess::ChannelFind,
    public std::tr1::enable_shared_from_this<RpcChannelProviderImpl> 
{
public:
    POINTER_DEFINITIONS(RpcChannelProviderImpl);
    
    static const epics::pvData::String ProviderName;
    static const epics::pvData::Status NoSuchChannelStatus;
    
    RpcChannelProviderImpl();
    virtual epics::pvData::String getProviderName();
    virtual std::tr1::shared_ptr<epics::pvAccess::ChannelProvider> getChannelProvider();
    virtual void cancelChannelFind();
    virtual void destroy();
    virtual epics::pvAccess::ChannelFind::shared_pointer channelFind(const epics::pvData::String& channelName, const epics::pvAccess::ChannelFindRequester::shared_pointer& channelFindRequester);
    virtual epics::pvAccess::Channel::shared_pointer createChannel(
            const epics::pvData::String& channelName,
            const epics::pvAccess::ChannelRequester::shared_pointer& channelRequester,
            short priority);
    virtual epics::pvAccess::Channel::shared_pointer createChannel(
        const epics::pvData::String& channelName,
        const epics::pvAccess::ChannelRequester::shared_pointer& channelRequester,
        short priority,
        const epics::pvData::String& address);
    void registerService(const epics::pvData::String& serviceName, const epics::pvAccess::RPCService::shared_pointer& rpcService);
    void unregisterService(const epics::pvData::String& serviceName);

private:    
    typedef std::map<epics::pvData::String, epics::pvAccess::RPCService::shared_pointer> RPCServiceMap;
    RPCServiceMap rpcServiceMap;
    epics::pvData::Mutex mutex;
};

#endif // RPC_CHANNEL_PROVIDER_IMPL_H

