#include <stdlib.h>
#include <sstream>

#include "PvScalar.h"
#include "PvType.h"
#include "PyPvDataUtility.h"
#include "StringUtility.h"

PvScalar::PvScalar(const boost::python::dict& pyDict)
    : PvObject(pyDict)
{
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
            value = pvStructurePtr->getBooleanField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvByte: {
            value = pvStructurePtr->getByteField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvUByte: {
            value = pvStructurePtr->getUByteField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvShort: {
            value = pvStructurePtr->getShortField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvUShort: {
            value = pvStructurePtr->getUShortField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvInt: {
            value = pvStructurePtr->getIntField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvUInt: {
            value = pvStructurePtr->getUIntField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvLong: {
            value = pvStructurePtr->getLongField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvULong: {
            value = pvStructurePtr->getULongField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvFloat: {
            value = pvStructurePtr->getFloatField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvDouble: {
            value = pvStructurePtr->getDoubleField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvString: {
            std::string s = pvStructurePtr->getStringField(ValueFieldKey)->get();
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
            oss << static_cast<bool>(pvStructurePtr->getBooleanField(ValueFieldKey)->get());
            break;
        }
        case epics::pvData::pvByte: {
            oss << pvStructurePtr->getByteField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvUByte: {
            oss << pvStructurePtr->getUByteField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvShort: {
            oss << pvStructurePtr->getShortField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvUShort: {
            oss << pvStructurePtr->getUShortField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvInt: {
            oss << pvStructurePtr->getIntField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvUInt: {
            oss << pvStructurePtr->getUIntField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvLong: {
            oss << pvStructurePtr->getLongField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvULong: {
            oss << pvStructurePtr->getULongField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvFloat: {
            oss << pvStructurePtr->getFloatField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvDouble: {
            oss << pvStructurePtr->getDoubleField(ValueFieldKey)->get();
            break;
        }
        case epics::pvData::pvString: {
            oss << pvStructurePtr->getStringField(ValueFieldKey)->get();
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
            epics::pvData::PVBooleanPtr fieldPtr = pvStructurePtr->getBooleanField(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvByte: {
            epics::pvData::PVBytePtr fieldPtr = pvStructurePtr->getByteField(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvUByte: {
            epics::pvData::PVUBytePtr fieldPtr = pvStructurePtr->getUByteField(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvShort: {
            epics::pvData::PVShortPtr fieldPtr = pvStructurePtr->getShortField(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvUShort: {
            epics::pvData::PVUShortPtr fieldPtr = pvStructurePtr->getUShortField(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvInt: {
            epics::pvData::PVIntPtr fieldPtr = pvStructurePtr->getIntField(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvUInt: {
            epics::pvData::PVUIntPtr fieldPtr = pvStructurePtr->getUIntField(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvLong: {
            epics::pvData::PVLongPtr fieldPtr = pvStructurePtr->getLongField(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvULong: {
            epics::pvData::PVULongPtr fieldPtr = pvStructurePtr->getULongField(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvFloat: {
            epics::pvData::PVFloatPtr fieldPtr = pvStructurePtr->getFloatField(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvDouble: {
            epics::pvData::PVDoublePtr fieldPtr = pvStructurePtr->getDoubleField(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+i);
            break;
        }
        case epics::pvData::pvString: {
            epics::pvData::PVStringPtr fieldPtr = pvStructurePtr->getStringField(ValueFieldKey);
            fieldPtr->put(fieldPtr->get()+StringUtility::toString(i));
            break;
        }
        default: {
            throw PvaException("Unrecognized scalar type: %d", scalarType);
        }
    }
    return *this;
}
