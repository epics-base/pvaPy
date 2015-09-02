// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include <stdlib.h>
#include <sstream>

#include "PvScalar.h"
#include "PvType.h"
#include "PyPvDataUtility.h"
#include "StringUtility.h"

PvScalar::PvScalar(const boost::python::dict& pyDict)
    : PvObject(pyDict)
{
    dataType = PvType::Scalar;
}

PvScalar::PvScalar(const PvScalar& pvScalar)
    : PvObject(pvScalar.pvStructurePtr)
{
}

PvScalar::~PvScalar()
{
}

PvScalar::operator int() const
{
    return (int)(toDouble());
}

PvScalar::operator double() const
{
    return toDouble();
}

int PvScalar::toInt() const
{
    int i = toDouble();
    return i;
}

long long PvScalar::toLongLong() const
{
    long long ll = toDouble();
    return ll;
}

double PvScalar::toDouble() const
{
    epics::pvData::ScalarType scalarType = PyPvDataUtility::getScalarType(ValueFieldKey, pvStructurePtr);
    double value;
    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            value = pvStructurePtr->getSubField<epics::pvData::PVBoolean>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvByte: {
            value = pvStructurePtr->getSubField<epics::pvData::PVByte>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvUByte: {
            value = pvStructurePtr->getSubField<epics::pvData::PVUByte>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvShort: {
            value = pvStructurePtr->getSubField<epics::pvData::PVShort>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvUShort: {
            value = pvStructurePtr->getSubField<epics::pvData::PVUShort>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvInt: {
            value = pvStructurePtr->getSubField<epics::pvData::PVInt>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvUInt: {
            value = pvStructurePtr->getSubField<epics::pvData::PVUInt>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvLong: {
            value = pvStructurePtr->getSubField<epics::pvData::PVLong>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvULong: {
            value = pvStructurePtr->getSubField<epics::pvData::PVULong>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvFloat: {
            value = pvStructurePtr->getSubField<epics::pvData::PVFloat>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvDouble: {
            value = pvStructurePtr->getSubField<epics::pvData::PVDouble>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvString: {
            std::string s = pvStructurePtr->getSubField<epics::pvData::PVString>(ValueFieldKey)->get();
            value = atof(s.c_str());
            break;
        }
        default: {
            throw PvaException("Unrecognized scalar type: %d", scalarType);
        }
    }
    return value;
}

std::string PvScalar::toString() const
{
    std::ostringstream oss;
    epics::pvData::ScalarType scalarType = PyPvDataUtility::getScalarType(ValueFieldKey, pvStructurePtr);
    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            oss << static_cast<bool>(pvStructurePtr->getSubField<epics::pvData::PVBoolean>(ValueFieldKey)->get());
            break;
        }
        case epics::pvData::pvByte: {
            oss << pvStructurePtr->getSubField<epics::pvData::PVByte>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvUByte: {
            oss << pvStructurePtr->getSubField<epics::pvData::PVUByte>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvShort: {
            oss << pvStructurePtr->getSubField<epics::pvData::PVShort>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvUShort: {
            oss << pvStructurePtr->getSubField<epics::pvData::PVUShort>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvInt: {
            oss << pvStructurePtr->getSubField<epics::pvData::PVInt>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvUInt: {
            oss << pvStructurePtr->getSubField<epics::pvData::PVUInt>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvLong: {
            oss << pvStructurePtr->getSubField<epics::pvData::PVLong>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvULong: {
            oss << pvStructurePtr->getSubField<epics::pvData::PVULong>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvFloat: {
            oss << pvStructurePtr->getSubField<epics::pvData::PVFloat>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvDouble: {
            oss << pvStructurePtr->getSubField<epics::pvData::PVDouble>(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvString: {
            oss << pvStructurePtr->getSubField<epics::pvData::PVString>(ValueFieldKey)->get();
            break;
        }
        default: {
            throw PvaException("Unrecognized scalar type: %d", scalarType);
        }
    }
    return oss.str();
}

PvScalar& PvScalar::add(int i)
{
    epics::pvData::ScalarType scalarType = PyPvDataUtility::getScalarType(ValueFieldKey, pvStructurePtr);
    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            epics::pvData::PVBooleanPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVBoolean>(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvByte: {
            epics::pvData::PVBytePtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVByte>(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvUByte: {
            epics::pvData::PVUBytePtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVUByte>(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvShort: {
            epics::pvData::PVShortPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVShort>(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvUShort: {
            epics::pvData::PVUShortPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVUShort>(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvInt: {
            epics::pvData::PVIntPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVInt>(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvUInt: {
            epics::pvData::PVUIntPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVUInt>(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvLong: {
            epics::pvData::PVLongPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVLong>(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvULong: {
            epics::pvData::PVULongPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVULong>(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvFloat: {
            epics::pvData::PVFloatPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVFloat>(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvDouble: {
            epics::pvData::PVDoublePtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVDouble>(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvString: {
            epics::pvData::PVStringPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVString>(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+StringUtility::toString(i));
            break;
        }
        default: {
            throw PvaException("Unrecognized scalar type: %d", scalarType);
        }
    }
    return *this;
}
