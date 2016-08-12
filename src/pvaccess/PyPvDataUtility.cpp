// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PyPvDataUtility.h"
#include "PvType.h"
#include "PvaConstants.h"
#include "FieldNotFound.h"
#include "InvalidDataType.h"
#include "InvalidArgument.h"
#include "InvalidRequest.h"
#include "PvObject.h"

// Scalar array utilities
namespace PyPvDataUtility
{

//
// Checks
//
void checkFieldExists(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::PVFieldPtr pvFieldPtr = pvStructurePtr->getSubField(fieldName);
    if (!pvFieldPtr) {
        throw FieldNotFound("Object does not have field " + fieldName);
    }
}

void checkFieldPathExists(const std::string& fieldPath, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    std::vector<std::string> fieldNames = StringUtility::split(fieldPath);
    epics::pvData::PVStructurePtr pvStructurePtr2 = getParentStructureForFieldPath(fieldNames, pvStructurePtr);
    int nElements = fieldNames.size();
    
    // Last field in the path is what we want.
    std::string fieldName = fieldNames[nElements-1];
    checkFieldExists(fieldName, pvStructurePtr2);
}

//
// Field retrieval
//
std::string getValueOrSingleFieldName(const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    // If structure has value field key, return it.
    epics::pvData::StructureConstPtr structurePtr = pvStructurePtr->getStructure();
    epics::pvData::StringArray fieldNames = structurePtr->getFieldNames();
    for (unsigned int i = 0; i < fieldNames.size(); i++) {
        std::string fieldName = fieldNames[i];
        if (fieldName == PvaConstants::ValueFieldKey) {
            return PvaConstants::ValueFieldKey;
        }
    }
    if (fieldNames.size() > 1) {
        throw InvalidRequest("Ambiguous request: object has multiple fields, but no %s field", PvaConstants::ValueFieldKey);
    }
    // Structure has only one field key.
    return fieldNames[0];
}

std::string getValueOrSelectedUnionFieldName(const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    std::string fieldName = PvaConstants::ValueFieldKey;
    epics::pvData::PVFieldPtr pvFieldPtr = pvStructurePtr->getSubField(fieldName);
    if (!pvFieldPtr) {
        epics::pvData::PVUnionPtr pvUnionPtr = pvStructurePtr->getSubField<epics::pvData::PVUnion>(fieldName);
        if (!pvUnionPtr) {
            throw InvalidRequest("Field " + fieldName + " is not a union");
        }
        fieldName = pvUnionPtr->getSelectedFieldName();
    }
    return fieldName;
}

epics::pvData::PVFieldPtr getSubField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::PVFieldPtr pvFieldPtr = pvStructurePtr->getSubField(fieldName);
    if (!pvFieldPtr) {
        throw FieldNotFound("Object does not have subfield " + fieldName);
    }
    return pvFieldPtr;
}

epics::pvData::FieldConstPtr getField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::PVFieldPtr pvFieldPtr = pvStructurePtr->getSubField(fieldName);
    if (!pvFieldPtr) {
        throw FieldNotFound("Object does not have field " + fieldName);
    }
    return pvFieldPtr->getField();
}

epics::pvData::PVStructurePtr getParentStructureForFieldPath(const std::string& fieldPath, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    std::vector<std::string> fieldNames = StringUtility::split(fieldPath);
    return getParentStructureForFieldPath(fieldNames, pvStructurePtr);
}

epics::pvData::PVStructurePtr getParentStructureForFieldPath(const std::vector<std::string>& fieldNames, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    // All path parts except for the last one must be structures
    epics::pvData::PVStructurePtr pvStructurePtr2 = pvStructurePtr;
    int nElements = fieldNames.size();
    for (int i = 0; i < nElements-1; i++) {
        std::string fieldName = fieldNames[i];
        pvStructurePtr2 = getStructureField(fieldName, pvStructurePtr2);
    }
    return pvStructurePtr2;
}

epics::pvData::ScalarConstPtr getScalarField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr);
    epics::pvData::ScalarConstPtr scalarPtr = std::tr1::static_pointer_cast<const epics::pvData::Scalar>(fieldPtr);
    if (!scalarPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a scalar");
    }
    return scalarPtr;
}

epics::pvData::PVScalarArrayPtr getScalarArrayField(const std::string& fieldName, epics::pvData::ScalarType scalarType, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVScalarArrayPtr pvScalarArrayPtr = pvStructurePtr->getSubField<epics::pvData::PVScalarArray>(fieldName);
    if (!pvScalarArrayPtr || pvScalarArrayPtr->getScalarArray()->getElementType() != scalarType) {
        throw InvalidRequest("Field %s is not a scalar array of type %d", fieldName.c_str(), scalarType);
    }
    return pvScalarArrayPtr; 
}

epics::pvData::StructureConstPtr getStructure(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr);
    epics::pvData::StructureConstPtr structurePtr = std::tr1::static_pointer_cast<const epics::pvData::Structure>(fieldPtr);
    if (!structurePtr) {
        throw InvalidRequest("Field " + fieldName + " is not a structure");
    }
    return structurePtr;
}

epics::pvData::PVStructurePtr getStructureField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVStructurePtr pvStructurePtr2 = pvStructurePtr->getSubField<epics::pvData::PVStructure>(fieldName);
    if (!pvStructurePtr2) {
        throw InvalidRequest("Field " + fieldName + " is not a structure");
    }
    return pvStructurePtr2;
}

epics::pvData::PVStructureArrayPtr getStructureArrayField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVStructureArrayPtr pvStructureArrayPtr = pvStructurePtr->getSubField<epics::pvData::PVStructureArray>(fieldName);
    if (!pvStructureArrayPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a structure array");
    }
    return pvStructureArrayPtr;
}

epics::pvData::PVUnionPtr getUnionField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVUnionPtr pvUnionPtr = pvStructurePtr->getSubField<epics::pvData::PVUnion>(fieldName);
    if (!pvUnionPtr) {
        throw InvalidRequest("Field " + fieldName + " is not an union");
    }
    return pvUnionPtr;
}

void setUnionField(const epics::pvData::PVFieldPtr& pvFrom, epics::pvData::PVUnionPtr pvUnionPtr)
{
    if (pvUnionPtr->getUnion()->isVariant()) {
        pvUnionPtr->set(pvFrom);
    }
    else {
        epics::pvData::FieldConstPtr field = pvFrom->getField();
        std::string fieldName = pvFrom->getFieldName();
        int fieldIndex = -1;
        epics::pvData::FieldConstPtrArray fields = pvUnionPtr->getUnion()->getFields();
        epics::pvData::StringArray fieldNames = pvUnionPtr->getUnion()->getFieldNames();
        for (size_t i = 0; i < fields.size(); ++i) {
            epics::pvData::FieldConstPtr unionField = fields[i];
            if (fieldNames[i] == fieldName) {
                if (fields[i] != field) {
                    throw InvalidArgument("PV field type does not match union field type for field name: %s.", fieldName.c_str());
                }
                fieldIndex = i;
                break;
            }
        }
        if (fieldIndex < 0) {
            throw InvalidArgument("PV field does not match any of union fields.");
        }
        pvUnionPtr->set(fieldIndex, pvFrom);
    }
}

epics::pvData::PVUnionArrayPtr getUnionArrayField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVUnionArrayPtr pvUnionArrayPtr = pvStructurePtr->getSubField<epics::pvData::PVUnionArray>(fieldName);
    if (!pvUnionArrayPtr) {
        throw InvalidRequest("Field " + fieldName + " is not an union array");
    }
    return pvUnionArrayPtr;
}

epics::pvData::PVBooleanPtr getBooleanField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVBooleanPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVBoolean>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a boolean");
    }
    return fieldPtr;
}

epics::pvData::PVBytePtr getByteField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVBytePtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVByte>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a byte");
    }
    return fieldPtr;
}

epics::pvData::PVUBytePtr getUByteField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVUBytePtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVUByte>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not an unsigned byte");
    }
    return fieldPtr;
}

epics::pvData::PVShortPtr getShortField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVShortPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVShort>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a short");
    }
    return fieldPtr;
}

epics::pvData::PVUShortPtr getUShortField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVUShortPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVUShort>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not an unsigned short");
    }
    return fieldPtr;
}

epics::pvData::PVIntPtr getIntField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVIntPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVInt>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not an int");
    }
    return fieldPtr;
}

epics::pvData::PVUIntPtr getUIntField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVUIntPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVUInt>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not an unsigned int");
    }
    return fieldPtr;
}

epics::pvData::PVLongPtr getLongField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVLongPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVLong>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a long");
    }
    return fieldPtr;
}

epics::pvData::PVULongPtr getULongField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVULongPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVULong>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not an unsigned long");
    }
    return fieldPtr;
}

