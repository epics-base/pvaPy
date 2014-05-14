
#ifndef RPC_CHANNEL_PROVIDER_FACTORY_H
#define RPC_CHANNEL_PROVIDER_FACTORY_H

#include "pv/rpcServer.h"
#include "RpcChannelProviderImpl.h"

class RpcChannelProviderFactory : public epics::pvAccess::ChannelProviderFactory
{
public:
    POINTER_DEFINITIONS(RpcChannelProviderFactory);

    RpcChannelProviderFactory();
    virtual epics::pvData::String getFactoryName();
    virtual epics::pvAccess::ChannelProvider::shared_pointer sharedInstance();
    virtual epics::pvAccess::ChannelProvider::shared_pointer newInstance();
private:
    RpcChannelProviderImpl::shared_pointer rpcChannelProviderImpl;
};


#endif // RPC_CHANNEL_PROVIDER_FACTORY_H
