// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "RpcChannelProviderFactory.h"

RpcChannelProviderFactory::RpcChannelProviderFactory() :
    rpcChannelProviderImpl(new RpcChannelProviderImpl())
{
}

epics::pvData::String RpcChannelProviderFactory::getFactoryName()
{
    return RpcChannelProviderImpl::ProviderName;
}

epics::pvAccess::ChannelProvider::shared_pointer RpcChannelProviderFactory::sharedInstance()
{
    return rpcChannelProviderImpl;
}

epics::pvAccess::ChannelProvider::shared_pointer RpcChannelProviderFactory::newInstance()
{
    return epics::pvAccess::ChannelProvider::shared_pointer(new RpcChannelProviderImpl());
}

