#include "ChannelRpcServiceImpl.h"

ChannelRpcServiceImpl::ChannelRpcServiceImpl(const epics::pvAccess::ChannelRPCRequester::shared_pointer& channelRpcRequester_, const epics::pvAccess::RPCService::shared_pointer& rpcService_) :
    channelRpcRequester(channelRpcRequester_),
    rpcService(rpcService_)
{
}

ChannelRpcServiceImpl::~ChannelRpcServiceImpl()
{
    destroy();
}

void ChannelRpcServiceImpl::processRequest(const epics::pvData::PVStructure::shared_pointer& pvArgument, bool lastRequest)
{
    epics::pvData::PVStructure::shared_pointer result;
    epics::pvData::Status status = epics::pvData::Status::Ok;
    epics::pvData::Status::StatusType fatalError = epics::pvData::Status::STATUSTYPE_FATAL;
    bool ok = true;
    try {
        result = rpcService->request(pvArgument);
    }
    catch (epics::pvAccess::RPCRequestException& rre) {
        status = epics::pvData::Status(rre.getStatus(), rre.what());
        ok = false;
    }
    catch (std::exception& ex) {
        status = epics::pvData::Status(fatalError, ex.what());
        ok = false;
    }
    catch (...) {
        // handle user unexpected errors
        status = epics::pvData::Status(fatalError, "Unexpected exception caught while calling RPCService.request(PVStructure).");
        ok = false;
    }
    
    if (ok && result.get() == 0) {
        status = epics::pvData::Status(fatalError, "RPCService.request(PVStructure) returned null.");
    }
        
    channelRpcRequester->requestDone(status, result);
        
    if (lastRequest) {
        destroy();
    }

}

void ChannelRpcServiceImpl::request(const epics::pvData::PVStructure::shared_pointer& pvArgument, bool lastRequest) 
{
    processRequest(pvArgument, lastRequest);
}

void ChannelRpcServiceImpl::destroy()
{
    // noop
}

void ChannelRpcServiceImpl::lock()
{
    // noop
}

void ChannelRpcServiceImpl::unlock()
{
    // noop
}




