// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python.hpp"
#include "NtNdArray.h"
#include "StringUtility.h"
#include "PyPvDataUtility.h"
#include "InvalidArgument.h"
#include "NtAttribute.h"
#include "pv/ntndarray.h"

namespace nt = epics::nt;
namespace pvd = epics::pvData;
namespace bp = boost::python;

const char* NtNdArray::StructureId(nt::NTNDArray::URI.c_str());

const char* NtNdArray::BooleanValueFieldKey("booleanValue");
const char* NtNdArray::ByteValueFieldKey("byteValue");
const char* NtNdArray::UByteValueFieldKey("ubyteValue");
const char* NtNdArray::ShortValueFieldKey("shortValue");
const char* NtNdArray::UShortValueFieldKey("ushortValue");
const char* NtNdArray::IntValueFieldKey("intValue");
const char* NtNdArray::UIntValueFieldKey("uintValue");
const char* NtNdArray::LongValueFieldKey("longValue");
const char* NtNdArray::ULongValueFieldKey("ulongValue");
const char* NtNdArray::FloatValueFieldKey("floatValue");
const char* NtNdArray::DoubleValueFieldKey("doubleValue");

const char* NtNdArray::AlarmFieldKey("alarm");
const char* NtNdArray::AttributeFieldKey("attribute");
const char* NtNdArray::CodecFieldKey("codec");
const char* NtNdArray::CompressedSizeFieldKey("compressedSize");
const char* NtNdArray::DataTimeStampFieldKey("dataTimeStamp");
const char* NtNdArray::DescriptorFieldKey("descriptor");
const char* NtNdArray::DimensionFieldKey("dimension");
const char* NtNdArray::DisplayFieldKey("display");
const char* NtNdArray::TimeStampFieldKey("timeStamp");
const char* NtNdArray::UncompressedSizeFieldKey("uncompressedSize");
const char* NtNdArray::UniqueIdFieldKey("uniqueId");

bp::dict NtNdArray::createStructureDict(const bp::dict& extraFieldsDict)
{
    bp::dict structureDict;
    bp::dict valueDict;
    valueDict[BooleanValueFieldKey] = PyPvDataUtility::createStructureList(PvType::Boolean);
    valueDict[ByteValueFieldKey] = PyPvDataUtility::createStructureList(PvType::Byte);
    valueDict[UByteValueFieldKey] = PyPvDataUtility::createStructureList(PvType::UByte);
    valueDict[ShortValueFieldKey] = PyPvDataUtility::createStructureList(PvType::Short);
    valueDict[UShortValueFieldKey] = PyPvDataUtility::createStructureList(PvType::UShort);
    valueDict[IntValueFieldKey] = PyPvDataUtility::createStructureList(PvType::Int);
    valueDict[UIntValueFieldKey] = PyPvDataUtility::createStructureList(PvType::UInt);
    valueDict[LongValueFieldKey] = PyPvDataUtility::createStructureList(PvType::Long);
    valueDict[ULongValueFieldKey] = PyPvDataUtility::createStructureList(PvType::ULong);
    valueDict[FloatValueFieldKey] = PyPvDataUtility::createStructureList(PvType::Float);
    valueDict[DoubleValueFieldKey] = PyPvDataUtility::createStructureList(PvType::Double);
    bp::tuple valueTuple = bp::make_tuple(valueDict);
    structureDict[ValueFieldKey] = valueTuple;
    structureDict[CodecFieldKey] = PvCodec::createStructureDict();
    structureDict[CompressedSizeFieldKey] = PvType::Long;
    structureDict[UncompressedSizeFieldKey] = PvType::Long;
    structureDict[DimensionFieldKey] = PyPvDataUtility::createStructureList(PvDimension::createStructureDict());
    structureDict[UniqueIdFieldKey] = PvType::Int;
    structureDict[DataTimeStampFieldKey] = PvTimeStamp::createStructureDict();
    structureDict[AttributeFieldKey] = PyPvDataUtility::createStructureList(NtAttribute::createStructureDict());
    structureDict[DescriptorFieldKey] = PvType::String;
    structureDict[AlarmFieldKey] = PvAlarm::createStructureDict();
    structureDict[TimeStampFieldKey] = PvTimeStamp::createStructureDict();
    structureDict[DisplayFieldKey] = PvDisplay::createStructureDict();

    // Extra fields
    bp::list keys = extraFieldsDict.keys();
    for (int i = 0; i < bp::len(keys); i++) {
        bp::object keyObject = keys[i];
        bp::extract<std::string> keyExtract(keyObject);
        std::string key;
        if (keyExtract.check()) {
            key = keyExtract();
        }
        else {
            throw InvalidDataType("Extra fields dictionary key must be a string");
        }
        structureDict[key] = extraFieldsDict[key];
    }
    return structureDict;
}

