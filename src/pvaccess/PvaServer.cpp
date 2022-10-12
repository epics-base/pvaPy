// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include <boost/python.hpp>
#include <list>

#include <epicsThread.h>
#include <pv/channelProviderLocal.h>
#include "PvaException.h"
#include "ObjectAlreadyExists.h"
#include "ObjectNotFound.h"
#include "InvalidRequest.h"
#include "QueueEmpty.h"
#include "PvaServer.h"
#include "PyGilManager.h"

namespace epvd = epics::pvData;
namespace epvdb = epics::pvDatabase;

const double PvaServer::ShutdownWaitTime(0.1);
const double PvaServer::RecordUpdateTimeout(10.0);
PvaPyLogger PvaServer::logger("PvaServer");

PvaServer::PvaServer() :
    recordMap(),
    isRunning(false),
    callbackQueuePtr(new SynchronizedQueue<std::string>()),
    callbackThreadRunning(false),
    callbackThreadMutex(),
    callbackThreadExitEvent()
{
    PvObject::initializeBoostNumPy();
    start();
}

PvaServer::PvaServer(const std::string& channelName, const PvObject& pvObject) :
    recordMap(),
    isRunning(false),
    callbackQueuePtr(new SynchronizedQueue<std::string>()),
    callbackThreadRunning(false),
    callbackThreadMutex(),
    callbackThreadExitEvent()
{
    initRecord(channelName, pvObject);
    start();
}

PvaServer::PvaServer(const std::string& channelName, const PvObject& pvObject, const boost::python::object& onWriteCallback) :
    recordMap(),
    isRunning(false),
    callbackQueuePtr(new SynchronizedQueue<std::string>()),
    callbackThreadRunning(false),
    callbackThreadMutex(),
    callbackThreadExitEvent()
{
    initRecord(channelName, pvObject, onWriteCallback);
    start();
}

PvaServer::PvaServer(const PvaServer& pvaServer) :
    recordMap(),
    isRunning(false),
    callbackQueuePtr(new SynchronizedQueue<std::string>()),
    callbackThreadRunning(false),
    callbackThreadMutex(),
    callbackThreadExitEvent()
{
    start();
}

PvaServer::~PvaServer() 
{
    removeAllRecords();
    stop();
}

#if PVA_API_VERSION >= 483

void PvaServer::initAs(const std::string& filePath)
{
    initAs(filePath, "");
}

