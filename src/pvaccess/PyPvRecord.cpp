// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include <boost/python.hpp>

#include "PyPvRecord.h"
#include "PyUtility.h"
#include "PyGilManager.h"

namespace bp = boost::python;
namespace epvd = epics::pvData;
namespace epvdb = epics::pvDatabase;

PvaPyLogger PyPvRecord::logger("PyPvRecord");

PyPvRecordPtr PyPvRecord::create(const std::string& name, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    PyPvRecordPtr pvRecord(new PyPvRecord(name, pvStructurePtr));
    if(!pvRecord->init()) {
        pvRecord.reset();
    }
    return pvRecord;
}

PyPvRecordPtr PyPvRecord::create(const std::string& name, const PvObject& pvObject, const StringQueuePtr& callbackQueuePtr, const bp::object& onWriteCallback)
{
    PyPvRecordPtr pvRecord(new PyPvRecord(name, pvObject, callbackQueuePtr, onWriteCallback));
    if(!pvRecord->init()) {
        pvRecord.reset();
    }
    return pvRecord;
}

#if PVA_API_VERSION >= 483

PyPvRecordPtr PyPvRecord::create(const std::string& name, const PvObject& pvObject, int asLevel, const std::string& asGroup, const StringQueuePtr& callbackQueuePtr, const bp::object& onWriteCallback)
{
    PyPvRecordPtr pvRecord(new PyPvRecord(name, pvObject, asLevel, asGroup, callbackQueuePtr, onWriteCallback));
    if(!pvRecord->init()) {
        pvRecord.reset();
    }
    return pvRecord;
}

#endif // if PVA_API_VERSION >= 483

PyPvRecord::PyPvRecord(const std::string& name, const epics::pvData::PVStructurePtr& pvStructurePtr)
    : epvdb::PVRecord(name, pvStructurePtr)
    , callbackQueuePtr()
    , onWriteCallback()
    , processingEnabled(true)
{
}

PyPvRecord::PyPvRecord(const std::string& name, const PvObject& pvObject, const StringQueuePtr& callbackQueuePtr_, const bp::object& onWriteCallback_)
    : epvdb::PVRecord(name, pvObject.getPvStructurePtr())
    , callbackQueuePtr(callbackQueuePtr_)
    , onWriteCallback(onWriteCallback_)
    , processingEnabled(true)
{
    if(!PyUtility::isPyNone(onWriteCallback)) {
        PyGilManager::evalInitThreads();
    }
}

#if PVA_API_VERSION >= 483

PyPvRecord::PyPvRecord(const std::string& name, const PvObject& pvObject, int asLevel, const std::string& asGroup, const StringQueuePtr& callbackQueuePtr_, const bp::object& onWriteCallback_)
    : epvdb::PVRecord(name, pvObject.getPvStructurePtr(), asLevel, asGroup)
    , callbackQueuePtr(callbackQueuePtr_)
    , onWriteCallback(onWriteCallback_)
    , processingEnabled(true)
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
    if(!processingEnabled) {
        return;
    }
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
    catch(const bp::error_already_set&) {
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
    update(pvObject.getPvStructurePtr());
}

void PyPvRecord::updateUnchecked(const PvObject& pvObject)
{
    updateUnchecked(pvObject.getPvStructurePtr());
}

void PyPvRecord::update(const epvd::PVStructurePtr& pvStructurePtr)
{
    lock();
    try {
        beginGroupPut();
        getPVStructure()->copy(*pvStructurePtr);
        endGroupPut();
    }
    catch(...) {
        endGroupPut();
        unlock();
        throw;
    }
    unlock();
}

void PyPvRecord::updateUnchecked(const epvd::PVStructurePtr& pvStructurePtr)
{
    lock();
    try {
        beginGroupPut();
        getPVStructure()->copyUnchecked(*pvStructurePtr);
        endGroupPut();
    }
    catch(...) {
        endGroupPut();
        unlock();
        throw;
    }
    unlock();
}

void PyPvRecord::disableProcessing() 
{
    processingEnabled = false;
}

