// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "RpcChannelImpl.h"
#include "ChannelRpcServiceImpl.h"

const epics::pvData::Status RpcChannelImpl::NotSupportedStatus(epics::pvData::Status::STATUSTYPE_ERROR, "Only ChannelRPC requests are supported by this channel");
const epics::pvData::Status RpcChannelImpl::DestroyedStatus(epics::pvData::Status::STATUSTYPE_ERROR, "Channel destroyed");

RpcChannelImpl::RpcChannelImpl(const epics::pvData::String& channelName_, const epics::pvAccess::ChannelProvider::shared_pointer& channelProvider_, const epics::pvAccess::ChannelRequester::shared_pointer& channelRequester_, const epics::pvAccess::RPCService::shared_pointer& rpcService_) :
    channelName(channelName_),
    channelProvider(channelProvider_),
    channelRequester(channelRequester_),
    rpcService(rpcService_)
{
}

RpcChannelImpl::~RpcChannelImpl()
{
    destroy();
}

std::tr1::shared_ptr<epics::pvAccess::ChannelProvider> RpcChannelImpl::getProvider()
{
    return channelProvider;
}
    
epics::pvData::String RpcChannelImpl::getRemoteAddress()
{
    return getChannelName();
}

epics::pvAccess::Channel::ConnectionState RpcChannelImpl::getConnectionState()
{
    return isConnected() ?
        epics::pvAccess::Channel::CONNECTED :
        epics::pvAccess::Channel::DESTROYED;
}

epics::pvData::String RpcChannelImpl::getChannelName()
{
    return channelName;
}

std::tr1::shared_ptr<epics::pvAccess::ChannelRequester> RpcChannelImpl::getChannelRequester()
{
    return channelRequester;
}

bool RpcChannelImpl::isConnected()
{
    return !destroyed.get();
}


epics::pvAccess::AccessRights RpcChannelImpl::getAccessRights(const epics::pvData::PVField::shared_pointer& /*pvField*/)
{
    return epics::pvAccess::none;
}

void RpcChannelImpl::getField(const epics::pvAccess::GetFieldRequester::shared_pointer& requester, const epics::pvData::String& /*fieldName*/)
{
    requester->getDone(NotSupportedStatus, epics::pvData::Field::shared_pointer());    
}

epics::pvAccess::ChannelProcess::shared_pointer RpcChannelImpl::createChannelProcess(
    const epics::pvAccess::ChannelProcessRequester::shared_pointer& channelProcessRequester,
    const epics::pvData::PVStructure::shared_pointer& /*pvRequest*/)
{
    epics::pvAccess::ChannelProcess::shared_pointer nullPtr;
    channelProcessRequester->channelProcessConnect(NotSupportedStatus, nullPtr);
    return nullPtr; 
}

epics::pvAccess::ChannelGet::shared_pointer RpcChannelImpl::createChannelGet(
    const epics::pvAccess::ChannelGetRequester::shared_pointer& channelGetRequester,
    const epics::pvData::PVStructure::shared_pointer& /*pvRequest*/)
{
    epics::pvAccess::ChannelGet::shared_pointer nullPtr;
    channelGetRequester->channelGetConnect(NotSupportedStatus, nullPtr,
        epics::pvData::PVStructure::shared_pointer(), epics::pvData::BitSet::shared_pointer());
    return nullPtr; 
}
            
epics::pvAccess::ChannelPut::shared_pointer RpcChannelImpl::createChannelPut(
    const epics::pvAccess::ChannelPutRequester::shared_pointer& channelPutRequester,
    const epics::pvData::PVStructure::shared_pointer& /*pvRequest*/)
{
    epics::pvAccess::ChannelPut::shared_pointer nullPtr;
    channelPutRequester->channelPutConnect(NotSupportedStatus, nullPtr,
        epics::pvData::PVStructure::shared_pointer(), epics::pvData::BitSet::shared_pointer());
    return nullPtr; 
}
            

epics::pvAccess::ChannelPutGet::shared_pointer RpcChannelImpl::createChannelPutGet(
    const epics::pvAccess::ChannelPutGetRequester::shared_pointer& channelPutGetRequester,
    const epics::pvData::PVStructure::shared_pointer& /*pvRequest*/)
{
    epics::pvAccess::ChannelPutGet::shared_pointer nullPtr;
    epics::pvData::PVStructure::shared_pointer nullStructure;
    channelPutGetRequester->channelPutGetConnect(NotSupportedStatus, nullPtr, nullStructure, nullStructure);
    return nullPtr; 
}

epics::pvAccess::ChannelRPC::shared_pointer RpcChannelImpl::createChannelRPC(
    const epics::pvAccess::ChannelRPCRequester::shared_pointer& channelRPCRequester,
    const epics::pvData::PVStructure::shared_pointer& /*pvRequest*/)
{
    // nothing expected to be in pvRequest
    if (channelRPCRequester.get() == 0) {
        throw std::invalid_argument("channelRPCRequester == null");
    }

    if (destroyed.get()) {
        epics::pvAccess::ChannelRPC::shared_pointer nullPtr;
        channelRPCRequester->channelRPCConnect(DestroyedStatus, nullPtr);
        return nullPtr;
    }
        
    epics::pvAccess::ChannelRPC::shared_pointer channelRPCImpl(new ChannelRpcServiceImpl(channelRPCRequester, rpcService));
    channelRPCRequester->channelRPCConnect(epics::pvData::Status::Ok, channelRPCImpl);
    return channelRPCImpl;
}

epics::pvData::Monitor::shared_pointer RpcChannelImpl::createMonitor(
    const epics::pvData::MonitorRequester::shared_pointer& monitorRequester,
    const epics::pvData::PVStructure::shared_pointer& /*pvRequest*/)
{
    epics::pvData::Monitor::shared_pointer nullPtr;
    monitorRequester->monitorConnect(NotSupportedStatus, nullPtr, epics::pvData::Structure::shared_pointer());
    return nullPtr; 
}

epics::pvAccess::ChannelArray::shared_pointer RpcChannelImpl::createChannelArray(
    const epics::pvAccess::ChannelArrayRequester::shared_pointer& channelArrayRequester,
    const epics::pvData::PVStructure::shared_pointer& /*pvRequest*/)
{
    epics::pvAccess::ChannelArray::shared_pointer nullPtr;
    channelArrayRequester->channelArrayConnect(NotSupportedStatus, nullPtr, epics::pvData::PVArray::shared_pointer());
    return nullPtr; 
}
            

void RpcChannelImpl::printInfo()
{
    std::cout << "RpcChannelImpl: " << getChannelName() << " [" << epics::pvAccess::Channel::ConnectionStateNames[getConnectionState()] << "]" << std::endl;
}

void RpcChannelImpl::printInfo(epics::pvData::StringBuilder out)
{
    *out += "RpcChannelImpl: ";
    *out += getChannelName();
    *out += " [";
    *out += epics::pvAccess::Channel::ConnectionStateNames[getConnectionState()];
    *out += "]";
}

epics::pvData::String RpcChannelImpl::getRequesterName()
{
    return getChannelName();
}
    
void RpcChannelImpl::message(const epics::pvData::String& message, epics::pvData::MessageType messageType)
{
    channelRequester->message(message, messageType);
}

void RpcChannelImpl::destroy()
{
    destroyed.set();   
} 