epics::pvData::PVFloatPtr getFloatField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVFloatPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVFloat>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a float");
    }
    return fieldPtr;
}

epics::pvData::PVDoublePtr getDoubleField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVDoublePtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVDouble>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a double");
    }
    return fieldPtr;
}

epics::pvData::PVStringPtr getStringField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    epics::pvData::PVStringPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVString>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a string");
    }
    return fieldPtr;
}

//
// Field type retrieval
//
epics::pvData::Type getFieldType(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr);
    return fieldPtr->getType();
}

epics::pvData::ScalarType getScalarType(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::ScalarConstPtr scalarPtr = getScalarField(fieldName, pvStructurePtr);
    return scalarPtr->getScalarType();
}

epics::pvData::ScalarType getScalarArrayType(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr);
    epics::pvData::Type type = fieldPtr->getType();
    if (type != epics::pvData::scalarArray) {
        throw InvalidRequest("Object does not have scalar array field " + fieldName);
    }
    epics::pvData::ScalarArrayConstPtr scalarArrayPtr = std::tr1::static_pointer_cast<const epics::pvData::ScalarArray>(fieldPtr);
    epics::pvData::ScalarType scalarType = scalarArrayPtr->getElementType();
    return scalarType;
}

//
// Conversion PY object => PV Field
//
void pyObjectToField(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr)
{
    // Find field in pv structure
    epics::pvData::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr);
    epics::pvData::Type type = fieldPtr->getType();
    switch (type) {
        case epics::pvData::scalar: {
            pyObjectToScalarField(pyObject, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::structure: {
            pyObjectToStructureField(pyObject, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::scalarArray: {
            pyObjectToScalarArrayField(pyObject, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::structureArray: {
            pyObjectToStructureArrayField(pyObject, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::union_: {
            pyObjectToUnionField(pyObject, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::unionArray: {
            pyObjectToUnionArrayField(pyObject, fieldName, pvStructurePtr);
            break;
        }
        default: {
            throw PvaException("Unrecognized field type: %d", type);
        }
    } 
}    

//
// Conversion PY object => PV Scalar
//
void pyObjectToScalarField(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::ScalarType scalarType = getScalarType(fieldName, pvStructurePtr);
    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            epics::pvData::PVBooleanPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVBoolean>(fieldName);
            bool value = PyUtility::extractValueFromPyObject<bool>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::boolean>(value));
            break;
        }
        case epics::pvData::pvByte: {
            epics::pvData::PVBytePtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVByte>(fieldName);
            char value = PyUtility::extractValueFromPyObject<char>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::int8>(value));
            break;
        }
        case epics::pvData::pvUByte: {
            epics::pvData::PVUBytePtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVUByte>(fieldName);
            unsigned char value = PyUtility::extractValueFromPyObject<unsigned char>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::uint8>(value));
            break;
        }
        case epics::pvData::pvShort: {
            epics::pvData::PVShortPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVShort>(fieldName);
            short value = PyUtility::extractValueFromPyObject<short>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::int16>(value));
            break;
        }
        case epics::pvData::pvUShort: {
            epics::pvData::PVUShortPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVUShort>(fieldName);
            unsigned short value = PyUtility::extractValueFromPyObject<unsigned short>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::uint16>(value));
            break;
        }
        case epics::pvData::pvInt: {
            epics::pvData::PVIntPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVInt>(fieldName);
            int value = PyUtility::extractValueFromPyObject<int>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::int32>(value));
            break;
        }
        case epics::pvData::pvUInt: {
            epics::pvData::PVUIntPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVUInt>(fieldName);
            unsigned int value = PyUtility::extractValueFromPyObject<unsigned int>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::uint32>(value));
            break;
        }
        case epics::pvData::pvLong: {
            epics::pvData::PVLongPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVLong>(fieldName);
            long long value = PyUtility::extractValueFromPyObject<long long>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::int64>(value));
            break;
        }
        case epics::pvData::pvULong: {
            epics::pvData::PVULongPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVULong>(fieldName);
            unsigned long long value = PyUtility::extractValueFromPyObject<unsigned long long>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::uint64>(value));
            break;
        }
        case epics::pvData::pvFloat: {
            epics::pvData::PVFloatPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVFloat>(fieldName);
            float value = PyUtility::extractValueFromPyObject<float>(pyObject);
            fieldPtr->put(value);
            break;
        }
        case epics::pvData::pvDouble: {
            epics::pvData::PVDoublePtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVDouble>(fieldName);
            double value = PyUtility::extractValueFromPyObject<double>(pyObject);
            fieldPtr->put(value);
            break;
        }
        case epics::pvData::pvString: {
            epics::pvData::PVStringPtr fieldPtr = pvStructurePtr->getSubField<epics::pvData::PVString>(fieldName);
            std::string value = PyUtility::extractValueFromPyObject<std::string>(pyObject);
            fieldPtr->put(value);
            break;
        }
        default: {
            throw PvaException("Unrecognized scalar type: %d", scalarType);
        }
    }
}

//
// Conversion PY object => PV Scalar Array
//
void pyObjectToScalarArrayField(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr)
{
    boost::python::list pyList = PyUtility::extractValueFromPyObject<boost::python::list>(pyObject);
    pyListToScalarArrayField(pyList, fieldName, pvStructurePtr);
}

//
// Conversion PY object => PV Structure
//
void pyObjectToStructureField(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr)
{
    boost::python::dict pyDict = PyUtility::extractValueFromPyObject<boost::python::dict>(pyObject);
    pyDictToStructureField(pyDict, fieldName, pvStructurePtr);
}

//
// Conversion PY object => PV Structure Array
//
void pyObjectToStructureArrayField(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr)
{
    boost::python::list pyList = PyUtility::extractValueFromPyObject<boost::python::list>(pyObject);
    pyListToStructureArrayField(pyList, fieldName, pvStructurePtr);
}

//
// Conversion PY object => PV Union
//
void pyObjectToUnionField(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr)
{
    // First dictionary within tuple, then dictionary
    boost::python::object pyObject2 = pyObject;
    boost::python::extract<boost::python::tuple> extractTuple(pyObject);
    if (extractTuple.check()) {
        boost::python::tuple pyTuple = extractTuple();
        pyObject2 = pyTuple[0];
    }    
    boost::python::dict pyDict = PyUtility::extractValueFromPyObject<boost::python::dict>(pyObject2);
    pyDictToUnionField(pyDict, fieldName, pvStructurePtr);
}

//
// Conversion PY object => PV Union Array
//
void pyObjectToUnionArrayField(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr)
{
    boost::python::list pyList = PyUtility::extractValueFromPyObject<boost::python::list>(pyObject);
    pyListToUnionArrayField(pyList, fieldName, pvStructurePtr);
}

