#ifndef CHANNEL_RPC_SERVICE_IMPL_H
#define CHANNEL_RPC_SERVICE_IMPL_H

#include <stdexcept>
#include "pv/rpcServer.h"


class ChannelRpcServiceImpl : public epics::pvAccess::ChannelRPC
{
public:
    ChannelRpcServiceImpl(const epics::pvAccess::ChannelRPCRequester::shared_pointer& channelRpcRequester, const epics::pvAccess::RPCService::shared_pointer& rpcService);
        
    virtual ~ChannelRpcServiceImpl();
    void processRequest(const epics::pvData::PVStructure::shared_pointer& pvArgument, bool lastRequest=false);
    virtual void request(const epics::pvData::PVStructure::shared_pointer& pvArgument, bool lastRequest=false);
    virtual void destroy();
    virtual void lock();
    virtual void unlock();
private:
    epics::pvAccess::ChannelRPCRequester::shared_pointer channelRpcRequester;
    epics::pvAccess::RPCService::shared_pointer rpcService;
};

#endif // CHANNEL_RPC_SERVICE_IMPL_H
