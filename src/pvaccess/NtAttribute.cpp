// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "NtAttribute.h"
#include "PvType.h"
#include "PyPvDataUtility.h"
#include "pv/ntattribute.h"

namespace nt = epics::nt;
namespace pvd = epics::pvData;
namespace bp = boost::python;

const char* NtAttribute::StructureId(nt::NTAttribute::URI.c_str());

const char* NtAttribute::NameFieldKey("name");
const char* NtAttribute::TagsFieldKey("tags");
const char* NtAttribute::SourceTypeFieldKey("sourceType");
const char* NtAttribute::SourceFieldKey("source");

bp::dict NtAttribute::createStructureDict()
{
    bp::dict structureDict;
    structureDict[NameFieldKey] = PvType::String;
    structureDict[ValueFieldKey] = bp::make_tuple();
    bp::list pyList;
    pyList.append(PvType::String);
    structureDict[TagsFieldKey] = pyList;
    structureDict[DescriptorFieldKey] = PvType::String;
    structureDict[AlarmFieldKey] = PvAlarm::createStructureDict();
    structureDict[TimeStampFieldKey] = PvTimeStamp::createStructureDict();
    structureDict[SourceTypeFieldKey] = PvType::Int;
    structureDict[SourceFieldKey] = PvType::String;
    return structureDict;
}

bp::dict NtAttribute::createStructureFieldIdDict()
{
    bp::dict structureFieldIdDict;
    structureFieldIdDict[AlarmFieldKey] = PvAlarm::StructureId;
    structureFieldIdDict[TimeStampFieldKey] = PvTimeStamp::StructureId;
    return structureFieldIdDict;
}

NtAttribute::NtAttribute()
    : NtType(createStructureDict(), StructureId, createStructureFieldIdDict())
{
}

NtAttribute::NtAttribute(const std::string& name, const PvObject& value)
    : NtType(createStructureDict(), StructureId, createStructureFieldIdDict())
{
    setName(name);
    setValue(value);
}

NtAttribute::NtAttribute(const boost::python::dict& structureDict, const std::string& structureId)
    : NtType(structureDict, structureId)
{
}

NtAttribute::NtAttribute(const NtAttribute& ntAttribute)
    : NtType(ntAttribute.pvStructurePtr)
{
}

NtAttribute::NtAttribute(const epics::pvData::PVStructurePtr& pvStructurePtr)
    : NtType(pvStructurePtr)
{
}

NtAttribute::~NtAttribute()
{
}

void NtAttribute::setName(const std::string& name)
{
    pvStructurePtr->getSubField<pvd::PVString>(NameFieldKey)->put(name);
}

std::string NtAttribute::getName() const
{
    return pvStructurePtr->getSubField<pvd::PVString>(NameFieldKey)->get();
}

void NtAttribute::setValue(const PvObject& value)
{
    setUnion(ValueFieldKey, value);
}

PvObject NtAttribute::getValue() const
{
    return getUnion(ValueFieldKey);
}

void NtAttribute::setTags(const boost::python::list& pyList)
{
    PyPvDataUtility::pyListToScalarArrayField(pyList, TagsFieldKey, pvStructurePtr);
}

boost::python::list NtAttribute::getTags() const
{
    boost::python::list pyList;
    PyPvDataUtility::scalarArrayFieldToPyList(TagsFieldKey, pvStructurePtr, pyList);
    return pyList;
}

void NtAttribute::setDescriptor(const std::string& descriptor)
{
    pvStructurePtr->getSubField<epics::pvData::PVString>(DescriptorFieldKey)->put(descriptor);
}

std::string NtAttribute::getDescriptor() const
{
    return pvStructurePtr->getSubField<epics::pvData::PVString>(DescriptorFieldKey)->get();
}

PvTimeStamp NtAttribute::getTimeStamp() const
{
    return PvTimeStamp(PyPvDataUtility::getStructureField(TimeStampFieldKey, pvStructurePtr));
}

void NtAttribute::setTimeStamp(const PvTimeStamp& pvTimeStamp)
{
    PyPvDataUtility::pyDictToStructureField(pvTimeStamp, TimeStampFieldKey, pvStructurePtr);
}

PvAlarm NtAttribute::getAlarm() const
{
    return PvAlarm(PyPvDataUtility::getStructureField(AlarmFieldKey, pvStructurePtr));
}

void NtAttribute::setAlarm(const PvAlarm& pvAlarm)
{
    PyPvDataUtility::pyDictToStructureField(pvAlarm, AlarmFieldKey, pvStructurePtr);
}

void NtAttribute::setSourceType(int value)
{
    pvStructurePtr->getSubField<pvd::PVInt>(SourceTypeFieldKey)->put(value);
}

int NtAttribute::getSourceType() const
{
    return pvStructurePtr->getSubField<pvd::PVInt>(SourceTypeFieldKey)->get();
}

void NtAttribute::setSource(const std::string& source)
{
    pvStructurePtr->getSubField<pvd::PVString>(SourceFieldKey)->put(source);
}

std::string NtAttribute::getSource() const
{
    return pvStructurePtr->getSubField<pvd::PVString>(SourceFieldKey)->get();
}