//
// Conversion PV Scalar Array => PY List
//
void scalarArrayFieldToPyList(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::list& pyList)
{
    epics::pvData::ScalarType scalarType = getScalarArrayType(fieldName, pvStructurePtr);
    epics::pvData::PVScalarArrayPtr pvScalarArrayPtr = pvStructurePtr->getSubField<epics::pvData::PVScalarArray>(fieldName);

    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            scalarArrayToPyList<epics::pvData::PVBooleanArray, epics::pvData::boolean>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvByte: {
            scalarArrayToPyList<epics::pvData::PVByteArray, epics::pvData::int8>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvUByte: {
            scalarArrayToPyList<epics::pvData::PVUByteArray, epics::pvData::uint8>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvShort: {
            scalarArrayToPyList<epics::pvData::PVShortArray, epics::pvData::int16>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvUShort: {
            scalarArrayToPyList<epics::pvData::PVUShortArray, epics::pvData::uint16>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvInt: {
            scalarArrayToPyList<epics::pvData::PVIntArray, epics::pvData::int32>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvUInt: {
            scalarArrayToPyList<epics::pvData::PVUIntArray, epics::pvData::uint32>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvLong: {
            scalarArrayToPyList<epics::pvData::PVLongArray, epics::pvData::int64>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvULong: {
            scalarArrayToPyList<epics::pvData::PVULongArray, epics::pvData::uint64>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvFloat: {
            scalarArrayToPyList<epics::pvData::PVFloatArray, float>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvDouble: {
            scalarArrayToPyList<epics::pvData::PVDoubleArray, double>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvString: {
            scalarArrayToPyList<epics::pvData::PVStringArray, std::string>(pvScalarArrayPtr, pyList);
            break;
        }
        default: {
            throw PvaException("Unrecognized scalar type: %d", scalarType);
        }
    }
}

boost::python::list getScalarArrayFieldAsPyList(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    boost::python::list pyList;
    scalarArrayFieldToPyList(fieldName, pvStructurePtr, pyList);
    return pyList;
}


//
// Conversion PY List => PV Scalar Array
//
void pyListToScalarArrayField(const boost::python::list& pyList, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    epics::pvData::ScalarType scalarType = getScalarArrayType(fieldName, pvStructurePtr);
    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            pyListToScalarArrayField<epics::pvData::PVBooleanArray, epics::pvData::boolean, bool>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::pvByte: {
            pyListToScalarArrayField<epics::pvData::PVByteArray, epics::pvData::int8, char>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::pvUByte: {
            pyListToScalarArrayField<epics::pvData::PVUByteArray, epics::pvData::uint8, unsigned char>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::pvShort: {
            pyListToScalarArrayField<epics::pvData::PVShortArray, epics::pvData::int16, short>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::pvUShort: {
            pyListToScalarArrayField<epics::pvData::PVUShortArray, epics::pvData::uint16, unsigned short>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::pvInt: {
            pyListToScalarArrayField<epics::pvData::PVIntArray, epics::pvData::int32, int>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::pvUInt: {
            pyListToScalarArrayField<epics::pvData::PVUIntArray, epics::pvData::uint32, unsigned int>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::pvLong: {
            pyListToScalarArrayField<epics::pvData::PVLongArray, epics::pvData::int64, long long>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::pvULong: {
            pyListToScalarArrayField<epics::pvData::PVULongArray, epics::pvData::uint64, unsigned long long>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::pvFloat: {
            pyListToScalarArrayField<epics::pvData::PVFloatArray, float, float>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::pvDouble: {
            pyListToScalarArrayField<epics::pvData::PVDoubleArray, double, double>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case epics::pvData::pvString: {
            pyListToScalarArrayField<epics::pvData::PVStringArray, std::string, std::string>(pyList, fieldName, pvStructurePtr);
            break;
        }
        default: {
            throw PvaException("Unrecognized scalar type: %d", scalarType);
        }
    }
}

//
// Conversion PY {} => PV Structure
//
void pyDictToStructure(const boost::python::dict& pyDict, epics::pvData::PVStructurePtr& pvStructurePtr)
{
    boost::python::list keys = pyDict.keys();
    for (int i = 0; i < boost::python::len(keys); i++) {
        boost::python::object keyObject = keys[i];
        boost::python::extract<std::string> keyExtract(keyObject);
        std::string key;
        if (keyExtract.check()) {
            key = keyExtract();
        }
        else {
            throw InvalidDataType("Dictionary key must be a string");
        }
        boost::python::object pyObject = pyDict[keyObject];
        pyObjectToField(pyObject, key, pvStructurePtr);
    }
}

//
// Conversion PY {} => PV Structure Field
//
void pyDictToStructureField(const boost::python::dict& pyDict, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::PVStructurePtr pvStructurePtr2 = getStructureField(fieldName, pvStructurePtr);
    pyDictToStructure(pyDict, pvStructurePtr2);
}

//
// Conversion PY {} => PV Union
//
void pyDictToUnion(const boost::python::dict& pyDict, epics::pvData::PVUnionPtr& pvUnionPtr)
{
    epics::pvData::PVFieldPtr pvField;
    std::string unionFieldName;
    int dictSize = boost::python::len(pyDict);
    if (dictSize != 1) {
        throw InvalidArgument("Dictionary representing union value must have exactly one element.");
    }
    boost::python::list keys = pyDict.keys();
    boost::python::object keyObject = keys[0];
    boost::python::extract<std::string> keyExtract(keyObject);
    if (keyExtract.check()) {
        unionFieldName = keyExtract();
    }
    else {
        throw InvalidArgument("Dictionary representing union value must have string key.");
    }
    int fieldIndex = -1;
    if (!pvUnionPtr->getUnion()->isVariant()) {
        try {
            pvField = pvUnionPtr->select(unionFieldName);
            fieldIndex = pvUnionPtr->getSelectedIndex();
        }
        catch (std::invalid_argument& ex) {
            throw InvalidArgument("Union does not have field " + unionFieldName);
        }
    }
    else {
        pvField = pvUnionPtr->get();
    }

    epics::pvData::PVStructurePtr unionPvStructurePtr;
    if(pvField) {
        epics::pvData::StructureConstPtr unionStructurePtr = epics::pvData::getFieldCreate()->createFieldBuilder()->add(unionFieldName, pvField->getField())->createStructure();
        unionPvStructurePtr = epics::pvData::getPVDataCreate()->createPVStructure(unionStructurePtr);
    }
    else {
        unionPvStructurePtr = epics::pvData::getPVDataCreate()->createPVStructure(epics::pvData::getFieldCreate()->createStructure());
    }

    pyDictToStructure(pyDict, unionPvStructurePtr);
    if (fieldIndex >= 0) {
        pvUnionPtr->set(fieldIndex, unionPvStructurePtr->getSubField(unionFieldName));
    }
    else {
        pvUnionPtr->set(unionPvStructurePtr->getSubField(unionFieldName));
    }
}

//
// Conversion PY () => PV Union Field
//
void pyTupleToUnionField(const boost::python::tuple& pyTuple, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr)
{
    // Extract dictionary within tuple
    if (boost::python::len(pyTuple) != 1) {
        throw InvalidArgument("PV union tuple provided for field name %s must have exactly one element.", fieldName.c_str());
    }
    boost::python::object pyObject = pyTuple[0];
    boost::python::dict pyDict = PyUtility::extractValueFromPyObject<boost::python::dict>(pyObject);
    pyDictToUnionField(pyDict, fieldName, pvStructurePtr);
}

//
// Conversion PY {} => PV Union Field
//
void pyDictToUnionField(const boost::python::dict& pyDict, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::PVUnionPtr pvUnionPtr = getUnionField(fieldName, pvStructurePtr);
    pyDictToUnion(pyDict, pvUnionPtr);
}

//
// Conversion PY [{}] => PV Structure Array
//
void pyListToStructureArrayField(const boost::python::list& pyList, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::PVStructureArrayPtr pvStructureArrayPtr = getStructureArrayField(fieldName, pvStructurePtr);
    epics::pvData::StructureArrayConstPtr structureArrayPtr = pvStructureArrayPtr->getStructureArray();
    epics::pvData::StructureConstPtr structurePtr = structureArrayPtr->getStructure();
    int listSize = boost::python::len(pyList);
    epics::pvData::PVStructureArray::svector pvStructures(listSize);
    for (int i = 0; i < listSize; i++) {
        boost::python::extract<boost::python::dict> dictExtract(pyList[i]);
        if (dictExtract.check()) {
            boost::python::dict pyDict = dictExtract();
            epics::pvData::PVStructurePtr pvStructure = epics::pvData::getPVDataCreate()->createPVStructure(structurePtr);
            pyDictToStructure(pyDict, pvStructure);
            pvStructures[i] = pvStructure;
        }
        else {
            throw InvalidDataType("Invalid data type for element %d", i);
        }
    }
    pvStructureArrayPtr->setCapacity(listSize);
    pvStructureArrayPtr->replace(freeze(pvStructures));
}

//
// Conversion PY [{}] => PV Union Array
//
void pyListToUnionArrayField(const boost::python::list& pyList, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::PVUnionArrayPtr pvUnionArrayPtr = getUnionArrayField(fieldName, pvStructurePtr);

    epics::pvData::UnionConstPtr unionPtr = pvUnionArrayPtr->getUnionArray()->getUnion();
    int listSize = boost::python::len(pyList);
    epics::pvData::PVUnionArray::svector data(listSize);
    for(size_t i = 0; i < data.size(); ++i) {
        PvObject pvObject = boost::python::extract<PvObject>(pyList[i]);
        epics::pvData::PVStructurePtr pv = pvObject.getPvStructurePtr();
        epics::pvData::PVUnionPtr pvUnion = epics::pvData::getPVDataCreate()->createPVUnion(unionPtr);
        pvUnion->set(pv);
        data[i] = pvUnion;
    }
    pvUnionArrayPtr->setCapacity(listSize);
    pvUnionArrayPtr->replace(freeze(data));
}

//
// Conversion PV Structure Array => PY [{}] 
//
void structureArrayFieldToPyList(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::list& pyList, bool useNumPyArrays)
{
    epics::pvData::PVStructureArrayPtr pvStructureArrayPtr = getStructureArrayField(fieldName, pvStructurePtr);
    int nDataElements = pvStructureArrayPtr->getLength();
    epics::pvData::PVStructureArray::const_svector arrayData(pvStructureArrayPtr->view());
    for (int i = 0; i < nDataElements; ++i) {
        epics::pvData::PVStructurePtr pvStructure = arrayData[i];
        boost::python::dict pyDict;
        structureToPyDict(pvStructure, pyDict, useNumPyArrays);
        pyList.append(pyDict);
    }
}

//
// Conversion PV String Array => PY List
//
void stringArrayToPyList(const epics::pvData::StringArray& stringArray, boost::python::list& pyList)
{
    for(size_t i = 0; i < stringArray.size(); ++i) {
        pyList.append(stringArray[i]);
    }  
}

//
// Conversion PV Structure => PY {}
//
void structureToPyDict(const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict, bool useNumPyArrays)
{
    epics::pvData::StructureConstPtr structurePtr = pvStructurePtr->getStructure();
    epics::pvData::StringArray fieldNames = structurePtr->getFieldNames();
    for (unsigned int i = 0; i < fieldNames.size(); ++i) {
        std::string fieldName = fieldNames[i];
        epics::pvData::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr);
        epics::pvData::Type type = fieldPtr->getType();
        switch (type) {
            case epics::pvData::scalar: {
                epics::pvData::ScalarConstPtr scalarPtr = std::tr1::static_pointer_cast<const epics::pvData::Scalar>(fieldPtr);
                epics::pvData::ScalarType scalarType = scalarPtr->getScalarType();
                addScalarFieldToDict(fieldName, scalarType, pvStructurePtr, pyDict);
                break;
            }
            case epics::pvData::scalarArray: {
                epics::pvData::ScalarArrayConstPtr scalarArrayPtr = std::tr1::static_pointer_cast<const epics::pvData::ScalarArray>(fieldPtr);
                epics::pvData::ScalarType scalarType = scalarArrayPtr->getElementType();
                addScalarArrayFieldToDict(fieldName, scalarType, pvStructurePtr, pyDict, useNumPyArrays);
                break;
            }
            case epics::pvData::structure: {
                addStructureFieldToDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays);
                break;
            }
            case epics::pvData::structureArray: {
                addStructureArrayFieldToDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays);
                break;
            }
            case epics::pvData::union_: {
                addUnionFieldToDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays);
                break;
            }
            case epics::pvData::unionArray: {
                addUnionArrayFieldToDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays);
                break;
            }
            default: {
                throw PvaException("Unrecognized field type: %d", type);
            }
        }
    }
}

void structureFieldToPyDict(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict, bool useNumPyArrays)
{
    structureToPyDict(getStructureField(fieldName, pvStructurePtr), pyDict, useNumPyArrays);
}

//
// Add PV Scalar => PY {}
// 
void addScalarFieldToDict(const std::string& fieldName, epics::pvData::ScalarType scalarType, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict)
{
    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            bool value = getBooleanField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case epics::pvData::pvByte: {
            char value = getByteField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case epics::pvData::pvUByte: {
            unsigned char value = getUByteField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case epics::pvData::pvShort: {
            short value = getShortField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case epics::pvData::pvUShort: {
            ushort value = getUShortField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case epics::pvData::pvInt: {
            int32_t value = getIntField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case epics::pvData::pvUInt: {
            uint32_t value = getUIntField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case epics::pvData::pvLong: {
            int64_t value = getLongField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case epics::pvData::pvULong: {
            uint64_t value = getULongField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case epics::pvData::pvFloat: {
            float value = getFloatField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case epics::pvData::pvDouble: {
            double value = getDoubleField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case epics::pvData::pvString: {
            std::string value = getStringField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        default: {
            throw InvalidDataType("Unrecognized scalar type: %d", scalarType);
        }
    }
}

boost::python::object getScalarFieldAsPyObject(const std::string& fieldName, epics::pvData::ScalarType scalarType, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    boost::python::dict pyDict;
    addScalarFieldToDict(fieldName, scalarType, pvStructurePtr, pyDict);
    return pyDict[fieldName];
}

//
// Add PV Scalar Array => PY {}
// 
void addScalarArrayFieldToDict(const std::string& fieldName, epics::pvData::ScalarType scalarType, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict, bool useNumPyArrays)
{
    pyDict[fieldName] = getScalarArrayFieldAsPyObject(fieldName, scalarType, pvStructurePtr, useNumPyArrays); 
}


boost::python::object getScalarArrayFieldAsPyObject(const std::string& fieldName, epics::pvData::ScalarType scalarType, const epics::pvData::PVStructurePtr& pvStructurePtr, bool useNumPyArrays)
{

// Only use NumPy arrays if support is compiled in and the corresponding
// flag is set 
#if defined HAVE_BOOST_NUM_PY && HAVE_BOOST_NUM_PY == 1
    if (useNumPyArrays && scalarType != epics::pvData::pvString) {
        return getScalarArrayFieldAsNumPyArray(fieldName, pvStructurePtr);
    }
    else {
        return getScalarArrayFieldAsPyList(fieldName, pvStructurePtr);
    }
#else
    return getScalarArrayFieldAsPyList(fieldName, pvStructurePtr);
#endif // if defined HAVE_BOOST_NUM_PY && HAVE_BOOST_NUM_PY == 1
}


//
// Add PV Structure => PY {}
// 
void addStructureFieldToDict(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict, bool useNumPyArrays)
{
    boost::python::dict pyDict2;
    structureFieldToPyDict(fieldName, pvStructurePtr, pyDict2, useNumPyArrays);
    pyDict[fieldName] = pyDict2;
}

boost::python::object getStructureFieldAsPyObject(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, bool useNumPyArrays)
{
    boost::python::dict pyDict;
    structureFieldToPyDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays);
    return pyDict;
}

//
// Add PV Structure Array => PY {}
// 
void addStructureArrayFieldToDict(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict, bool useNumPyArrays) 
{
    boost::python::list pyList;
    epics::pvData::PVStructureArrayPtr structureArrayPtr = getStructureArrayField(fieldName, pvStructurePtr);
    int nDataElements = structureArrayPtr->getLength();
    epics::pvData::PVStructureArray::const_svector arrayData(structureArrayPtr->view());
    for (int i = 0; i < nDataElements; ++i) {
        boost::python::dict pyDict2;
        structureToPyDict(arrayData[i], pyDict2, useNumPyArrays);   
        pyList.append(pyDict2);   
    }
    pyDict[fieldName] = pyList;
}

boost::python::object getStructureArrayFieldAsPyObject(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, bool useNumPyArrays)
{
    boost::python::dict pyDict;
    addStructureArrayFieldToDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays); 
    return pyDict[fieldName];
}

//
// Add PV Union => PY {}
// 
void addUnionFieldToDict(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict, bool useNumPyArrays)
{
    epics::pvData::PVUnionPtr pvUnionPtr = pvStructurePtr->getSubField<epics::pvData::PVUnion>(fieldName);
    std::string unionFieldName = PvaConstants::ValueFieldKey;
    epics::pvData::PVFieldPtr pvField;
    if (!pvUnionPtr->getUnion()->isVariant()) {
        unionFieldName = pvUnionPtr->getSelectedFieldName();
        if (unionFieldName != "") {
            pvField = pvUnionPtr->select(unionFieldName);
        }
    }
    else {
        pvField = pvUnionPtr->get();
    }
    epics::pvData::PVStructurePtr unionPvStructurePtr;
    if(pvField) {
        epics::pvData::StructureConstPtr unionStructurePtr = epics::pvData::getFieldCreate()->createFieldBuilder()->add(unionFieldName, pvField->getField())->createStructure();
        unionPvStructurePtr = epics::pvData::getPVDataCreate()->createPVStructure(unionStructurePtr);
        unionPvStructurePtr->getSubField(unionFieldName)->copy(*pvField);
    }
    else {
        unionPvStructurePtr = epics::pvData::getPVDataCreate()->createPVStructure(
            epics::pvData::getFieldCreate()->createStructure());
    }

    boost::python::dict pyDict2;
    structureToPyDict(unionPvStructurePtr, pyDict2, useNumPyArrays);
    boost::python::tuple pyTuple = boost::python::make_tuple(pyDict2);
    pyDict[fieldName] = pyTuple;
}

boost::python::object getUnionFieldAsPyObject(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, bool useNumPyArrays)
{
    boost::python::dict pyDict;
    addUnionFieldToDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays);
    return pyDict[fieldName];
}

//
// Add PV Union Array => PY {}
// 
void addUnionArrayFieldToDict(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict, bool useNumPyArrays)
{
    epics::pvData::PVUnionArrayPtr pvUnionArrayPtr = pvStructurePtr->getSubField<epics::pvData::PVUnionArray>(fieldName);
    epics::pvData::shared_vector<const epics::pvData::PVUnionPtr> pvUnions = pvUnionArrayPtr->view();
    boost::python::list pyList;
    for(size_t i = 0 ; i < pvUnions.size(); ++i) {
        epics::pvData::PVUnionPtr pvUnionPtr = pvUnions[i];
        std::string fieldName = PvaConstants::ValueFieldKey;
        epics::pvData::PVFieldPtr pvField;
        if (!pvUnionPtr->getUnion()->isVariant()) {
            fieldName = pvUnionPtr->getSelectedFieldName();
            pvField = pvUnionPtr->select(fieldName);
        }
        else {
            pvField = pvUnionPtr->get();
        }

        epics::pvData::PVStructurePtr unionPvStructurePtr;
        epics::pvData::StructureConstPtr unionStructurePtr = epics::pvData::getFieldCreate()->createFieldBuilder()->add(fieldName, pvField->getField())->createStructure();
        unionPvStructurePtr = epics::pvData::getPVDataCreate()->createPVStructure(unionStructurePtr);
        unionPvStructurePtr->getSubField(fieldName)->copy(*pvField);

        boost::python::dict pyDict2;
        structureToPyDict(unionPvStructurePtr, pyDict2, useNumPyArrays);
        boost::python::tuple pyTuple = boost::python::make_tuple(pyDict2);
        pyList.append(pyTuple);
    }
    pyDict[fieldName] = pyList;
}

boost::python::object getUnionArrayFieldAsPyObject(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, bool useNumPyArrays)
{
    boost::python::dict pyDict;
    addUnionArrayFieldToDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays);
    return pyDict[fieldName];
}

//
// Conversion Structure => PY {}
//
void structureToPyDict(const epics::pvData::StructureConstPtr& structurePtr, boost::python::dict& pyDict)
{
    epics::pvData::StringArray fieldNames = structurePtr->getFieldNames();
    for (unsigned int i = 0; i < fieldNames.size(); ++i) {
        std::string fieldName = fieldNames[i];
        epics::pvData::FieldConstPtr fieldPtr = structurePtr->getField(fieldName);
        fieldToPyDict(fieldPtr, fieldName, pyDict);
    }
}

//
// Conversion Union => PY ()
//
boost::python::tuple unionToPyTuple(const epics::pvData::UnionConstPtr& unionPtr)
{
    // Variant union is represented as empty tuple
    if (unionPtr->isVariant()) {
        return boost::python::make_tuple();
    }

    // Restricted union is represented as dict inside tuple
    epics::pvData::StringArray fieldNames = unionPtr->getFieldNames();
    boost::python::dict pyDict;
    for (unsigned int i = 0; i < fieldNames.size(); ++i) {
        std::string fieldName = fieldNames[i];
        epics::pvData::FieldConstPtr fieldPtr = unionPtr->getField(fieldName);
        fieldToPyDict(fieldPtr, fieldName, pyDict);
    }
    boost::python::tuple pyTuple = boost::python::make_tuple(pyDict);
    return pyTuple;
}

//
// Conversion Field => PY {}
//
void fieldToPyDict(const epics::pvData::FieldConstPtr& fieldPtr, const std::string& fieldName, boost::python::dict& pyDict)
{
    epics::pvData::Type type = fieldPtr->getType();
    switch (type) {
        case epics::pvData::scalar: {
            epics::pvData::ScalarConstPtr scalarPtr = std::tr1::static_pointer_cast<const epics::pvData::Scalar>(fieldPtr);
            epics::pvData::ScalarType scalarType = scalarPtr->getScalarType();
            pyDict[fieldName] = static_cast<PvType::ScalarType>(scalarType);
            break;
        }
        case epics::pvData::scalarArray: {
            epics::pvData::ScalarArrayConstPtr scalarArrayPtr = std::tr1::static_pointer_cast<const epics::pvData::ScalarArray>(fieldPtr);
            epics::pvData::ScalarType scalarType = scalarArrayPtr->getElementType();
            boost::python::list pyList;
            pyList.append(static_cast<PvType::ScalarType>(scalarType));
            pyDict[fieldName] = pyList;
            break;
        }
        case epics::pvData::structure: {
            epics::pvData::StructureConstPtr structurePtr2 = std::tr1::static_pointer_cast<const epics::pvData::Structure>(fieldPtr);
            boost::python::dict pyDict2;
            structureToPyDict(structurePtr2, pyDict2);
            pyDict[fieldName] = pyDict2;
            break;
        }
        case epics::pvData::structureArray: {
            epics::pvData::StructureArrayConstPtr structureArrayPtr = std::tr1::static_pointer_cast<const epics::pvData::StructureArray>(fieldPtr);
            epics::pvData::StructureConstPtr structurePtr2 = structureArrayPtr->getStructure(); 
            boost::python::dict pyDict2;
            structureToPyDict(structurePtr2, pyDict2);
            boost::python::list pyList;
            pyList.append(pyDict2);
            pyDict[fieldName] = pyList;
            break;
        }
        case epics::pvData::union_: {
            epics::pvData::UnionConstPtr unionPtr = std::tr1::static_pointer_cast<const epics::pvData::Union>(fieldPtr);
            pyDict[fieldName] = unionToPyTuple(unionPtr);
            break;
        }
        case epics::pvData::unionArray: {
            epics::pvData::UnionArrayConstPtr unionArrayPtr = std::tr1::static_pointer_cast<const epics::pvData::UnionArray>(fieldPtr);
            epics::pvData::UnionConstPtr unionPtr = unionArrayPtr->getUnion();
            boost::python::list pyList;
            pyList.append(unionToPyTuple(unionPtr));
            pyDict[fieldName] = pyList;
            break;
        }
        default: {
            throw PvaException("Unrecognized field type: %d", type);
        }
    }
}

//
// Copy PV Structure => PV Structure
//
void copyStructureToStructure(const epics::pvData::PVStructurePtr& srcPvStructurePtr, epics::pvData::PVStructurePtr& destPvStructurePtr)
{
    epics::pvData::StructureConstPtr srcStructurePtr = srcPvStructurePtr->getStructure();
    epics::pvData::StringArray fieldNames = srcStructurePtr->getFieldNames();
    for (unsigned int i = 0; i < fieldNames.size(); ++i) {
        std::string fieldName = fieldNames[i];
        epics::pvData::PVFieldPtr pvFieldPtr = srcPvStructurePtr->getSubField(fieldName);
        epics::pvData::FieldConstPtr fieldPtr = pvFieldPtr->getField();
        epics::pvData::Type type = fieldPtr->getType();
        switch (type) {
            case epics::pvData::scalar: {
                epics::pvData::ScalarConstPtr scalarPtr = std::tr1::static_pointer_cast<const epics::pvData::Scalar>(fieldPtr);
                epics::pvData::ScalarType scalarType = scalarPtr->getScalarType();
                copyScalarToStructure(fieldName, scalarType, srcPvStructurePtr, destPvStructurePtr);
                break;
            }
            case epics::pvData::scalarArray: {
                epics::pvData::ScalarArrayConstPtr scalarArrayPtr = std::tr1::static_pointer_cast<const epics::pvData::ScalarArray>(fieldPtr);
                epics::pvData::ScalarType scalarType = scalarArrayPtr->getElementType();
                copyScalarArrayToStructure(fieldName, scalarType, srcPvStructurePtr, destPvStructurePtr);
                break;
            }
            case epics::pvData::structure: {
                copyStructureToStructure(fieldName, srcPvStructurePtr, destPvStructurePtr);
                break;
            }
            case epics::pvData::structureArray: {
                copyStructureArrayToStructure(fieldName, srcPvStructurePtr, destPvStructurePtr);
                break;
            }
            case epics::pvData::union_: {
                epics::pvData::PVUnionPtr pvFrom = srcPvStructurePtr->getSubField<epics::pvData::PVUnion>(fieldName);
                epics::pvData::PVUnionPtr pvTo = destPvStructurePtr->getSubField<epics::pvData::PVUnion>(fieldName);
                pvTo->copy(*pvFrom);
                break;
            }
            case epics::pvData::unionArray: {
                epics::pvData::PVUnionArrayPtr pvFrom = srcPvStructurePtr->getSubField<epics::pvData::PVUnionArray>(fieldName);
                epics::pvData::PVUnionArrayPtr pvTo = destPvStructurePtr->getSubField<epics::pvData::PVUnionArray>(fieldName);
                pvTo->replace(pvFrom->view());
                break;
            }
            default: {
                throw PvaException("Unrecognized field type: %d", type);
            }
        }
    }
}

void copyStructureToStructure(const std::string& fieldName, const epics::pvData::PVStructurePtr& srcPvStructurePtr, epics::pvData::PVStructurePtr& destPvStructurePtr)
{
    epics::pvData::PVStructurePtr destPvStructurePtr2 = destPvStructurePtr->getSubField<epics::pvData::PVStructure>(fieldName);
    if (destPvStructurePtr2) {
        epics::pvData::PVStructurePtr srcPvStructurePtr2 = srcPvStructurePtr->getSubField<epics::pvData::PVStructure>(fieldName);
        if (srcPvStructurePtr2) {
            copyStructureToStructure(srcPvStructurePtr2, destPvStructurePtr2);
        }
        else {
            throw FieldNotFound("Source structure has no structure field " + fieldName);
        }
    }
    else {
        throw FieldNotFound("Destination structure has no structure field " + fieldName);
    }
}

//
// Copy PV Structure Array => PV Structure
//
void copyStructureArrayToStructure(const std::string& fieldName, const epics::pvData::PVStructurePtr& srcPvStructurePtr, epics::pvData::PVStructurePtr& destPvStructurePtr)
{
    epics::pvData::PVStructureArrayPtr destPvStructureArrayPtr = getStructureArrayField(fieldName, destPvStructurePtr);
    if (!destPvStructureArrayPtr) {
        throw FieldNotFound("Destination structure has no structure array field " + fieldName);
    }
    epics::pvData::StructureArrayConstPtr destStructureArrayPtr = destPvStructureArrayPtr->getStructureArray();
    epics::pvData::StructureConstPtr structurePtr = destStructureArrayPtr->getStructure();

    epics::pvData::PVStructureArrayPtr srcPvStructureArrayPtr = getStructureArrayField(fieldName, srcPvStructurePtr);
    if (!srcPvStructureArrayPtr) {
        throw FieldNotFound("Source structure has no structure array field " + fieldName);
    }


    int nElements = srcPvStructureArrayPtr->getLength();
    epics::pvData::PVStructureArray::const_svector srcPvStructures(srcPvStructureArrayPtr->view());
    epics::pvData::PVStructureArray::svector destPvStructures(nElements);

    for (int i = 0; i < nElements; i++) {
        epics::pvData::PVStructurePtr destPvStructurePtr2 = epics::pvData::getPVDataCreate()->createPVStructure(structurePtr);
        epics::pvData::PVStructurePtr srcPvStructurePtr2 = srcPvStructures[i];
        copyStructureToStructure(srcPvStructurePtr2, destPvStructurePtr2);
        destPvStructures[i] = destPvStructurePtr2;
    }
    destPvStructureArrayPtr->setCapacity(nElements);
    destPvStructureArrayPtr->replace(freeze(destPvStructures));
}

//
// Copy PV Scalar => PV Structure
//
void copyScalarToStructure(const std::string& fieldName, epics::pvData::ScalarType scalarType, const epics::pvData::PVStructurePtr& srcPvStructurePtr, epics::pvData::PVStructurePtr& destPvStructurePtr)
{
    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            epics::pvData::PVBooleanPtr fieldPtr = destPvStructurePtr->getSubField<epics::pvData::PVBoolean>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<epics::pvData::PVBoolean>(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvByte: {
            epics::pvData::PVBytePtr fieldPtr = destPvStructurePtr->getSubField<epics::pvData::PVByte>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<epics::pvData::PVByte>(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvUByte: {
            epics::pvData::PVUBytePtr fieldPtr = destPvStructurePtr->getSubField<epics::pvData::PVUByte>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<epics::pvData::PVUByte>(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvShort: {
            epics::pvData::PVShortPtr fieldPtr = destPvStructurePtr->getSubField<epics::pvData::PVShort>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<epics::pvData::PVShort>(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvUShort: {
            epics::pvData::PVUShortPtr fieldPtr = destPvStructurePtr->getSubField<epics::pvData::PVUShort>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<epics::pvData::PVUShort>(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvInt: {
            epics::pvData::PVIntPtr fieldPtr = destPvStructurePtr->getSubField<epics::pvData::PVInt>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<epics::pvData::PVInt>(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvUInt: {
            epics::pvData::PVUIntPtr fieldPtr = destPvStructurePtr->getSubField<epics::pvData::PVUInt>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<epics::pvData::PVUInt>(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvLong: {
            epics::pvData::PVLongPtr fieldPtr = destPvStructurePtr->getSubField<epics::pvData::PVLong>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<epics::pvData::PVLong>(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvULong: {
            epics::pvData::PVULongPtr fieldPtr = destPvStructurePtr->getSubField<epics::pvData::PVULong>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<epics::pvData::PVULong>(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvFloat: {
            epics::pvData::PVFloatPtr fieldPtr = destPvStructurePtr->getSubField<epics::pvData::PVFloat>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<epics::pvData::PVFloat>(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvDouble: {
            epics::pvData::PVDoublePtr fieldPtr = destPvStructurePtr->getSubField<epics::pvData::PVDouble>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<epics::pvData::PVDouble>(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvString: {
            epics::pvData::PVStringPtr fieldPtr = destPvStructurePtr->getSubField<epics::pvData::PVString>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<epics::pvData::PVString>(fieldName)->get());
            }
            break;
        }
        default: {
            throw InvalidDataType("Unrecognized scalar type: %d", scalarType);
        }
    }
}
    
//
// Copy PV Scalar Array => PV Structure
//
void copyScalarArrayToStructure(const std::string& fieldName, epics::pvData::ScalarType scalarType, const epics::pvData::PVStructurePtr& srcPvStructurePtr, epics::pvData::PVStructurePtr& destPvStructurePtr)
{
    epics::pvData::PVScalarArrayPtr srcPvScalarArrayPtr = getScalarArrayField(fieldName, scalarType, srcPvStructurePtr);
    epics::pvData::PVScalarArrayPtr destPvScalarArrayPtr = getScalarArrayField(fieldName, scalarType, destPvStructurePtr);

    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            copyScalarArrayToScalarArray<epics::pvData::PVBooleanArray, epics::pvData::boolean>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvByte: {
            copyScalarArrayToScalarArray<epics::pvData::PVByteArray, epics::pvData::int8>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvUByte: {
            copyScalarArrayToScalarArray<epics::pvData::PVUByteArray, epics::pvData::uint8>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvShort: {
            copyScalarArrayToScalarArray<epics::pvData::PVShortArray, epics::pvData::int16>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvUShort: {
            copyScalarArrayToScalarArray<epics::pvData::PVUShortArray, epics::pvData::uint16>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvInt: {
            copyScalarArrayToScalarArray<epics::pvData::PVIntArray, epics::pvData::int32>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvUInt: {
            copyScalarArrayToScalarArray<epics::pvData::PVUIntArray, epics::pvData::uint32>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvLong: {
            copyScalarArrayToScalarArray<epics::pvData::PVLongArray, epics::pvData::int64>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvULong: {
            copyScalarArrayToScalarArray<epics::pvData::PVULongArray, epics::pvData::uint64>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvFloat: {
            copyScalarArrayToScalarArray<epics::pvData::PVFloatArray, float>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvDouble: {
            copyScalarArrayToScalarArray<epics::pvData::PVDoubleArray, double>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvString: {
            copyScalarArrayToScalarArray<epics::pvData::PVStringArray, std::string>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        default: {
            throw InvalidDataType("Unrecognized scalar type: %d", scalarType);
        }
    }
}

//
// Methods for creating structure
//
epics::pvData::StructureConstPtr createStructureFromDict(const boost::python::dict& pyDict, const std::string& structureId)
{
    epics::pvData::FieldConstPtrArray fields;
    epics::pvData::StringArray names;
    updateFieldArrayFromDict(pyDict, fields, names);

    std::string structureName = StringUtility::trim(structureId);
    if (structureName.length()) {
        return epics::pvData::getFieldCreate()->createStructure(structureName, names, fields);
    }
    return epics::pvData::getFieldCreate()->createStructure(names, fields);
}

epics::pvData::UnionConstPtr createUnionFromDict(const boost::python::dict& pyDict, const std::string& structureId)
{
    epics::pvData::FieldConstPtrArray fields;
    epics::pvData::StringArray names;
    updateFieldArrayFromDict(pyDict, fields, names);

    std::string structureName = StringUtility::trim(structureId);
    if (structureName.length()) {
        return epics::pvData::getFieldCreate()->createUnion(structureName, names, fields);
    }
    return epics::pvData::getFieldCreate()->createUnion(names, fields);
}

void updateFieldArrayFromDict(const boost::python::dict& pyDict, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    boost::python::list fieldNames = pyDict.keys();
    for (int i = 0; i < boost::python::len(fieldNames); i++) {
        boost::python::object fieldNameObject = fieldNames[i];
        boost::python::extract<std::string> fieldNameExtract(fieldNameObject);
        std::string fieldName;
        if (fieldNameExtract.check()) {
            fieldName = fieldNameExtract();
        }
        else {
            throw InvalidArgument("Dictionary key is used as field name and must be a string");
        }

        // Check for Scalar
        boost::python::object valueObject = pyDict[fieldNameObject];
        if (updateFieldArrayFromInt(valueObject, fieldName, fields, names)) {
            continue;
        }

        // Check for list: []
        // Type of the first element in the list will determine PV list type
        boost::python::extract<boost::python::list> listExtract(valueObject);
        if (listExtract.check()) {
            boost::python::list pyList = listExtract();
            int listSize = boost::python::len(pyList);
            if (listSize != 1) {
                throw InvalidArgument("PV type list provided for field name %s must have exactly one element.", fieldName.c_str());
            }

            // [Scalar] => ScalarArray
            if (updateFieldArrayFromIntList(pyList[0], fieldName, fields, names)) {
                continue;
            }   
            
            // [()] => UnionArray
            if (updateFieldArrayFromTupleList(pyList[0], fieldName, fields, names)) {
                continue;
            }   

            // [{}] => StructureArray
            if (updateFieldArrayFromDictList(pyList[0], fieldName, fields, names)) {
                continue;
            }   

            // [PvObject] => StructureArray
            if (updateFieldArrayFromPvObjectList(pyList[0], fieldName, fields, names)) {
                continue;
            }

            // Invalid request.
            throw InvalidArgument("Unrecognized list type for field name %s", fieldName.c_str());
        } 

        // Check for tuple: () => Union
        if (updateFieldArrayFromTuple(valueObject, fieldName, fields, names)) {
            continue;
        }

        // Check for dict: {} => Structure
        if (updateFieldArrayFromDict(valueObject, fieldName, fields, names)) {
            continue;
        }

        // Check for PvObject: PvObject => Structure
        if (updateFieldArrayFromPvObject(valueObject, fieldName, fields, names)) {
            continue;
        }

        // Invalid request.
        throw InvalidArgument("Unrecognized structure type for field name %s", fieldName.c_str());
    }
}

epics::pvData::PVStructurePtr createUnionPvStructure(const epics::pvData::PVUnionPtr& pvUnionPtr, const std::string& fieldName)
{
    epics::pvData::PVFieldPtr pvField = pvUnionPtr->get();
    epics::pvData::StringArray names(1);
    epics::pvData::PVFieldPtrArray pvfields(1);
    names[0] = PvaConstants::ValueFieldKey;
    if (!pvUnionPtr->getUnion()->isVariant()) {
        std::string unionFieldName = pvUnionPtr->getSelectedFieldName();
        if (unionFieldName != "") {
            names[0] = unionFieldName;
        }
    }
    pvfields[0] = pvField;
    epics::pvData::PVStructurePtr pv = epics::pvData::getPVDataCreate()->createPVStructure(names, pvfields);
    return pv;
}

epics::pvData::PVStructurePtr createUnionFieldPvStructure(epics::pvData::UnionConstPtr unionPtr, const std::string& fieldName)
{
    epics::pvData::StringArray names(1);
    epics::pvData::PVFieldPtrArray pvfields(1);
    names[0] = PvaConstants::ValueFieldKey;
    epics::pvData::FieldConstPtr field;
    if (unionPtr->isVariant()) {
        field = epics::pvData::getFieldCreate()->createVariantUnion();
    } 
    else {
        field = unionPtr->getField(fieldName);
    }
    pvfields[0] = epics::pvData::getPVDataCreate()->createPVField(field);
    return epics::pvData::getPVDataCreate()->createPVStructure(names,pvfields);
}

void addScalarField(const std::string& fieldName, epics::pvData::ScalarType scalarType, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    fields.push_back(epics::pvData::getFieldCreate()->createScalar(scalarType));
    names.push_back(fieldName);
}

void addScalarArrayField(const std::string& fieldName, epics::pvData::ScalarType scalarType, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    fields.push_back(epics::pvData::getFieldCreate()->createScalarArray(scalarType));
    names.push_back(fieldName);
}

void addStructureField(const std::string& fieldName, const boost::python::dict& pyDict, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    fields.push_back(createStructureFromDict(pyDict));
    names.push_back(fieldName);
}

void addStructureField(const std::string& fieldName, const PvObject & pvObject, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    fields.push_back(pvObject.getPvStructurePtr()->getStructure());
    names.push_back(fieldName);   
}

void addStructureArrayField(const std::string& fieldName, const boost::python::dict& pyDict, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    fields.push_back(epics::pvData::getFieldCreate()->createStructureArray(createStructureFromDict(pyDict)));
    names.push_back(fieldName);
}

void addStructureArrayField(const std::string& fieldName, const PvObject & pvObject, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    fields.push_back(epics::pvData::getFieldCreate()->createStructureArray(pvObject.getPvStructurePtr()->getStructure()));
    names.push_back(fieldName);   
}

void addUnionField(const std::string& fieldName, const boost::python::dict& pyDict, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    fields.push_back(createUnionFromDict(pyDict));
    names.push_back(fieldName);
}

void addUnionArrayField(const std::string& fieldName, const boost::python::dict& pyDict, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    fields.push_back(epics::pvData::getFieldCreate()->createUnionArray(createUnionFromDict(pyDict)));
    names.push_back(fieldName);
}

void addVariantUnionField(const std::string& fieldName, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    fields.push_back(epics::pvData::getFieldCreate()->createVariantUnion());
    names.push_back(fieldName);
}

void addVariantUnionArrayField(const std::string& fieldName, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    fields.push_back(epics::pvData::getFieldCreate()->createUnionArray(epics::pvData::getFieldCreate()->createVariantUnion()));
    names.push_back(fieldName);
}

bool updateFieldArrayFromInt(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    boost::python::extract<int> scalarExtract(pyObject);
    if (!scalarExtract.check()) {
        return false;
    }

    int scalarExtractValue = scalarExtract();
    epics::pvData::ScalarType scalarType = static_cast<epics::pvData::ScalarType>(scalarExtractValue);
    addScalarField(fieldName, scalarType, fields, names);
    return true;
}

bool updateFieldArrayFromIntList(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    boost::python::extract<int> arrayScalarExtract(pyObject);
    if (!arrayScalarExtract.check()) {
        return false;
    }

    int arrayScalarExtractValue = arrayScalarExtract();
    epics::pvData::ScalarType scalarType = static_cast<epics::pvData::ScalarType>(arrayScalarExtractValue);
    addScalarArrayField(fieldName, scalarType, fields, names);
    return true;
}

bool updateFieldArrayFromDict(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    boost::python::extract<boost::python::dict> dictExtract(pyObject);
    if (!dictExtract.check()) {
        return false;
    }

    boost::python::dict pyDict2 = dictExtract();
    int dictSize = boost::python::len(pyDict2);
    if (!dictSize) {
        throw InvalidArgument("PV type dict provided for field name %s must be non-empty.", fieldName.c_str());
    }
    addStructureField(fieldName, pyDict2, fields, names);
    return true;
}

bool updateFieldArrayFromDictList(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    boost::python::extract<boost::python::dict> dictExtract(pyObject);
    if (!dictExtract.check()) {
        return false;
    }

    boost::python::dict pyDict2 = dictExtract();
    int dictSize = boost::python::len(pyDict2);
    if (!dictSize) {
        throw InvalidArgument("PV type dict provided for field name %s must be non-empty.", fieldName.c_str());
    }
    addStructureArrayField(fieldName, pyDict2, fields, names);
    return true;
}

bool updateFieldArrayFromTuple(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    boost::python::extract<boost::python::tuple> tupleExtract(pyObject);
    if (!tupleExtract.check()) {
        return false;
    }

    boost::python::tuple pyTuple = tupleExtract();
    int tupleSize = boost::python::len(pyTuple);
    switch (tupleSize) {
        case (0): {
            addVariantUnionField(fieldName, fields, names);
            break;
        }
        case (1): {
            boost::python::extract<boost::python::dict> dictExtract(pyTuple[0]);
            if (dictExtract.check()) {
                boost::python::dict pyDict2 = dictExtract();
                int dictSize = boost::python::len(pyDict2);
                if (!dictSize) {
                    addVariantUnionField(fieldName, fields, names);
                }
                else {
                    addUnionField(fieldName, pyDict2, fields, names);
                }
            }
            break;
        }
        default: {
            throw InvalidArgument("PV union tuple provided for field name %s must have zero or one element.", fieldName.c_str());
        }
    }
    return true;
}

bool updateFieldArrayFromTupleList(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    boost::python::extract<boost::python::tuple> tupleExtract(pyObject);
    if (!tupleExtract.check()) {
        return false;
    }

    boost::python::tuple pyTuple = tupleExtract();
    int tupleSize = boost::python::len(pyTuple);
    switch (tupleSize) {
        case (0): {
            addVariantUnionArrayField(fieldName, fields, names);
            break;
        }
        case (1): {
            boost::python::extract<boost::python::dict> dictExtract(pyTuple[0]);
            if (dictExtract.check()) {
                boost::python::dict pyDict2 = dictExtract();
                int dictSize = boost::python::len(pyDict2);
                if (!dictSize) {
                    addVariantUnionArrayField(fieldName, fields, names);
                }
                else {
                    addUnionArrayField(fieldName, pyDict2, fields, names);
                }
            }
            break;
        }
        default: {
            throw InvalidArgument("PV union tuple provided for field name %s must have zero or one element.", fieldName.c_str());
        }
    }
    return true;
}

bool updateFieldArrayFromPvObject(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    boost::python::extract<PvObject> pvObjectExtract(pyObject);
    if (!pvObjectExtract.check()) {
        return false;
    }

    PvObject pvObject = pvObjectExtract();
    boost::python::dict pyDict2 = pvObject.getStructureDict();
    int dictSize = boost::python::len(pyDict2);
    if (!dictSize) {
        throw InvalidArgument("PV object dict provided for field name %s must be non-empty.", fieldName.c_str());
    }
    PvType::DataType dataType = pvObject.getDataType();
    switch (dataType) {
        case PvType::Union: {
            addUnionField(fieldName, extractUnionStructureDict(pyDict2), fields, names);
            break;
        }
        case PvType::Variant: {
            addVariantUnionField(fieldName, fields, names);
            break;
        }
        default: {
            addStructureField(fieldName, pvObject, fields, names);
        }
    }
    return true;
}

bool updateFieldArrayFromPvObjectList(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
{
    boost::python::extract<PvObject> pvObjectExtract(pyObject);
    if (!pvObjectExtract.check()) {
        return false;
    }

    PvObject pvObject = pvObjectExtract();
    boost::python::dict pyDict2 = pvObject.getStructureDict();
    int dictSize = boost::python::len(pyDict2);
    if (!dictSize) {
        throw InvalidArgument("PV object dict provided for field name %s must be non-empty.", fieldName.c_str());
    }
    PvType::DataType dataType = pvObject.getDataType();
    switch (dataType) {
        case PvType::Union: {
            addUnionArrayField(fieldName, extractUnionStructureDict(pyDict2), fields, names);
            break;
        }
        case PvType::Variant: {
            addVariantUnionArrayField(fieldName, fields, names);
            break;
        }
        default: {
            addStructureArrayField(fieldName, pvObject, fields, names);
        }
    }
    return true;
}

//
// Extract union structure dict 
//
boost::python::dict extractUnionStructureDict(const boost::python::dict& pyDict)
{
    // Do not duplicate value key, if one is provided, if it is a single
    // dictionary key, and if it holds dictionary or tuple holding dictionary
    boost::python::dict pyDict2;
    if (pyDict.has_key(PvaConstants::ValueFieldKey) && boost::python::len(pyDict) == 1) {
        boost::python::object pyObject = pyDict[PvaConstants::ValueFieldKey];
        boost::python::extract<boost::python::tuple> tupleExtract(pyObject);

        // Look for dict inside tuple
        if (tupleExtract.check()) {
            boost::python::tuple pyTuple = tupleExtract();
            if (boost::python::len(pyTuple) == 1) {
                boost::python::extract<boost::python::dict> dictExtract(pyTuple[0]);
                if (dictExtract.check()) {
                    return dictExtract();
                }
            }
        }
        
        // Look for dict
        boost::python::extract<boost::python::dict> dictExtract(pyObject);
        if (dictExtract.check()) {
            return dictExtract();
        }
    }

    // We could not find union in the structure, simply use provided dict
    return pyDict;
}

//
// Return structure field as python object. Allow notation like 'x.y.z'
// for the field path.
//
boost::python::object getFieldPathAsPyObject(const std::string& fieldPath, const epics::pvData::PVStructurePtr& pvStructurePtr, bool useNumPyArrays)
{
    std::vector<std::string> fieldNames = StringUtility::split(fieldPath);
    epics::pvData::PVStructurePtr pvStructurePtr2 = getParentStructureForFieldPath(fieldNames, pvStructurePtr);

    // Last field in the path is what we want.
    int nElements = fieldNames.size();
    std::string fieldName = fieldNames[nElements-1];
    epics::pvData::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr2);
    epics::pvData::Type type = fieldPtr->getType();
    switch (type) {
        case epics::pvData::scalar: {
            epics::pvData::ScalarConstPtr scalarPtr = std::tr1::static_pointer_cast<const epics::pvData::Scalar>(fieldPtr);
            epics::pvData::ScalarType scalarType = scalarPtr->getScalarType();
            return getScalarFieldAsPyObject(fieldName, scalarType, pvStructurePtr2);
        }
        case epics::pvData::scalarArray: {
            epics::pvData::ScalarArrayConstPtr scalarArrayPtr = std::tr1::static_pointer_cast<const epics::pvData::ScalarArray>(fieldPtr);
            epics::pvData::ScalarType scalarType = scalarArrayPtr->getElementType();
            return getScalarArrayFieldAsPyObject(fieldName, scalarType, pvStructurePtr2, useNumPyArrays);
        }
        case epics::pvData::structure: {
            return getStructureFieldAsPyObject(fieldName, pvStructurePtr2, useNumPyArrays);
        }
        case epics::pvData::structureArray: {
            return getStructureArrayFieldAsPyObject(fieldName, pvStructurePtr2, useNumPyArrays);
        }
        case epics::pvData::union_: {
            return getUnionFieldAsPyObject(fieldName, pvStructurePtr2, useNumPyArrays);
        }
        case epics::pvData::unionArray: {
            return getUnionArrayFieldAsPyObject(fieldName, pvStructurePtr2, useNumPyArrays);
        }
        default: {
            throw PvaException("Unrecognized field type: %d", type);
        }
    }
}

//
// Set structure field from python object. Allow notation like 'x.y.z'
// for the field path.
//
void setPyObjectToFieldPath(const boost::python::object& pyObject, const std::string& fieldPath, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    std::vector<std::string> fieldNames = StringUtility::split(fieldPath);
    epics::pvData::PVStructurePtr pvStructurePtr2 = getParentStructureForFieldPath(fieldNames, pvStructurePtr);
    int nElements = fieldNames.size();
    
    // Last field in the path is what we want.
    std::string fieldName = fieldNames[nElements-1];
    pyObjectToField(pyObject, fieldName, pvStructurePtr2);
}

#if defined HAVE_BOOST_NUM_PY && HAVE_BOOST_NUM_PY == 1

//
// Conversion PV Scalar Array => NumPy Array
//
boost::numpy::ndarray getScalarArrayFieldAsNumPyArray(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::ScalarType scalarType = getScalarArrayType(fieldName, pvStructurePtr);
    epics::pvData::PVScalarArrayPtr pvScalarArrayPtr = pvStructurePtr->getSubField<epics::pvData::PVScalarArray>(fieldName);
    int nDataElements = pvScalarArrayPtr->getLength();
    boost::python::tuple shape = boost::python::make_tuple(nDataElements);
    boost::python::object arrayOwner;

    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            return getScalarArrayAsNumPyArray<epics::pvData::PVBooleanArray, epics::pvData::boolean>(pvScalarArrayPtr);
        }
        case epics::pvData::pvByte: {
            return getScalarArrayAsNumPyArray<epics::pvData::PVByteArray, int8_t>(pvScalarArrayPtr);
        }
        case epics::pvData::pvUByte: {
            return getScalarArrayAsNumPyArray<epics::pvData::PVUByteArray, uint8_t>(pvScalarArrayPtr);
        }
        case epics::pvData::pvShort: {
            return getScalarArrayAsNumPyArray<epics::pvData::PVShortArray, short>(pvScalarArrayPtr);
        }
        case epics::pvData::pvUShort: {
            return getScalarArrayAsNumPyArray<epics::pvData::PVUShortArray, ushort>(pvScalarArrayPtr);
        }
        case epics::pvData::pvInt: {
            return getScalarArrayAsNumPyArray<epics::pvData::PVIntArray, int32_t>(pvScalarArrayPtr);
        }
        case epics::pvData::pvUInt: {
            return getScalarArrayAsNumPyArray<epics::pvData::PVUIntArray, uint32_t>(pvScalarArrayPtr);
        }
        case epics::pvData::pvLong: {
            return getScalarArrayAsNumPyArray<epics::pvData::PVLongArray, int64_t>(pvScalarArrayPtr);
        }
        case epics::pvData::pvULong: {
            return getScalarArrayAsNumPyArray<epics::pvData::PVULongArray, uint64_t>(pvScalarArrayPtr);
        }
        case epics::pvData::pvFloat: {
            return getScalarArrayAsNumPyArray<epics::pvData::PVFloatArray, float>(pvScalarArrayPtr);
        }
        case epics::pvData::pvDouble: {
            return getScalarArrayAsNumPyArray<epics::pvData::PVDoubleArray, double>(pvScalarArrayPtr);
        }
        default: {
            throw PvaException("Unrecognized scalar type: %d", scalarType);
        }
    }
}

#endif // if defined HAVE_BOOST_NUM_PY && HAVE_BOOST_NUM_PY == 1

} // namespace PyPvDataUtility


