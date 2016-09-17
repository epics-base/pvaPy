// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PyPvRecord.h"

PvaPyLogger PyPvRecord::logger("PyPvRecord");

PyPvRecordPtr PyPvRecord::create(const std::string& name, const PvObject& pvObject)
{
    PyPvRecordPtr pvRecord(new PyPvRecord(name, pvObject));
    if(!pvRecord->init()) {
        pvRecord.reset();
    }
    return pvRecord;
}

PyPvRecord::PyPvRecord(const std::string& name, const PvObject& pvObject)
    : epics::pvDatabase::PVRecord(name, pvObject.getPvStructurePtr()) 
{
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