bp::dict NtNdArray::createStructureFieldIdDict()
{
    bp::dict structureFieldIdDict;
    structureFieldIdDict[CodecFieldKey] = PvCodec::StructureId;
    structureFieldIdDict[DataTimeStampFieldKey] = PvTimeStamp::StructureId;
    structureFieldIdDict[DimensionFieldKey] = PvDimension::StructureId;
    structureFieldIdDict[AttributeFieldKey] = NtAttribute::StructureId;
    structureFieldIdDict[AlarmFieldKey] = PvAlarm::StructureId;
    structureFieldIdDict[TimeStampFieldKey] = PvTimeStamp::StructureId;
    structureFieldIdDict[DisplayFieldKey] = PvDisplay::StructureId;
    return structureFieldIdDict;
}

NtNdArray::NtNdArray()
    : NtType(createStructureDict(), StructureId, createStructureFieldIdDict())
{
}

NtNdArray::NtNdArray(const bp::dict& extraFieldsDict)
    : NtType(createStructureDict(extraFieldsDict), StructureId, createStructureFieldIdDict())
{
}

NtNdArray::NtNdArray(const PvObject& pvObject)
    : NtType(pvObject.getPvStructurePtr())
{
}

NtNdArray::NtNdArray(const NtNdArray& ntNdArray)
    : NtType(ntNdArray.pvStructurePtr)
{
}

NtNdArray::~NtNdArray()
{
}

void NtNdArray::setValue(const bp::dict& pyDict)
{
    setUnion(pyDict);
}

void NtNdArray::setValue(const PvObject& pvObject)
{
    setUnion(pvObject);
}

bp::object NtNdArray::getValue() const 
{
    return getUnion();
}

void NtNdArray::setCodec(const PvCodec& pvCodec)
{
    PyPvDataUtility::pyDictToStructureField(pvCodec, CodecFieldKey, pvStructurePtr);
}

PvCodec NtNdArray::getCodec() const
{
    return PvCodec(PyPvDataUtility::getStructureField(CodecFieldKey, pvStructurePtr));
}

void NtNdArray::setCompressedSize(int value)
{
    pvStructurePtr->getSubField<epics::pvData::PVInt>(CompressedSizeFieldKey)->put(value);
}

int NtNdArray::getCompressedSize() const
{
    return pvStructurePtr->getSubField<epics::pvData::PVInt>(CompressedSizeFieldKey)->get();
}

void NtNdArray::setUncompressedSize(int value)
{
    pvStructurePtr->getSubField<epics::pvData::PVInt>(UncompressedSizeFieldKey)->put(value);
}

int NtNdArray::getUncompressedSize() const
{
    return pvStructurePtr->getSubField<epics::pvData::PVInt>(UncompressedSizeFieldKey)->get();
}

void NtNdArray::setDimension(const bp::list& pyList)
{
    setStructureArray(DimensionFieldKey, pyList);
}

bp::list NtNdArray::getDimension() const
{
    return getStructureArray(DimensionFieldKey);
}

void NtNdArray::setUniqueId(int value)
{
    pvStructurePtr->getSubField<epics::pvData::PVInt>(UniqueIdFieldKey)->put(value);
}

int NtNdArray::getUniqueId() const 
{
    return pvStructurePtr->getSubField<epics::pvData::PVInt>(UniqueIdFieldKey)->get();
}

PvTimeStamp NtNdArray::getDataTimeStamp() const
{
    return PvTimeStamp(PyPvDataUtility::getStructureField(DataTimeStampFieldKey, pvStructurePtr));
}

void NtNdArray::setDataTimeStamp(const PvTimeStamp& pvTimeStamp)
{
    PyPvDataUtility::pyDictToStructureField(pvTimeStamp, DataTimeStampFieldKey, pvStructurePtr);
}

void NtNdArray::setAttribute(const bp::list& pyList)
{
    setStructureArray(AttributeFieldKey, pyList);
}

bp::list NtNdArray::getAttribute() const 
{
    return getStructureArray(AttributeFieldKey);
}

void NtNdArray::setDescriptor(const std::string& descriptor)
{
    pvStructurePtr->getSubField<epics::pvData::PVString>(DescriptorFieldKey)->put(descriptor);
}

std::string NtNdArray::getDescriptor() const
{
    return pvStructurePtr->getSubField<epics::pvData::PVString>(DescriptorFieldKey)->get();
}

PvAlarm NtNdArray::getAlarm() const
{
    return PvAlarm(PyPvDataUtility::getStructureField(AlarmFieldKey, pvStructurePtr));
}

void NtNdArray::setAlarm(const PvAlarm& pvAlarm)
{
    PyPvDataUtility::pyDictToStructureField(pvAlarm, AlarmFieldKey, pvStructurePtr);
}

PvTimeStamp NtNdArray::getTimeStamp() const
{
    return PvTimeStamp(PyPvDataUtility::getStructureField(TimeStampFieldKey, pvStructurePtr));
}

void NtNdArray::setTimeStamp(const PvTimeStamp& pvTimeStamp)
{
    PyPvDataUtility::pyDictToStructureField(pvTimeStamp, TimeStampFieldKey, pvStructurePtr);
}

PvDisplay NtNdArray::getDisplay() const
{
    return PvDisplay(PyPvDataUtility::getStructureField(DisplayFieldKey, pvStructurePtr));
}

void NtNdArray::setDisplay(const PvDisplay& pvDisplay)
{
    PyPvDataUtility::pyDictToStructureField(pvDisplay, DisplayFieldKey, pvStructurePtr);
}

