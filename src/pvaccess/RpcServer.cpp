// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python.hpp"
#include "epicsThread.h"
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
    bool verbose = logger.hasLogLevel(PvaPyLogger::PVAPY_LOG_LEVEL_INFO|PvaPyLogger::PVAPY_LOG_LEVEL_DEBUG);
    if (verbose) {
        printInfo();
    }

    PyThreadState* _pyThreadState;
    _pyThreadState = PyEval_SaveThread();
    run(seconds);
    PyEval_RestoreThread(_pyThreadState);
    destroyed = true;
}

void RpcServer::start()
{
    listen(0);
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

