// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python.hpp"
#include "PyPvRecord.h"
#include "PyUtility.h"
#include "PyGilManager.h"

PvaPyLogger PyPvRecord::logger("PyPvRecord");

PyPvRecordPtr PyPvRecord::create(const std::string& name, const PvObject& pvObject, const boost::python::object& onWriteCallback)
{
    PyPvRecordPtr pvRecord(new PyPvRecord(name, pvObject, onWriteCallback));
    if(!pvRecord->init()) {
        pvRecord.reset();
    }
    return pvRecord;
}

PyPvRecord::PyPvRecord(const std::string& name, const PvObject& pvObject, const boost::python::object& onWriteCallback_)
    : epics::pvDatabase::PVRecord(name, pvObject.getPvStructurePtr()),
    onWriteCallback(onWriteCallback_)
{
    if(!PyUtility::isPyNone(onWriteCallback)) {
        PyGilManager::evalInitThreads();
    }
}

PyPvRecord::~PyPvRecord()
{
}

bool PyPvRecord::init()
{
    initPVRecord();
    return true;
}

void PyPvRecord::destroy()
{
    epics::pvDatabase::PVRecord::destroy();
}

void PyPvRecord::process() 
{
    if(PyUtility::isPyNone(onWriteCallback)) {
        return;
    }
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

