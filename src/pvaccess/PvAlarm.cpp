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
    pvStructurePtr->getIntField(SeverityFieldKey)->put(severity);
}

int PvAlarm::getSeverity() const
{
    return pvStructurePtr->getIntField(SeverityFieldKey)->get();
}

void PvAlarm::setStatus(int status)
{
    pvStructurePtr->getIntField(StatusFieldKey)->put(status);
}

int PvAlarm::getStatus() const
{
    return pvStructurePtr->getIntField(StatusFieldKey)->get();
}

void PvAlarm::setMessage(const std::string& message)
{
    pvStructurePtr->getStringField(MessageFieldKey)->put(message);
}

std::string PvAlarm::getMessage() const
{
    return pvStructurePtr->getStringField(MessageFieldKey)->get();
}

