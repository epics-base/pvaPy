// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvValueAlarm.h"
#include "PvType.h"

const char* PvValueAlarm::StructureId("valueAlarm_t");

const char* PvValueAlarm::ActiveFieldKey("active");
const char* PvValueAlarm::LowAlarmLimitFieldKey("lowAlarmLimit");
const char* PvValueAlarm::LowWarningLimitFieldKey("lowWarningLimit");
const char* PvValueAlarm::HighWarningLimitFieldKey("highWarningLimit");
const char* PvValueAlarm::HighAlarmLimitFieldKey("highAlarmLimit");
const char* PvValueAlarm::LowAlarmSeverityFieldKey("lowAlarmSeverity");
const char* PvValueAlarm::LowWarningSeverityFieldKey("lowWarningSeverity");
const char* PvValueAlarm::HighWarningSeverityFieldKey("highWarningSeverity");
const char* PvValueAlarm::HighAlarmSeverityFieldKey("highAlarmSeverity");
const char* PvValueAlarm::HysteresisFieldKey("hysteresis");

namespace bp = boost::python;
namespace epvd = epics::pvData;

boost::python::dict PvValueAlarm::createStructureDict(PvType::ScalarType scalarType)
{
    // StandardField implementation is using signed types for some reason.
    bp::dict pyDict;
    pyDict[ActiveFieldKey] = PvType::Boolean;
    pyDict[LowAlarmLimitFieldKey] = scalarType;
    pyDict[LowWarningLimitFieldKey] = scalarType;
    pyDict[HighWarningLimitFieldKey] = scalarType;
    pyDict[HighAlarmLimitFieldKey] = scalarType;
    pyDict[LowAlarmSeverityFieldKey] = PvType::Int;
    pyDict[LowWarningSeverityFieldKey] = PvType::Int;
    pyDict[HighWarningSeverityFieldKey] = PvType::Int;
    pyDict[HighAlarmSeverityFieldKey] = PvType::Int;
    pyDict[HysteresisFieldKey] = PvType::Byte;
    return pyDict;
}

PvValueAlarm::PvValueAlarm(PvType::ScalarType scalarType)
    : PvObject(createStructureDict(scalarType), StructureId)
{
    setActive(false);
}

PvValueAlarm::PvValueAlarm(const boost::python::dict& pyDict, const std::string& structureId)
    : PvObject(pyDict, structureId)
{
}

PvValueAlarm::PvValueAlarm(const PvValueAlarm& pvValueAlarm)
    : PvObject(pvValueAlarm.pvStructurePtr)
{
}

PvValueAlarm::PvValueAlarm(const epics::pvData::PVStructurePtr& pvStructurePtr)
    : PvObject(pvStructurePtr)
{
}

PvValueAlarm::~PvValueAlarm()
{
}

void PvValueAlarm::setActive(bool value)
{
    pvStructurePtr->getSubField<epvd::PVBoolean>(ActiveFieldKey)->put(value);
}

bool PvValueAlarm::getActive() const
{
    return pvStructurePtr->getSubField<epvd::PVBoolean>(ActiveFieldKey)->get();
}

void PvValueAlarm::setLowAlarmLimit(const bp::object& pyObject)
{
    setPyObject(LowAlarmLimitFieldKey, pyObject);
}

bp::object PvValueAlarm::getLowAlarmLimit() const
{
    return getPyObject(LowAlarmLimitFieldKey);
}

void PvValueAlarm::setLowWarningLimit(const bp::object& pyObject)
{
    setPyObject(LowWarningLimitFieldKey, pyObject);
}

bp::object PvValueAlarm::getLowWarningLimit() const
{
    return getPyObject(LowWarningLimitFieldKey);
}

void PvValueAlarm::setHighWarningLimit(const bp::object& pyObject)
{
    setPyObject(HighWarningLimitFieldKey, pyObject);
}

bp::object PvValueAlarm::getHighWarningLimit() const
{
    return getPyObject(HighWarningLimitFieldKey);
}

void PvValueAlarm::setHighAlarmLimit(const bp::object& pyObject)
{
    setPyObject(HighAlarmLimitFieldKey, pyObject);
}

bp::object PvValueAlarm::getHighAlarmLimit() const
{
    return getPyObject(HighAlarmLimitFieldKey);
}

void PvValueAlarm::setLowAlarmSeverity(int value)
{
    pvStructurePtr->getSubField<epvd::PVInt>(LowAlarmSeverityFieldKey)->put(value);
}

int PvValueAlarm::getLowAlarmSeverity() const
{
    return pvStructurePtr->getSubField<epvd::PVInt>(LowAlarmSeverityFieldKey)->get();
}

void PvValueAlarm::setLowWarningSeverity(int value)
{
    pvStructurePtr->getSubField<epvd::PVInt>(LowWarningSeverityFieldKey)->put(value);
}

int PvValueAlarm::getLowWarningSeverity() const
{
    return pvStructurePtr->getSubField<epvd::PVInt>(LowWarningSeverityFieldKey)->get();
}

void PvValueAlarm::setHighWarningSeverity(int value)
{
    pvStructurePtr->getSubField<epvd::PVInt>(HighWarningSeverityFieldKey)->put(value);
}

int PvValueAlarm::getHighWarningSeverity() const
{
    return pvStructurePtr->getSubField<epvd::PVInt>(HighWarningSeverityFieldKey)->get();
}

void PvValueAlarm::setHighAlarmSeverity(int value)
{
    pvStructurePtr->getSubField<epvd::PVInt>(HighAlarmSeverityFieldKey)->put(value);
}

int PvValueAlarm::getHighAlarmSeverity() const
{
    return pvStructurePtr->getSubField<epvd::PVInt>(HighAlarmSeverityFieldKey)->get();
}

void PvValueAlarm::setHysteresis(char value)
{
    pvStructurePtr->getSubField<epvd::PVByte>(HysteresisFieldKey)->put(value);
}

char PvValueAlarm::getHysteresis() const
{
    return pvStructurePtr->getSubField<epvd::PVByte>(HysteresisFieldKey)->get();
}


