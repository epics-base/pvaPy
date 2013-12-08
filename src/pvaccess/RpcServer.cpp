
#include "RpcServer.h"

RpcServer::RpcServer() 
    : epics::pvAccess::RPCServer()
{
}

RpcServer::~RpcServer() 
{
}

void RpcServer::registerService(const std::string& serviceName, const boost::python::object& pyService)
{
    epics::pvAccess::RPCService::shared_pointer rpcServiceImplPtr(new RpcServiceImpl(pyService));
    epics::pvAccess::RPCServer::registerService(serviceName, rpcServiceImplPtr);
}

void RpcServer::unregisterService(const std::string& serviceName)
{
    epics::pvAccess::RPCServer::unregisterService(serviceName);
}

void RpcServer::listen(int seconds)
{
    printInfo();
    run(seconds);
}
