
#include "RpcServer.h"
#include "PyGilManager.h"
#include "InvalidState.h"

PvaPyLogger RpcServer::logger("RpcServer");
const double RpcServer::ShutdownWaitTime(0.1);

RpcServer::RpcServer() :
    epics::pvAccess::RPCServer(),
    destroyed(false)
{
}

RpcServer::~RpcServer() 
{
    shutdown();
    epicsThreadSleep(ShutdownWaitTime);
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

void RpcServer::startListener()
{
    if (destroyed) {
        throw InvalidState("Invalid state: server has been shutdown and cannot be restarted.");
    }

    // One must call PyEval_InitThreads() in the main thread
    // to initialize thread state, which is needed for proper functioning
    // of PyGILState_Ensure()/PyGILState_Release().
    PyGilManager::evalInitThreads();
    epicsThreadCreate("RpcServerListenerThread", epicsThreadPriorityLow, epicsThreadGetStackSize(epicsThreadStackSmall), (EPICSTHREADFUNC)listenerThread, this);
}

void RpcServer::stopListener()
{
    shutdown();
}

void RpcServer::listenerThread(RpcServer* server)
{
    logger.debug("Started listener thread %s", epicsThreadGetNameSelf());

    // Handle possible exceptions 
    try {
        server->run();
    }
    catch (const std::exception& ex) {
    // Not good.
        logger.error("Exception caught in listener thread %s: %s", epicsThreadGetNameSelf(), ex.what());
    }
}

void RpcServer::listen(int seconds)
{
    if (destroyed) {
        throw InvalidState("Invalid state: server has been shutdown and cannot be restarted.");
    }
    printInfo();
    run(seconds);
    destroyed = true;
}

void RpcServer::start()
{
    listen();
}

void RpcServer::stop()
{
    shutdown();
}

void RpcServer::shutdown()
{
    destroyed = true;
    epics::pvAccess::RPCServer::destroy();
}

