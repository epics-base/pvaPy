// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "RpcClient.h"
#include "PvaException.h"
#include "pv/rpcService.h"
#include "ChannelTimeout.h"

const int RpcClient::DefaultTimeout(1);

RpcClient::RpcClient(const std::string& channelName_) :
    PvaClient(),
    rpcClientInitialized(false),
    rpcClient(),
    channelName(channelName_)
{
}

RpcClient::RpcClient(const RpcClient& pvaRpcClient) :
    PvaClient(),
    rpcClientInitialized(pvaRpcClient.rpcClientInitialized),
    rpcClient(pvaRpcClient.rpcClient),
    channelName(pvaRpcClient.channelName)
{
}

RpcClient::~RpcClient()
{
    if (rpcClientInitialized) {
        rpcClientInitialized = false;
        rpcClient->destroy();
    }
}

epics::pvAccess::RPCClient::shared_pointer RpcClient::createRpcClient(const std::string& channelName, const epics::pvData::PVStructurePtr& pvRequest, double timeout) 
{
#if defined PVA_RPC_API_VERSION && PVA_RPC_API_VERSION == 440
    return epics::pvAccess::RPCClient::create(channelName);
#endif // if defined PVA_RPC_API_VERSION && PVA_RPC_API_VERSION == 440
}

epics::pvAccess::RPCClient::shared_pointer RpcClient::getRpcClient(const epics::pvData::PVStructurePtr& pvRequest, double timeout) 
{
    if (!rpcClientInitialized) {
        rpcClient = createRpcClient(channelName, pvRequest, timeout);
        rpcClientInitialized = true;
    }
    return rpcClient;
}

epics::pvData::PVStructure::shared_pointer RpcClient::request(const epics::pvData::PVStructurePtr& pvRequest, double timeout) 
{
    try {
        epics::pvAccess::RPCClient::shared_pointer client = getRpcClient(pvRequest, timeout);

#if defined PVA_RPC_API_VERSION && PVA_RPC_API_VERSION == 440
        epics::pvData::PVStructure::shared_pointer response = client->request(pvRequest, timeout);
#endif // if defined PVA_RPC_API_VERSION && PVA_RPC_API_VERSION == 440

        return response;
    }
    catch (const epics::pvAccess::RPCRequestException& ex) {
        throw PvaException(ex.what());
    }
    catch (std::exception& ex) {
        throw PvaException(ex.what());
    }
    catch (...) {
        throw PvaException("Unexpected error caught in RpcClient::request().");
    }
}


//PvObject* RpcClient::request(const PvObject& pvObject, double timeout) 
PvObject* RpcClient::invoke(const PvObject& pvObject) 
{
    epics::pvData::PVStructurePtr pvStructurePtr = pvObject.getPvStructurePtr();
    PvObject* response = new PvObject(request(pvStructurePtr));
    return response;
}
