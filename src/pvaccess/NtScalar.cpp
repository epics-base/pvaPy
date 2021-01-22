// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include <boost/python.hpp>
#include "NtScalar.h"
#include "StringUtility.h"
#include "PyPvDataUtility.h"
#include "InvalidArgument.h"

namespace bp = boost::python;
namespace epvd = epics::pvData;

const char* NtScalar::StructureId("epics:nt/NTScalar:1.0");
const char* NtScalar::DescriptorFieldKey("descriptor");
const char* NtScalar::TimeStampFieldKey("timeStamp");
const char* NtScalar::AlarmFieldKey("alarm");
const char* NtScalar::DisplayFieldKey("display");
const char* NtScalar::ControlFieldKey("control");

boost::python::dict NtScalar::createStructureDict(PvType::ScalarType scalarType)
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = scalarType;
    pyDict[DescriptorFieldKey] = PvType::String;
    pyDict[AlarmFieldKey] = PvAlarm::createStructureDict();
    pyDict[TimeStampFieldKey] = PvTimeStamp::createStructureDict();
    pyDict[DisplayFieldKey] = PvDisplay::createStructureDict();
    pyDict[ControlFieldKey] = PvControl::createStructureDict();
    return pyDict;
}

NtScalar::NtScalar(PvType::ScalarType scalarType)
    : NtType(createStructureDict(scalarType), StructureId)
{
}

NtScalar::NtScalar(PvType::ScalarType scalarType, const boost::python::object& pyObject)
    : NtType(createStructureDict(scalarType), StructureId)
{
    setPyObject(ValueFieldKey, pyObject);
}

NtScalar::NtScalar(const PvObject& pvObject)
    : NtType(pvObject.getPvStructurePtr())
{
}

NtScalar::NtScalar(const NtScalar& ntScalar)
    : NtType(ntScalar.pvStructurePtr)
{
}

NtScalar::~NtScalar()
{
}

void NtScalar::setValue(const bp::object& pyObject)
{
    setPyObject(ValueFieldKey, pyObject);
}

bp::object NtScalar::getValue() const
{
    return getPyObject(ValueFieldKey);
}

void NtScalar::setDescriptor(const std::string& descriptor)
{
    pvStructurePtr->getSubField<epics::pvData::PVString>(DescriptorFieldKey)->put(descriptor);
}

std::string NtScalar::getDescriptor() const
{
    return pvStructurePtr->getSubField<epics::pvData::PVString>(DescriptorFieldKey)->get();
}

PvTimeStamp NtScalar::getTimeStamp() const
{
    return PvTimeStamp(PyPvDataUtility::getStructureField(TimeStampFieldKey, pvStructurePtr));
}

void NtScalar::setTimeStamp(const PvTimeStamp& pvTimeStamp)
{
    PyPvDataUtility::pyDictToStructureField(pvTimeStamp, TimeStampFieldKey, pvStructurePtr);
}

PvAlarm NtScalar::getAlarm() const
{
    return PvAlarm(PyPvDataUtility::getStructureField(AlarmFieldKey, pvStructurePtr));
}

void NtScalar::setAlarm(const PvAlarm& pvAlarm)
{
    PyPvDataUtility::pyDictToStructureField(pvAlarm, AlarmFieldKey, pvStructurePtr);
}

PvDisplay NtScalar::getDisplay() const
{
    return PvDisplay(PyPvDataUtility::getStructureField(DisplayFieldKey, pvStructurePtr));
}

void NtScalar::setDisplay(const PvDisplay& pvDisplay)
{
    PyPvDataUtility::pyDictToStructureField(pvDisplay, DisplayFieldKey, pvStructurePtr);
}

PvControl NtScalar::getControl() const
{
    return PvControl(PyPvDataUtility::getStructureField(ControlFieldKey, pvStructurePtr));
}

void NtScalar::setControl(const PvControl& pvControl)
{
    PyPvDataUtility::pyDictToStructureField(pvControl, ControlFieldKey, pvStructurePtr);
}

