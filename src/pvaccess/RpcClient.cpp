#include "RpcClient.h"
#include "PvaException.h"
#include "pv/rpcService.h"

const int RpcClient::DefaultTimeout(1);

RpcClient::RpcClient(const std::string& serviceName_) :
    PvaClient(),
    rpcClient(epics::pvAccess::RPCClientFactory::create(serviceName_)),
    serviceName(serviceName_)
{
}

RpcClient::RpcClient(const RpcClient& pvaRpcClient) :
    PvaClient(),
    rpcClient(pvaRpcClient.rpcClient),
    serviceName(pvaRpcClient.serviceName)
{
}

RpcClient::~RpcClient()
{
}

epics::pvData::PVStructure::shared_pointer RpcClient::request(const epics::pvData::PVStructurePtr& pvRequest, double timeOut) 
{
    try {
        epics::pvData::PVStructure::shared_pointer response = rpcClient->request(pvRequest, timeOut);
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


//PvObject* RpcClient::request(const PvObject& pvObject, double timeOut) 
PvObject* RpcClient::invoke(const PvObject& pvObject) 
{
    epics::pvData::PVStructurePtr pvStructurePtr = pvObject.getPvStructurePtr();
    PvObject* response = new PvObject(request(pvStructurePtr));
    return response;
}
