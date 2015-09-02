// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvAlarm.h"
#include "PvType.h"

const char* PvAlarm::StructureId("alarm_t");

const char* PvAlarm::SeverityFieldKey("severity");
const char* PvAlarm::StatusFieldKey("status");
const char* PvAlarm::MessageFieldKey("message");

boost::python::dict PvAlarm::createStructureDict()
{
    // StandardField implementation is using signed types for some reason.
    boost::python::dict pyDict;
    pyDict[SeverityFieldKey] = PvType::Int;
    pyDict[StatusFieldKey] = PvType::Int;
    pyDict[MessageFieldKey] = PvType::String;
    return pyDict;
}

PvAlarm::PvAlarm()
    : PvObject(createStructureDict(), StructureId)
{
    setSeverity(0);
    setStatus(0);
    setMessage("");
}

PvAlarm::PvAlarm(int severity, int status, const std::string& message)
    : PvObject(createStructureDict(), StructureId)
{
    setSeverity(severity);
    setStatus(status);
    setMessage(message);
}

PvAlarm::PvAlarm(const boost::python::dict& pyDict, const std::string& structureId)
    : PvObject(pyDict, structureId)
{
}

PvAlarm::PvAlarm(const PvAlarm& pvTimeStamp)
    : PvObject(pvTimeStamp.pvStructurePtr)
{
}

PvAlarm::PvAlarm(const epics::pvData::PVStructurePtr& pvStructurePtr)
    : PvObject(pvStructurePtr)
{
}

PvAlarm::~PvAlarm()
{
}

void PvAlarm::setSeverity(int severity)
{
    pvStructurePtr->getSubField<epics::pvData::PVInt>(SeverityFieldKey)->put(severity);
}

int PvAlarm::getSeverity() const
{
    return pvStructurePtr->getSubField<epics::pvData::PVInt>(SeverityFieldKey)->get();
}

void PvAlarm::setStatus(int status)
{
    pvStructurePtr->getSubField<epics::pvData::PVInt>(StatusFieldKey)->put(status);
}

int PvAlarm::getStatus() const
{
    return pvStructurePtr->getSubField<epics::pvData::PVInt>(StatusFieldKey)->get();
}

void PvAlarm::setMessage(const std::string& message)
{
    pvStructurePtr->getSubField<epics::pvData::PVString>(MessageFieldKey)->put(message);
}

std::string PvAlarm::getMessage() const
{
    return pvStructurePtr->getSubField<epics::pvData::PVString>(MessageFieldKey)->get();
}

