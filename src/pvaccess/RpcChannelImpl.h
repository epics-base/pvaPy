// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include <stdexcept>
#include "pv/pvAccess.h"
#include "pv/rpcServer.h"

class RpcChannelImpl : public virtual epics::pvAccess::Channel
{
public:    
    POINTER_DEFINITIONS(RpcChannelImpl);

    RpcChannelImpl(const epics::pvData::String& channelName, const epics::pvAccess::ChannelProvider::shared_pointer& channelProvider, const epics::pvAccess::ChannelRequester::shared_pointer& channelRequester, const epics::pvAccess::RPCService::shared_pointer& rpcService);
    virtual ~RpcChannelImpl();
    virtual std::tr1::shared_ptr<epics::pvAccess::ChannelProvider> getProvider();
    virtual std::tr1::shared_ptr<epics::pvAccess::ChannelRequester> getChannelRequester();
    virtual epics::pvData::String getRemoteAddress();
    virtual epics::pvData::String getChannelName();
    virtual epics::pvData::String getRequesterName();
    virtual epics::pvAccess::Channel::ConnectionState getConnectionState();
    virtual bool isConnected();

    virtual epics::pvAccess::AccessRights getAccessRights(const epics::pvData::PVField::shared_pointer& pvField);
    virtual void getField(const epics::pvAccess::GetFieldRequester::shared_pointer& requester, const epics::pvData::String& fieldName);

    virtual epics::pvAccess::ChannelProcess::shared_pointer createChannelProcess(
        const epics::pvAccess::ChannelProcessRequester::shared_pointer& channelProcessRequester, 
        const epics::pvData::PVStructure::shared_pointer& pvRequest);
    virtual epics::pvAccess::ChannelGet::shared_pointer createChannelGet(
        const epics::pvAccess::ChannelGetRequester::shared_pointer& channelGetRequester,
        const epics::pvData::PVStructure::shared_pointer& pvRequest);
    virtual epics::pvAccess::ChannelPut::shared_pointer createChannelPut(
        const epics::pvAccess::ChannelPutRequester::shared_pointer& channelPutRequester,
        const epics::pvData::PVStructure::shared_pointer& pvRequest);
    virtual epics::pvAccess::ChannelPutGet::shared_pointer createChannelPutGet(
        const epics::pvAccess::ChannelPutGetRequester::shared_pointer& channelPutGetRequester,
        const epics::pvData::PVStructure::shared_pointer& pvRequest);
    virtual epics::pvAccess::ChannelRPC::shared_pointer createChannelRPC(
        const epics::pvAccess::ChannelRPCRequester::shared_pointer& channelRPCRequester,
        const epics::pvData::PVStructure::shared_pointer& pvRequest);
    virtual epics::pvData::Monitor::shared_pointer createMonitor(
        const epics::pvData::MonitorRequester::shared_pointer& monitorRequester,
        const epics::pvData::PVStructure::shared_pointer& pvRequest);
    virtual epics::pvAccess::ChannelArray::shared_pointer createChannelArray(
        const epics::pvAccess::ChannelArrayRequester::shared_pointer& channelArrayRequester,
        const epics::pvData::PVStructure::shared_pointer& pvRequest);

    virtual void printInfo();
    virtual void printInfo(epics::pvData::StringBuilder out);
    virtual void message(const epics::pvData::String& message, epics::pvData::MessageType messageType);
    virtual void destroy();
private:

    static const epics::pvData::Status NotSupportedStatus;
    static const epics::pvData::Status DestroyedStatus;
    
    epics::pvAccess::AtomicBoolean destroyed;
    epics::pvData::String channelName;
    epics::pvAccess::ChannelProvider::shared_pointer channelProvider;
    epics::pvAccess::ChannelRequester::shared_pointer channelRequester;
    epics::pvAccess::RPCService::shared_pointer rpcService;
};