void PvaServer::initAs(const std::string& filePath, const std::string& substitutions)
{
    try {
        epvdb::ChannelProviderLocal::initAs(filePath, substitutions);
    }
    catch(std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
}

bool PvaServer::isAsActive()
{
    return epvdb::ChannelProviderLocal::isAsActive();
}

#endif // if PVA_API_VERSION >= 483

void PvaServer::start() 
{
    if (isRunning) {
        logger.warn("PVA Server is already running.");
        return;
    }
    PyGilManager::evalInitThreads();
    epics::pvDatabase::ChannelProviderLocalPtr channelProvider = epics::pvDatabase::getChannelProviderLocal();
    bool printInfo = logger.hasLogLevel(PvaPyLogger::PVAPY_LOG_LEVEL_INFO|PvaPyLogger::PVAPY_LOG_LEVEL_DEBUG);
    server = epics::pvAccess::startPVAServer(epics::pvAccess::PVACCESS_ALL_PROVIDERS, 0, true, printInfo);
    isRunning = true;
}

void PvaServer::stop() 
{
    if (!isRunning) {
        logger.warn("PVA Server is already stopped.");
        return;
    }
    server->shutdown();
    isRunning = false;
    callbackQueuePtr->cancelWaitForItemPushed();
    waitForCallbackThreadExit(ShutdownWaitTime);
}

void PvaServer::initRecord(const std::string& channelName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    PyPvRecordPtr record(PyPvRecord::create(channelName, pvStructurePtr));
    if(!record.get()) {
        throw PvaException("Failed to create PyPvRecord: " + channelName);
    }
    
    epics::pvDatabase::PVDatabasePtr master = epics::pvDatabase::PVDatabase::getMaster();
    if(!master->addRecord(record)) {
        throw PvaException("Cannot add record to master database for channel: " + channelName);
    }
    recordMap[channelName] = record;
}

void PvaServer::disableRecordProcessing(const std::string& channelName)
{
    std::map<std::string, PyPvRecordPtr>::iterator it = recordMap.find(channelName);
    if (it == recordMap.end()) {
        throw ObjectNotFound("Master database does not have record for channel: " + channelName);
    }
    it->second->disableProcessing();
}

void PvaServer::initRecord(const std::string& channelName, const PvObject& pvObject, const boost::python::object& onWriteCallback) 
{
    if (!callbackThreadRunning) {
        startCallbackThread();
    }
    PyPvRecordPtr record(PyPvRecord::create(channelName, pvObject, callbackQueuePtr, onWriteCallback));
    if(!record.get()) {
        throw PvaException("Failed to create PyPvRecord: " + channelName);
    }
    
    epics::pvDatabase::PVDatabasePtr master = epics::pvDatabase::PVDatabase::getMaster();
    if(!master->addRecord(record)) {
        throw PvaException("Cannot add record to master database for channel: " + channelName);
    }
    recordMap[channelName] = record;
}

#if PVA_API_VERSION >= 483

void PvaServer::initRecord(const std::string& channelName, const PvObject& pvObject, int asLevel, const std::string& asGroup, const boost::python::object& onWriteCallback) 
{
    if (!callbackThreadRunning) {
        startCallbackThread();
    }
    PyPvRecordPtr record(PyPvRecord::create(channelName, pvObject, asLevel, asGroup, callbackQueuePtr, onWriteCallback));
    if(!record.get()) {
        throw PvaException("Failed to create PyPvRecord: " + channelName);
    }
    
    epics::pvDatabase::PVDatabasePtr master = epics::pvDatabase::PVDatabase::getMaster();
    if(!master->addRecord(record)) {
        throw PvaException("Cannot add record to master database for channel: " + channelName);
    }
    recordMap[channelName] = record;
}

#endif // if PVA_API_VERSION >= 483

void PvaServer::update(const PvObject& pvObject) 
{
    if (recordMap.size() == 0) {
        throw InvalidRequest("Master database does not have any records.");
    }
    if (recordMap.size() != 1) {
        throw InvalidRequest("Master database has multiple records.");
    }

    std::map<std::string, PyPvRecordPtr>::iterator it = recordMap.begin();
    it->second->update(pvObject);
}

void PvaServer::update(const std::string& channelName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    std::map<std::string, PyPvRecordPtr>::iterator it = recordMap.find(channelName);
    if (it == recordMap.end()) {
        throw ObjectNotFound("Master database does not have record for channel: " + channelName);
    }
    it->second->update(pvStructurePtr);
}

void PvaServer::updateUnchecked(const std::string& channelName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    std::map<std::string, PyPvRecordPtr>::iterator it = recordMap.find(channelName);
    if (it == recordMap.end()) {
        throw ObjectNotFound("Master database does not have record for channel: " + channelName);
    }
    it->second->updateUnchecked(pvStructurePtr);
}

void PvaServer::update(const std::string& channelName, const PvObject& pvObject) 
{
    std::map<std::string, PyPvRecordPtr>::iterator it = recordMap.find(channelName);
    if (it == recordMap.end()) {
        throw ObjectNotFound("Master database does not have record for channel: " + channelName);
    }
    it->second->update(pvObject);
}

void PvaServer::addRecord(const std::string& channelName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    std::map<std::string, PyPvRecordPtr>::iterator it = recordMap.find(channelName);
    if (it != recordMap.end()) {
        throw ObjectAlreadyExists("Master database already has record for channel: " + channelName);
    }
    initRecord(channelName, pvStructurePtr);
}

void PvaServer::addRecord(const std::string& channelName, const PvObject& pvObject, const boost::python::object& onWriteCallback)
{
    std::map<std::string, PyPvRecordPtr>::iterator it = recordMap.find(channelName);
    if (it != recordMap.end()) {
        throw ObjectAlreadyExists("Master database already has record for channel: " + channelName);
    }

    initRecord(channelName, pvObject, onWriteCallback);
}

#if PVA_API_VERSION >= 483

void PvaServer::addRecordWithAs(const std::string& channelName, const PvObject& pvObject, int asLevel, const std::string& asGroup, const boost::python::object& onWriteCallback)
{
    std::map<std::string, PyPvRecordPtr>::iterator it = recordMap.find(channelName);
    if (it != recordMap.end()) {
        throw ObjectAlreadyExists("Master database already has record for channel: " + channelName);
    }

    initRecord(channelName, pvObject, asLevel, asGroup, onWriteCallback);
}

#endif // if PVA_API_VERSION >= 483

void PvaServer::removeRecord(const std::string& channelName)
{
    std::map<std::string, PyPvRecordPtr>::iterator it = recordMap.find(channelName);
    if (it == recordMap.end()) {
        throw ObjectNotFound("Master database does not have record for channel: " + channelName);
    }
    it->second->remove();
    recordMap.erase(it);
}

PyPvRecordPtr PvaServer::findRecord(const std::string& channelName)
{
    std::map<std::string, PyPvRecordPtr>::iterator it = recordMap.find(channelName);
    if (it == recordMap.end()) {
        throw ObjectNotFound("Master database does not have record for channel: " + channelName);
    }
    return it->second;
}

void PvaServer::removeAllRecords() 
{
    std::list<std::string> recordNames;
    typedef std::map<std::string, PyPvRecordPtr>::iterator MI;
    for (MI it = recordMap.begin(); it != recordMap.end(); it++) {
        recordNames.push_back(it->first);
    }

    typedef std::list<std::string>::iterator LI;
    for (LI it = recordNames.begin(); it != recordNames.end(); ++it) {
        removeRecord(*it);
    }
}

bool PvaServer::hasRecord(const std::string& channelName)
{
    if (recordMap.find(channelName) != recordMap.end()) {
        return true;
    }
    return false;
}

boost::python::list PvaServer::getRecordNames() 
{
    boost::python::list recordNames;
    typedef std::map<std::string, PyPvRecordPtr>::iterator MI;
    for (MI it = recordMap.begin(); it != recordMap.end(); it++) {
        recordNames.append(it->first);
    }
    return recordNames;
}

void PvaServer::callbackThread(PvaServer* server)
{
    server->callbackThreadRunning = true;
    logger.debug("Started PVA Server callback thread %s", epicsThreadGetNameSelf());
    while (true) {
        if (!server->isRunning) {
            break;
        }

        // Handle possible exceptions while retrieving data from empty queue.
        try {
            std::string recordName = server->callbackQueuePtr->frontAndPop(RecordUpdateTimeout);
            PyPvRecordPtr record = server->findRecord(recordName);
            if (!server->isRunning) {
                break;
            }
            record->executeCallback();
        }
        catch (ObjectNotFound& ex) {
            // Record has been deleted before we could get to update
        }
        catch (QueueEmpty& ex) {
            // Queue empty, no PV updates received.
        }
        catch (const std::exception& ex) {
            // Not good.
            logger.error("PVA Server callback thread caught exception: %s", ex.what());
        }
    }

    // Callback thread done.
    logger.debug("Exiting PVA Server callback thread %s", epicsThreadGetNameSelf());
    server->callbackQueuePtr->clear();
    server->notifyCallbackThreadExit();
    server->callbackThreadRunning = false;
}

void PvaServer::startCallbackThread()
{
    epics::pvData::Lock lock(callbackThreadMutex);
    if (!callbackThreadRunning) {
        epicsThreadCreate("CallbackThread", epicsThreadPriorityHigh, epicsThreadGetStackSize(epicsThreadStackSmall), (EPICSTHREADFUNC)callbackThread, this);
    }
    else {
        logger.warn("Callback thread is already running.");
    }
}

void PvaServer::waitForCallbackThreadExit(double timeout)
{
    if (callbackThreadRunning) {
        logger.debug("Waiting on callback thread exit, timeout in %f seconds", timeout);
        callbackThreadExitEvent.wait(timeout);
    }
}

void PvaServer::notifyCallbackThreadExit()
{
    callbackThreadExitEvent.signal();
}

