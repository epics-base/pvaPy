#ifndef RPC_CLIENT_H
#define RPC_CLIENT_H

#include <string>

#include "PvaClient.h"
#include "PvObject.h"
#include "pv/rpcClient.h"

/**
 * RPC client for PV access.
 */
class RpcClient : public PvaClient
{
public:
    static const int DefaultTimeout;

    RpcClient(const std::string& serviceName);
    RpcClient(const RpcClient& pvaRpcClient);
    std::string getServiceName() const;

    virtual ~RpcClient();
    virtual epics::pvData::PVStructurePtr request(const epics::pvData::PVStructurePtr& pvRequest, double timeOut=DefaultTimeout);
    //virtual PvObject* request(const PvObject& pvObject, double timeOut=DefaultTimeout);
    virtual PvObject* invoke(const PvObject& pvObject);

private:
    epics::pvAccess::RPCClient::shared_pointer rpcClient;
    std::string serviceName;
};

inline std::string RpcClient::getServiceName() const
{
    return serviceName;
}

#endif
