// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python.hpp"
#include "PyPvRecord.h"
#include "PyUtility.h"
#include "PyGilManager.h"

namespace epvdb = epics::pvDatabase;

PvaPyLogger PyPvRecord::logger("PyPvRecord");

PyPvRecordPtr PyPvRecord::create(const std::string& name, const PvObject& pvObject, const StringQueuePtr& callbackQueuePtr, const boost::python::object& onWriteCallback)
{
    PyPvRecordPtr pvRecord(new PyPvRecord(name, pvObject, callbackQueuePtr, onWriteCallback));
    if(!pvRecord->init()) {
        pvRecord.reset();
    }
    return pvRecord;
}

#if PVA_API_VERSION >= 483

PyPvRecordPtr PyPvRecord::create(const std::string& name, const PvObject& pvObject, int asLevel, const std::string& asGroup, const StringQueuePtr& callbackQueuePtr, const boost::python::object& onWriteCallback)
{
    PyPvRecordPtr pvRecord(new PyPvRecord(name, pvObject, asLevel, asGroup, callbackQueuePtr, onWriteCallback));
    if(!pvRecord->init()) {
        pvRecord.reset();
    }
    return pvRecord;
}

#endif // if PVA_API_VERSION >= 483

PyPvRecord::PyPvRecord(const std::string& name, const PvObject& pvObject, const StringQueuePtr& callbackQueuePtr_, const boost::python::object& onWriteCallback_)
    : epvdb::PVRecord(name, pvObject.getPvStructurePtr()),
    callbackQueuePtr(callbackQueuePtr_),
    onWriteCallback(onWriteCallback_)
{
    if(!PyUtility::isPyNone(onWriteCallback)) {
        PyGilManager::evalInitThreads();
    }
}

#if PVA_API_VERSION >= 483

PyPvRecord::PyPvRecord(const std::string& name, const PvObject& pvObject, int asLevel, const std::string& asGroup, const StringQueuePtr& callbackQueuePtr_, const boost::python::object& onWriteCallback_)
    : epvdb::PVRecord(name, pvObject.getPvStructurePtr(), asLevel, asGroup),
    callbackQueuePtr(callbackQueuePtr_),
    onWriteCallback(onWriteCallback_)
{
    if(!PyUtility::isPyNone(onWriteCallback)) {
        PyGilManager::evalInitThreads();
    }
}

#endif // if PVA_API_VERSION >= 483

PyPvRecord::~PyPvRecord()
{
}

bool PyPvRecord::init()
{
    initPVRecord();
    return true;
}

void PyPvRecord::process() 
{
    if(!PyUtility::isPyNone(onWriteCallback)) {
        callbackQueuePtr->push(getRecordName());
    }
    epvdb::PVRecord::process();
}

void PyPvRecord::executeCallback() 
{
    PyGilManager::gilStateEnsure();

    // Call python code
    try {
        PvObject pvObject(getPVStructure());
        onWriteCallback(pvObject);
    }
    catch(const boost::python::error_already_set&) {
        logger.error("Processing callback raised python exception.");
        PyErr_Print();
        PyErr_Clear();
    }
    catch (const std::exception& ex) {
        logger.error(ex.what());
    }

    // Release GIL.
    PyGilManager::gilStateRelease();
}

void PyPvRecord::update(const PvObject& pvObject)
{
    lock();
    try {
        beginGroupPut();
        getPVStructure()->copy(*(pvObject.getPvStructurePtr()));
        endGroupPut();
    }
    catch(...) {
        endGroupPut();
        unlock();
        throw;
    }
    unlock();
}

