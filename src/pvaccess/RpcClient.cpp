// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python.hpp"
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
    pvRequest = epics::pvData::CreateRequest::create()->createRequest("");
}

#if defined PVA_RPC_API_VERSION && PVA_RPC_API_VERSION >= 460
RpcClient::RpcClient(const std::string& channelName_, const PvObject& pvRequestObject) :
    PvaClient(),
    rpcClientInitialized(false),
    rpcClient(),
    channelName(channelName_)
{
    pvRequest = pvRequestObject.getPvStructurePtr();
}
#endif

RpcClient::RpcClient(const RpcClient& pvaRpcClient) :
    PvaClient(),
    rpcClientInitialized(pvaRpcClient.rpcClientInitialized),
    rpcClient(pvaRpcClient.rpcClient),
    channelName(pvaRpcClient.channelName),
    pvRequest(pvaRpcClient.pvRequest)
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
#if defined PVA_RPC_API_VERSION && PVA_RPC_API_VERSION >= 460
    return epics::pvAccess::RPCClient::create(channelName, pvRequest);
#else
    return epics::pvAccess::RPCClient::create(channelName);
#endif // if defined PVA_RPC_API_VERSION && PVA_RPC_API_VERSION >= 460
}

epics::pvAccess::RPCClient::shared_pointer RpcClient::getRpcClient(const epics::pvData::PVStructurePtr& pvRequest, double timeout) 
{
    if (!rpcClientInitialized) {
        rpcClient = createRpcClient(channelName, pvRequest, timeout);
        rpcClientInitialized = true;
    }
    return rpcClient;
}

epics::pvData::PVStructure::shared_pointer RpcClient::request(const epics::pvData::PVStructurePtr& arguments, double timeout)
{
    try {
        epics::pvAccess::RPCClient::shared_pointer client = getRpcClient(pvRequest, timeout);

#if defined PVA_RPC_API_VERSION && PVA_RPC_API_VERSION >= 440
        epics::pvData::PVStructure::shared_pointer response = client->request(arguments, timeout);
#endif // if defined PVA_RPC_API_VERSION && PVA_RPC_API_VERSION >= 440

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


PvObject* RpcClient::invoke(const PvObject& pvArgumentObject, double timeout) 
{
    epics::pvData::PVStructurePtr pvStructurePtr = pvArgumentObject.getPvStructurePtr();

    PvObject* response = new PvObject(request(pvStructurePtr, timeout));
    return response;
}

PvObject* RpcClient::invoke(const PvObject& pvArgumentObject) 
{
    return invoke(pvArgumentObject, DefaultTimeout);
}
