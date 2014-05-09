#ifndef RPC_CLIENT_H
#define RPC_CLIENT_H

#include <string>

#include "PvaClient.h"
#include "PvObject.h"
#include "pv/event.h" // this should really be in pv/rpcClient.h
#include "pv/rpcClient.h"

/**
 * RPC client for PV access.
 */
class RpcClient : public PvaClient
{
public:
    static const int DefaultTimeout;

    RpcClient(const std::string& channelName);
    RpcClient(const RpcClient& pvaRpcClient);
    std::string getChannelName() const;

    virtual ~RpcClient();
    virtual epics::pvData::PVStructurePtr request(const epics::pvData::PVStructurePtr& pvRequest, double timeout=DefaultTimeout);
    //virtual PvObject* request(const PvObject& pvObject, double timeout=DefaultTimeout);
    virtual PvObject* invoke(const PvObject& pvObject);

private:
    static epics::pvAccess::RPCClient::shared_pointer createRpcClient(const std::string& channelName, const epics::pvData::PVStructurePtr& pvRequest, double timeout=DefaultTimeout);
    epics::pvAccess::RPCClient::shared_pointer getRpcClient(const epics::pvData::PVStructurePtr& pvRequest, double timeout=DefaultTimeout);

    bool rpcClientInitialized;
    epics::pvAccess::RPCClient::shared_pointer rpcClient;
    std::string channelName;
};

inline std::string RpcClient::getChannelName() const
{
    return channelName;
}

#endif
