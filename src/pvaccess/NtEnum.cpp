// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python.hpp"
#include "NtEnum.h"
#include "StringUtility.h"
#include "PyPvDataUtility.h"
#include "InvalidArgument.h"
#include "pv/ntenum.h"

namespace nt = epics::nt;
namespace pvd = epics::pvData;
namespace bp = boost::python;

const char* NtEnum::StructureId(nt::NTEnum::URI.c_str());

bp::dict NtEnum::createStructureDict()
{
    bp::dict structureDict;

    structureDict[ValueFieldKey] = PvEnum::createStructureDict();
    structureDict[DescriptorFieldKey] = PvType::String;
    structureDict[AlarmFieldKey] = PvAlarm::createStructureDict();
    structureDict[TimeStampFieldKey] = PvTimeStamp::createStructureDict();

    return structureDict;
}

bp::dict NtEnum::createStructureFieldIdDict()
{
    bp::dict structureFieldIdDict;
    structureFieldIdDict[ValueFieldKey] = PvEnum::StructureId;
    structureFieldIdDict[AlarmFieldKey] = PvAlarm::StructureId;
    structureFieldIdDict[TimeStampFieldKey] = PvTimeStamp::StructureId;
    return structureFieldIdDict;
}

NtEnum::NtEnum()
    : NtType(createStructureDict(), StructureId, createStructureFieldIdDict())
{
}

NtEnum::NtEnum(const bp::list& choices, int index)
    : NtType(createStructureDict(), StructureId, createStructureFieldIdDict())
{
    setValue(PvEnum(choices, index));
}

NtEnum::NtEnum(const bp::dict& pyDict, const std::string& structureId)
    : NtType(pyDict, structureId)
{
}

NtEnum::NtEnum(const PvObject& pvObject)
    : NtType(pvObject.getPvStructurePtr())
{
}

NtEnum::NtEnum(const NtEnum& ntEnum)
    : NtType(ntEnum.pvStructurePtr)
{
}

NtEnum::~NtEnum()
{
}

void NtEnum::setValue(int index)
{
    PvEnum pvEnum = getValue();
    pvEnum.setIndex(index);
    setValue(pvEnum);
}

void NtEnum::setValue(const bp::dict& pyDict)
{
    PyPvDataUtility::pyDictToStructureField(pyDict, ValueFieldKey, pvStructurePtr);
}

void NtEnum::setValue(const PvEnum& pvEnum)
{
    PyPvDataUtility::pyDictToStructureField(pvEnum, ValueFieldKey, pvStructurePtr);
}

PvEnum NtEnum::getValue() const 
{
    return PvEnum(PyPvDataUtility::getStructureField(ValueFieldKey, pvStructurePtr));
}

void NtEnum::setDescriptor(const std::string& descriptor)
{
    pvStructurePtr->getSubField<epics::pvData::PVString>(DescriptorFieldKey)->put(descriptor);
}

std::string NtEnum::getDescriptor() const
{
    return pvStructurePtr->getSubField<epics::pvData::PVString>(DescriptorFieldKey)->get();
}

PvAlarm NtEnum::getAlarm() const
{
    return PvAlarm(PyPvDataUtility::getStructureField(AlarmFieldKey, pvStructurePtr));
}

void NtEnum::setAlarm(const PvAlarm& pvAlarm)
{
    PyPvDataUtility::pyDictToStructureField(pvAlarm, AlarmFieldKey, pvStructurePtr);
}

PvTimeStamp NtEnum::getTimeStamp() const
{
    return PvTimeStamp(PyPvDataUtility::getStructureField(TimeStampFieldKey, pvStructurePtr));
}

void NtEnum::setTimeStamp(const PvTimeStamp& pvTimeStamp)
{
    PyPvDataUtility::pyDictToStructureField(pvTimeStamp, TimeStampFieldKey, pvStructurePtr);
}


