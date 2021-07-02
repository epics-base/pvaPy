// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python.hpp"
#include "PyPvDataUtility.h"
#include "PvType.h"
#include "PvaConstants.h"
#include "FieldNotFound.h"
#include "InvalidDataType.h"
#include "InvalidArgument.h"
#include "InvalidRequest.h"
#include "PvObject.h"
#if PVA_API_VERSION == 440
#include "pv/convert.h"
#endif // if PVA_API_VERSION == 440

namespace pvd = epics::pvData;
namespace bp = boost::python;
#if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1
namespace np = numpy_;
#endif // if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1

// Scalar array utilities
namespace PyPvDataUtility
{

//
// Checks
//
void checkFieldExists(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::PVFieldPtr pvFieldPtr = pvStructurePtr->getSubField(fieldName);
    if (!pvFieldPtr) {
        throw FieldNotFound("Object does not have field " + fieldName);
    }
}

void checkFieldPathExists(const std::string& fieldPath, const pvd::PVStructurePtr& pvStructurePtr)
{
    std::vector<std::string> fieldNames = StringUtility::split(fieldPath);
    pvd::PVStructurePtr pvStructurePtr2 = getParentStructureForFieldPath(fieldNames, pvStructurePtr);
    int nElements = fieldNames.size();
    
    // Last field in the path is what we want.
    std::string fieldName = fieldNames[nElements-1];
    checkFieldExists(fieldName, pvStructurePtr2);
}

//
// Field retrieval
//
std::string getValueOrSingleFieldName(const pvd::PVStructurePtr& pvStructurePtr)
{
    // If structure has value field key, return it.
    pvd::StructureConstPtr structurePtr = pvStructurePtr->getStructure();
    pvd::StringArray fieldNames = structurePtr->getFieldNames();
    for (unsigned int i = 0; i < fieldNames.size(); i++) {
        std::string fieldName = fieldNames[i];
        if (fieldName == PvaConstants::ValueFieldKey) {
            return PvaConstants::ValueFieldKey;
        }
    }
    if (fieldNames.size() > 1) {
        throw InvalidRequest("Ambiguous request: object has multiple fields, but no %s field", PVA_VALUE_FIELD_KEY);
    }
    // Structure has only one field key.
    return fieldNames[0];
}

std::string getValueOrSelectedUnionFieldName(const pvd::PVStructurePtr& pvStructurePtr)
{
    std::string fieldName = PvaConstants::ValueFieldKey;
    pvd::PVFieldPtr pvFieldPtr = pvStructurePtr->getSubField(fieldName);
    if (!pvFieldPtr) {
        pvd::PVUnionPtr pvUnionPtr = pvStructurePtr->getSubField<pvd::PVUnion>(fieldName);
        if (!pvUnionPtr) {
            throw InvalidRequest("Field " + fieldName + " is not a union");
        }
        fieldName = pvUnionPtr->getSelectedFieldName();
    }
    return fieldName;
}

pvd::PVFieldPtr getSubField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::PVFieldPtr pvFieldPtr = pvStructurePtr->getSubField(fieldName);
    if (!pvFieldPtr) {
        throw FieldNotFound("Object does not have subfield " + fieldName);
    }
    return pvFieldPtr;
}

pvd::FieldConstPtr getField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::PVFieldPtr pvFieldPtr = pvStructurePtr->getSubField(fieldName);
    if (!pvFieldPtr) {
        throw FieldNotFound("Object does not have field " + fieldName);
    }
    return pvFieldPtr->getField();
}

pvd::PVStructurePtr getParentStructureForFieldPath(const std::string& fieldPath, const pvd::PVStructurePtr& pvStructurePtr)
{
    std::vector<std::string> fieldNames = StringUtility::split(fieldPath);
    return getParentStructureForFieldPath(fieldNames, pvStructurePtr);
}

pvd::PVStructurePtr getParentStructureForFieldPath(const std::vector<std::string>& fieldNames, const pvd::PVStructurePtr& pvStructurePtr)
{
    // All path parts except for the last one must be structures
    pvd::PVStructurePtr pvStructurePtr2 = pvStructurePtr;
    int nElements = fieldNames.size();
    for (int i = 0; i < nElements-1; i++) {
        std::string fieldName = fieldNames[i];
        pvStructurePtr2 = getStructureField(fieldName, pvStructurePtr2);
    }
    return pvStructurePtr2;
}

pvd::ScalarConstPtr getScalarField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr);
    pvd::ScalarConstPtr scalarPtr = std::tr1::static_pointer_cast<const pvd::Scalar>(fieldPtr);
    if (!scalarPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a scalar");
    }
    return scalarPtr;
}

pvd::PVScalarArrayPtr getScalarArrayField(const std::string& fieldName, pvd::ScalarType scalarType, const pvd::PVStructurePtr& pvStructurePtr)
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVScalarArrayPtr pvScalarArrayPtr = pvStructurePtr->getSubField<pvd::PVScalarArray>(fieldName);
    if (!pvScalarArrayPtr || pvScalarArrayPtr->getScalarArray()->getElementType() != scalarType) {
        throw InvalidRequest("Field %s is not a scalar array of type %d", fieldName.c_str(), scalarType);
    }
    return pvScalarArrayPtr; 
}

pvd::StructureConstPtr getStructure(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr);
    pvd::StructureConstPtr structurePtr = std::tr1::static_pointer_cast<const pvd::Structure>(fieldPtr);
    if (!structurePtr) {
        throw InvalidRequest("Field " + fieldName + " is not a structure");
    }
    return structurePtr;
}

pvd::PVStructurePtr getStructureField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVStructurePtr pvStructurePtr2 = pvStructurePtr->getSubField<pvd::PVStructure>(fieldName);
    if (!pvStructurePtr2) {
        throw InvalidRequest("Field " + fieldName + " is not a structure");
    }
    return pvStructurePtr2;
}

pvd::PVStructureArrayPtr getStructureArrayField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVStructureArrayPtr pvStructureArrayPtr = pvStructurePtr->getSubField<pvd::PVStructureArray>(fieldName);
    if (!pvStructureArrayPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a structure array");
    }
    return pvStructureArrayPtr;
}

pvd::PVUnionPtr getUnionField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVUnionPtr pvUnionPtr = pvStructurePtr->getSubField<pvd::PVUnion>(fieldName);
    if (!pvUnionPtr) {
        throw InvalidRequest("Field " + fieldName + " is not an union");
    }
    return pvUnionPtr;
}

void setUnionField(const pvd::PVFieldPtr& pvFrom, pvd::PVUnionPtr pvUnionPtr)
{
    if (pvUnionPtr->getUnion()->isVariant()) {
        pvUnionPtr->set(pvFrom);
    }
    else {
        pvd::FieldConstPtr field = pvFrom->getField();
        std::string fieldName = pvFrom->getFieldName();
        int fieldIndex = -1;
        pvd::FieldConstPtrArray fields = pvUnionPtr->getUnion()->getFields();
        pvd::StringArray fieldNames = pvUnionPtr->getUnion()->getFieldNames();
        for (size_t i = 0; i < fields.size(); ++i) {
            pvd::FieldConstPtr unionField = fields[i];
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

pvd::PVUnionArrayPtr getUnionArrayField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVUnionArrayPtr pvUnionArrayPtr = pvStructurePtr->getSubField<pvd::PVUnionArray>(fieldName);
    if (!pvUnionArrayPtr) {
        throw InvalidRequest("Field " + fieldName + " is not an union array");
    }
    return pvUnionArrayPtr;
}

pvd::PVBooleanPtr getBooleanField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVBooleanPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVBoolean>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a boolean");
    }
    return fieldPtr;
}

pvd::PVBytePtr getByteField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVBytePtr fieldPtr = pvStructurePtr->getSubField<pvd::PVByte>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a byte");
    }
    return fieldPtr;
}

pvd::PVUBytePtr getUByteField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVUBytePtr fieldPtr = pvStructurePtr->getSubField<pvd::PVUByte>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not an unsigned byte");
    }
    return fieldPtr;
}

pvd::PVShortPtr getShortField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVShortPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVShort>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a short");
    }
    return fieldPtr;
}

pvd::PVUShortPtr getUShortField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVUShortPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVUShort>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not an unsigned short");
    }
    return fieldPtr;
}

pvd::PVIntPtr getIntField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVIntPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVInt>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not an int");
    }
    return fieldPtr;
}

pvd::PVUIntPtr getUIntField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVUIntPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVUInt>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not an unsigned int");
    }
    return fieldPtr;
}

pvd::PVLongPtr getLongField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVLongPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVLong>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a long");
    }
    return fieldPtr;
}

pvd::PVULongPtr getULongField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVULongPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVULong>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not an unsigned long");
    }
    return fieldPtr;
}

pvd::PVFloatPtr getFloatField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVFloatPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVFloat>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a float");
    }
    return fieldPtr;
}

pvd::PVDoublePtr getDoubleField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVDoublePtr fieldPtr = pvStructurePtr->getSubField<pvd::PVDouble>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a double");
    }
    return fieldPtr;
}

pvd::PVStringPtr getStringField(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr) 
{
    checkFieldExists(fieldName, pvStructurePtr);
    pvd::PVStringPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVString>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Field " + fieldName + " is not a string");
    }
    return fieldPtr;
}

//
// Field type retrieval
//
pvd::Type getFieldType(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr);
    return fieldPtr->getType();
}

pvd::ScalarType getScalarType(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::ScalarConstPtr scalarPtr = getScalarField(fieldName, pvStructurePtr);
    return scalarPtr->getScalarType();
}

pvd::ScalarType getScalarArrayType(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr);
    pvd::Type type = fieldPtr->getType();
    if (type != pvd::scalarArray) {
        throw InvalidRequest("Object does not have scalar array field " + fieldName);
    }
    pvd::ScalarArrayConstPtr scalarArrayPtr = std::tr1::static_pointer_cast<const pvd::ScalarArray>(fieldPtr);
    pvd::ScalarType scalarType = scalarArrayPtr->getElementType();
    return scalarType;
}

//
// Conversion PY object => PV Field
//
void pyObjectToField(const bp::object& pyObject, const std::string& fieldName, pvd::PVStructurePtr& pvStructurePtr)
{
    // Find field in pv structure
    pvd::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr);
    pvd::Type type = fieldPtr->getType();
    switch (type) {
        case pvd::scalar: {
            pyObjectToScalarField(pyObject, fieldName, pvStructurePtr);
            break;
        }
        case pvd::structure: {
            pyObjectToStructureField(pyObject, fieldName, pvStructurePtr);
            break;
        }
        case pvd::scalarArray: {
            pyObjectToScalarArrayField(pyObject, fieldName, pvStructurePtr);
            break;
        }
        case pvd::structureArray: {
            pyObjectToStructureArrayField(pyObject, fieldName, pvStructurePtr);
            break;
        }
        case pvd::union_: {
            pyObjectToUnionField(pyObject, fieldName, pvStructurePtr);
            break;
        }
        case pvd::unionArray: {
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
void pyObjectToScalarField(const bp::object& pyObject, const std::string& fieldName, pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::ScalarType scalarType = getScalarType(fieldName, pvStructurePtr);
    switch (scalarType) {
        case pvd::pvBoolean: {
            pvd::PVBooleanPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVBoolean>(fieldName);
            bool value = PyUtility::extractValueFromPyObject<bool>(pyObject);
            fieldPtr->put(static_cast<pvd::boolean>(value));
            break;
        }
        case pvd::pvByte: {
            pvd::PVBytePtr fieldPtr = pvStructurePtr->getSubField<pvd::PVByte>(fieldName);
            char value = PyUtility::extractValueFromPyObject<char>(pyObject);
            fieldPtr->put(static_cast<pvd::int8>(value));
            break;
        }
        case pvd::pvUByte: {
            pvd::PVUBytePtr fieldPtr = pvStructurePtr->getSubField<pvd::PVUByte>(fieldName);
            unsigned char value = PyUtility::extractValueFromPyObject<unsigned char>(pyObject);
            fieldPtr->put(static_cast<pvd::uint8>(value));
            break;
        }
        case pvd::pvShort: {
            pvd::PVShortPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVShort>(fieldName);
            int16_t value = PyUtility::extractValueFromPyObject<int16_t>(pyObject);
            fieldPtr->put(static_cast<pvd::int16>(value));
            break;
        }
        case pvd::pvUShort: {
            pvd::PVUShortPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVUShort>(fieldName);
            uint16_t value = PyUtility::extractValueFromPyObject<uint16_t>(pyObject);
            fieldPtr->put(static_cast<pvd::uint16>(value));
            break;
        }
        case pvd::pvInt: {
            pvd::PVIntPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVInt>(fieldName);
            int value = PyUtility::extractValueFromPyObject<int>(pyObject);
            fieldPtr->put(static_cast<pvd::int32>(value));
            break;
        }
        case pvd::pvUInt: {
            pvd::PVUIntPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVUInt>(fieldName);
            unsigned int value = PyUtility::extractValueFromPyObject<unsigned int>(pyObject);
            fieldPtr->put(static_cast<pvd::uint32>(value));
            break;
        }
        case pvd::pvLong: {
            pvd::PVLongPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVLong>(fieldName);
            long long value = PyUtility::extractValueFromPyObject<long long>(pyObject);
            fieldPtr->put(static_cast<pvd::int64>(value));
            break;
        }
        case pvd::pvULong: {
            pvd::PVULongPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVULong>(fieldName);
            unsigned long long value = PyUtility::extractValueFromPyObject<unsigned long long>(pyObject);
            fieldPtr->put(static_cast<pvd::uint64>(value));
            break;
        }
        case pvd::pvFloat: {
            pvd::PVFloatPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVFloat>(fieldName);
            float value = PyUtility::extractValueFromPyObject<float>(pyObject);
            fieldPtr->put(value);
            break;
        }
        case pvd::pvDouble: {
            pvd::PVDoublePtr fieldPtr = pvStructurePtr->getSubField<pvd::PVDouble>(fieldName);
            double value = PyUtility::extractValueFromPyObject<double>(pyObject);
            fieldPtr->put(value);
            break;
        }
        case pvd::pvString: {
            pvd::PVStringPtr fieldPtr = pvStructurePtr->getSubField<pvd::PVString>(fieldName);
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
void pyObjectToScalarArrayField(const bp::object& pyObject, const std::string& fieldName, pvd::PVStructurePtr& pvStructurePtr)
{
    if (PyUtility::isPyList(pyObject)) {
        bp::list pyList = PyUtility::extractValueFromPyObject<bp::list>(pyObject);
        pyListToScalarArrayField(pyList, fieldName, pvStructurePtr);
    }
#if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1
    else if (PyUtility::isNumPyNDArray(pyObject)) {
        np::ndarray ndArray = PyUtility::extractValueFromPyObject<np::ndarray>(pyObject);
        setScalarArrayFieldFromNumPyArray(ndArray, fieldName, pvStructurePtr);
    }
#endif // if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1
    else {
        throw InvalidDataType("Dictionary key %s must be a list.", fieldName.c_str());
    }
}

//
// Conversion PY object => PV Structure
//
void pyObjectToStructureField(const bp::object& pyObject, const std::string& fieldName, pvd::PVStructurePtr& pvStructurePtr)
{
    bp::extract<PvObject> extractPvObject(pyObject);
    if (extractPvObject.check()) {
        PvObject pvObject = extractPvObject();
        pvd::PVStructurePtr pvStructurePtr2 = getStructureField(fieldName, pvStructurePtr);
        copyStructureToStructure(pvObject.getPvStructurePtr(), pvStructurePtr2);
    }
    else {
        bp::dict pyDict;
        pyDict = PyUtility::extractValueFromPyObject<bp::dict>(pyObject);
        pyDictToStructureField(pyDict, fieldName, pvStructurePtr);
    }
}

//
// Conversion PY object => PV Structure Array
//
void pyObjectToStructureArrayField(const bp::object& pyObject, const std::string& fieldName, pvd::PVStructurePtr& pvStructurePtr)
{
    bp::list pyList = PyUtility::extractValueFromPyObject<bp::list>(pyObject);
    pyListToStructureArrayField(pyList, fieldName, pvStructurePtr);
}

//
// Conversion PY object => PV Union
//
void pyObjectToUnionField(const bp::object& pyObject, const std::string& fieldName, pvd::PVStructurePtr& pvStructurePtr)
{
    // First dictionary within tuple, then dictionary
    bp::object pyObject2 = pyObject;
    bp::extract<bp::tuple> extractTuple(pyObject);
    if (extractTuple.check()) {
        bp::tuple pyTuple = extractTuple();
        pyObject2 = pyTuple[0];
    }    
    bp::extract<PvObject> extractPvObject(pyObject2);
    if (extractPvObject.check()) {
        PvObject pvObject = extractPvObject();
        pvObjectToUnionField(pvObject, fieldName, pvStructurePtr);
    }
    else {
        bp::dict pyDict = PyUtility::extractValueFromPyObject<bp::dict>(pyObject2);
        pyDictToUnionField(pyDict, fieldName, pvStructurePtr);
    }
}

//
// Conversion PY object => PV Union Array
//
void pyObjectToUnionArrayField(const bp::object& pyObject, const std::string& fieldName, pvd::PVStructurePtr& pvStructurePtr)
{
    bp::list pyList = PyUtility::extractValueFromPyObject<bp::list>(pyObject);
    pyListToUnionArrayField(pyList, fieldName, pvStructurePtr);
}

//
// Special method for boolean array => PY List
//
void booleanArrayToPyList(const pvd::PVScalarArrayPtr& pvScalarArrayPtr, bp::list& pyList)
{
    int nDataElements = pvScalarArrayPtr->getLength();
    pvd::PVBooleanArray::const_svector data;
    pvScalarArrayPtr->PVScalarArray::getAs<pvd::boolean>(data);
    for (int i = 0; i < nDataElements; ++i) {
        pyList.append(static_cast<bool>(data[i]));
    }
}

//
// Conversion PV Scalar Array => PY List
//
void scalarArrayFieldToPyList(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr, bp::list& pyList)
{
    pvd::ScalarType scalarType = getScalarArrayType(fieldName, pvStructurePtr);
    pvd::PVScalarArrayPtr pvScalarArrayPtr = pvStructurePtr->getSubField<pvd::PVScalarArray>(fieldName);

    switch (scalarType) {
        case pvd::pvBoolean: {
            booleanArrayToPyList(pvScalarArrayPtr, pyList);
            break;
        }
        case pvd::pvByte: {
            scalarArrayToPyList<pvd::PVByteArray, pvd::int8>(pvScalarArrayPtr, pyList);
            break;
        }
        case pvd::pvUByte: {
            scalarArrayToPyList<pvd::PVUByteArray, pvd::uint8>(pvScalarArrayPtr, pyList);
            break;
        }
        case pvd::pvShort: {
            scalarArrayToPyList<pvd::PVShortArray, pvd::int16>(pvScalarArrayPtr, pyList);
            break;
        }
        case pvd::pvUShort: {
            scalarArrayToPyList<pvd::PVUShortArray, pvd::uint16>(pvScalarArrayPtr, pyList);
            break;
        }
        case pvd::pvInt: {
            scalarArrayToPyList<pvd::PVIntArray, pvd::int32>(pvScalarArrayPtr, pyList);
            break;
        }
        case pvd::pvUInt: {
            scalarArrayToPyList<pvd::PVUIntArray, pvd::uint32>(pvScalarArrayPtr, pyList);
            break;
        }
        case pvd::pvLong: {
            scalarArrayToPyList<pvd::PVLongArray, pvd::int64>(pvScalarArrayPtr, pyList);
            break;
        }
        case pvd::pvULong: {
            scalarArrayToPyList<pvd::PVULongArray, pvd::uint64>(pvScalarArrayPtr, pyList);
            break;
        }
        case pvd::pvFloat: {
            scalarArrayToPyList<pvd::PVFloatArray, float>(pvScalarArrayPtr, pyList);
            break;
        }
        case pvd::pvDouble: {
            scalarArrayToPyList<pvd::PVDoubleArray, double>(pvScalarArrayPtr, pyList);
            break;
        }
        case pvd::pvString: {
            scalarArrayToPyList<pvd::PVStringArray, std::string>(pvScalarArrayPtr, pyList);
            break;
        }
        default: {
            throw PvaException("Unrecognized scalar type: %d", scalarType);
        }
    }
}

bp::list getScalarArrayFieldAsPyList(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    bp::list pyList;
    scalarArrayFieldToPyList(fieldName, pvStructurePtr, pyList);
    return pyList;
}


//
// Conversion PY List => PV Scalar Array
//
void pyListToScalarArrayField(const bp::list& pyList, const std::string& fieldName, pvd::PVStructurePtr& pvStructurePtr) 
{
    pvd::ScalarType scalarType = getScalarArrayType(fieldName, pvStructurePtr);
    switch (scalarType) {
        case pvd::pvBoolean: {
            pyListToScalarArrayField<pvd::PVBooleanArray, pvd::boolean, bool>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvByte: {
            pyListToScalarArrayField<pvd::PVByteArray, pvd::int8, char>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvUByte: {
            pyListToScalarArrayField<pvd::PVUByteArray, pvd::uint8, unsigned char>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvShort: {
            pyListToScalarArrayField<pvd::PVShortArray, pvd::int16, int16_t>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvUShort: {
            pyListToScalarArrayField<pvd::PVUShortArray, pvd::uint16, uint16_t>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvInt: {
            pyListToScalarArrayField<pvd::PVIntArray, pvd::int32, int>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvUInt: {
            pyListToScalarArrayField<pvd::PVUIntArray, pvd::uint32, unsigned int>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvLong: {
            pyListToScalarArrayField<pvd::PVLongArray, pvd::int64, long long>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvULong: {
            pyListToScalarArrayField<pvd::PVULongArray, pvd::uint64, unsigned long long>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvFloat: {
            pyListToScalarArrayField<pvd::PVFloatArray, float, float>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvDouble: {
            pyListToScalarArrayField<pvd::PVDoubleArray, double, double>(pyList, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvString: {
            pyListToScalarArrayField<pvd::PVStringArray, std::string, std::string>(pyList, fieldName, pvStructurePtr);
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
void pyDictToStructure(const bp::dict& pyDict, pvd::PVStructurePtr& pvStructurePtr)
{
    bp::list keys = pyDict.keys();
    for (int i = 0; i < bp::len(keys); i++) {
        bp::object keyObject = keys[i];
        bp::extract<std::string> keyExtract(keyObject);
        std::string key;
        if (keyExtract.check()) {
            key = keyExtract();
        }
        else {
            throw InvalidDataType("Dictionary key must be a string");
        }
        bp::object pyObject = pyDict[keyObject];
        pyObjectToField(pyObject, key, pvStructurePtr);
    }
}

//
// Conversion PY {} => PV Structure Field
//
void pyDictToStructureField(const bp::dict& pyDict, const std::string& fieldName, pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::PVStructurePtr pvStructurePtr2 = getStructureField(fieldName, pvStructurePtr);
    pyDictToStructure(pyDict, pvStructurePtr2);
}

//
// Conversion PY {} => PV Union
//
void pyDictToUnion(const bp::dict& pyDict, pvd::PVUnionPtr& pvUnionPtr)
{
    pvd::PVFieldPtr pvField;
    std::string unionFieldName;
    int dictSize = bp::len(pyDict);
    if (dictSize != 1) {
        throw InvalidArgument("Dictionary representing union value must have exactly one element.");
    }
    bp::list keys = pyDict.keys();
    bp::object keyObject = keys[0];
    bp::extract<std::string> keyExtract(keyObject);
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

    pvd::PVStructurePtr unionPvStructurePtr;
    if(pvField) {
        pvd::StructureConstPtr unionStructurePtr = pvd::getFieldCreate()->createFieldBuilder()->add(unionFieldName, pvField->getField())->createStructure();
        unionPvStructurePtr = pvd::getPVDataCreate()->createPVStructure(unionStructurePtr);
    }
    else {
        unionPvStructurePtr = pvd::getPVDataCreate()->createPVStructure(pvd::getFieldCreate()->createStructure());
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
void pyTupleToUnionField(const bp::tuple& pyTuple, const std::string& fieldName, pvd::PVStructurePtr& pvStructurePtr)
{
    // Extract dictionary within tuple
    if (bp::len(pyTuple) != 1) {
        throw InvalidArgument("PV union tuple provided for field name %s must have exactly one element.", fieldName.c_str());
    }
    bp::object pyObject = pyTuple[0];
    bp::dict pyDict = PyUtility::extractValueFromPyObject<bp::dict>(pyObject);
    pyDictToUnionField(pyDict, fieldName, pvStructurePtr);
}

//
// Conversion PY {} => PV Union Field
//
void pyDictToUnionField(const bp::dict& pyDict, const std::string& fieldName, pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::PVUnionPtr pvUnionPtr = getUnionField(fieldName, pvStructurePtr);
    pyDictToUnion(pyDict, pvUnionPtr);
}

//
// Conversion PvObject => PV Union Field
//
void pvObjectToUnionField(const PvObject& pvObject, const std::string& fieldName, pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::PVUnionPtr pvUnionPtr = getUnionField(fieldName, pvStructurePtr);

    std::string keyFrom = PyPvDataUtility::getValueOrSingleFieldName(pvObject.getPvStructurePtr());
    pvd::PVFieldPtr pvFrom = PyPvDataUtility::getSubField(keyFrom, pvObject.getPvStructurePtr());
    PyPvDataUtility::setUnionField(pvFrom, pvUnionPtr);
}

//
// Conversion PY [{}] => PV Structure Array
//
void pyListToStructureArrayField(const bp::list& pyList, const std::string& fieldName, pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::PVStructureArrayPtr pvStructureArrayPtr = getStructureArrayField(fieldName, pvStructurePtr);
    pvd::StructureArrayConstPtr structureArrayPtr = pvStructureArrayPtr->getStructureArray();
    pvd::StructureConstPtr structurePtr = structureArrayPtr->getStructure();
    int listSize = bp::len(pyList);
    pvd::PVStructureArray::svector pvStructures(listSize);
    for (int i = 0; i < listSize; i++) {
        pvd::PVStructurePtr pvStructure = pvd::getPVDataCreate()->createPVStructure(structurePtr);
        bp::extract<PvObject> extractPvObject(pyList[i]);
        if (extractPvObject.check()) {
            PvObject pvObject = extractPvObject();
            copyStructureToStructure(pvObject.getPvStructurePtr(), pvStructure);
            pvStructures[i] = pvStructure;
        }
        else {
            bp::extract<bp::dict> dictExtract(pyList[i]);
            if (dictExtract.check()) {
                bp::dict pyDict = dictExtract();
                pyDictToStructure(pyDict, pvStructure);
                pvStructures[i] = pvStructure;
            }
            else {
                throw InvalidDataType("Invalid data type for element %d", i);
            }
        }
    }
    pvStructureArrayPtr->setCapacity(listSize);
    pvStructureArrayPtr->replace(freeze(pvStructures));
}

//
// Conversion PY [{}] => PV Union Array
//
void pyListToUnionArrayField(const bp::list& pyList, const std::string& fieldName, pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::PVUnionArrayPtr pvUnionArrayPtr = getUnionArrayField(fieldName, pvStructurePtr);

    pvd::UnionConstPtr unionPtr = pvUnionArrayPtr->getUnionArray()->getUnion();
    int listSize = bp::len(pyList);
    pvd::PVUnionArray::svector data(listSize);
    for(size_t i = 0; i < data.size(); ++i) {
        pvd::PVUnionPtr pvUnionPtr = pvd::getPVDataCreate()->createPVUnion(unionPtr);
        bp::object pyObject = pyList[i];

        bp::extract<PvObject> extractPvObject(pyObject);
        if (extractPvObject.check()) {
            PvObject pvObject = extractPvObject();
            std::string keyFrom = getValueOrSingleFieldName(pvObject.getPvStructurePtr());
            pvd::PVFieldPtr pvFrom = PyPvDataUtility::getSubField(keyFrom, pvObject.getPvStructurePtr());
            PyPvDataUtility::setUnionField(pvFrom, pvUnionPtr);
            data[i] = pvUnionPtr;
            continue;
        }

        bp::extract<bp::tuple> extractTuple(pyObject);
        if (extractTuple.check()) {
            bp::tuple pyTuple = extractTuple();
            // Extract dictionary within tuple
            if (bp::len(pyTuple) != 1) {
                throw InvalidArgument("PV union tuple must have exactly one element.");
            }
            bp::object pyObject = pyTuple[0];
            bp::dict pyDict = PyUtility::extractValueFromPyObject<bp::dict>(pyObject);
            pyDictToUnion(pyDict, pvUnionPtr);
            data[i] = pvUnionPtr;
            continue;
        }

        bp::extract<bp::dict> extractDict(pyObject);
        if (extractDict.check()) {
            bp::dict pyDict = extractDict();
            PyPvDataUtility::pyDictToUnion(pyDict, pvUnionPtr);
            data[i] = pvUnionPtr;
            continue;
        }

        throw InvalidArgument("Python object representing an union must be PvObject, tuple containing dictionary, or dictionary.");

    }
    pvUnionArrayPtr->setCapacity(listSize);
    pvUnionArrayPtr->replace(freeze(data));
}

//
// Conversion PV Structure Array => PY [{}] 
//
void structureArrayFieldToPyList(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr, bp::list& pyList, bool useNumPyArrays)
{
    pvd::PVStructureArrayPtr pvStructureArrayPtr = getStructureArrayField(fieldName, pvStructurePtr);
    int nDataElements = pvStructureArrayPtr->getLength();
    pvd::PVStructureArray::const_svector arrayData(pvStructureArrayPtr->view());
    for (int i = 0; i < nDataElements; ++i) {
        pvd::PVStructurePtr pvStructure = arrayData[i];
        bp::dict pyDict;
        structureToPyDict(pvStructure, pyDict, useNumPyArrays);
        pyList.append(pyDict);
    }
}

//
// Conversion PV String Array => PY List
//
void stringArrayToPyList(const pvd::StringArray& stringArray, bp::list& pyList)
{
    for(size_t i = 0; i < stringArray.size(); ++i) {
        pyList.append(stringArray[i]);
    }  
}

//
// Conversion PV Structure => PY {}
//
void structureToPyDict(const pvd::PVStructurePtr& pvStructurePtr, bp::dict& pyDict, bool useNumPyArrays)
{
    pvd::StructureConstPtr structurePtr = pvStructurePtr->getStructure();
    pvd::StringArray fieldNames = structurePtr->getFieldNames();
    for (unsigned int i = 0; i < fieldNames.size(); ++i) {
        std::string fieldName = fieldNames[i];
        pvd::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr);
        pvd::Type type = fieldPtr->getType();
        switch (type) {
            case pvd::scalar: {
                addScalarFieldToDict(fieldName, pvStructurePtr, pyDict);
                break;
            }
            case pvd::scalarArray: {
                addScalarArrayFieldToDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays);
                break;
            }
            case pvd::structure: {
                addStructureFieldToDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays);
                break;
            }
            case pvd::structureArray: {
                addStructureArrayFieldToDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays);
                break;
            }
            case pvd::union_: {
                addUnionFieldToDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays);
                break;
            }
            case pvd::unionArray: {
                addUnionArrayFieldToDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays);
                break;
            }
            default: {
                throw PvaException("Unrecognized field type: %d", type);
            }
        }
    }
}

void structureFieldToPyDict(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr, bp::dict& pyDict, bool useNumPyArrays)
{
    structureToPyDict(getStructureField(fieldName, pvStructurePtr), pyDict, useNumPyArrays);
}

//
// Add PV Scalar => PY {}
// 
void addScalarFieldToDict(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr, bp::dict& pyDict)
{
    pvd::ScalarType scalarType = getScalarType(fieldName, pvStructurePtr);
    switch (scalarType) {
        case pvd::pvBoolean: {
            bool value = getBooleanField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case pvd::pvByte: {
            char value = getByteField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case pvd::pvUByte: {
            unsigned char value = getUByteField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case pvd::pvShort: {
            int16_t value = getShortField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case pvd::pvUShort: {
            uint16_t value = getUShortField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case pvd::pvInt: {
            int32_t value = getIntField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case pvd::pvUInt: {
            uint32_t value = getUIntField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case pvd::pvLong: {
            int64_t value = getLongField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case pvd::pvULong: {
            uint64_t value = getULongField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case pvd::pvFloat: {
            float value = getFloatField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case pvd::pvDouble: {
            double value = getDoubleField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        case pvd::pvString: {
            std::string value = getStringField(fieldName, pvStructurePtr)->get();
            pyDict[fieldName] = value;
            break;
        }
        default: {
            throw InvalidDataType("Unrecognized scalar type: %d", scalarType);
        }
    }
}

bp::object getScalarFieldAsPyObject(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    bp::dict pyDict;
    addScalarFieldToDict(fieldName, pvStructurePtr, pyDict);
    return pyDict[fieldName];
}

//
// Add PV Scalar Array => PY {}
// 
void addScalarArrayFieldToDict(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr, bp::dict& pyDict, bool useNumPyArrays)
{
    pyDict[fieldName] = getScalarArrayFieldAsPyObject(fieldName, pvStructurePtr, useNumPyArrays); 
}


bp::object getScalarArrayFieldAsPyObject(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr, bool useNumPyArrays)
{

// Only use NumPy arrays if support is compiled in and the corresponding
// flag is set 
#if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1
    pvd::ScalarType scalarType = getScalarArrayType(fieldName, pvStructurePtr);
    if (useNumPyArrays && scalarType != pvd::pvString) {
        return getScalarArrayFieldAsNumPyArray(fieldName, pvStructurePtr);
    }
    else {
        return getScalarArrayFieldAsPyList(fieldName, pvStructurePtr);
    }
#else
    return getScalarArrayFieldAsPyList(fieldName, pvStructurePtr);
#endif // if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1
}


//
// Add PV Structure => PY {}
// 
void addStructureFieldToDict(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr, bp::dict& pyDict, bool useNumPyArrays)
{
    bp::dict pyDict2;
    structureFieldToPyDict(fieldName, pvStructurePtr, pyDict2, useNumPyArrays);
    pyDict[fieldName] = pyDict2;
}

bp::object getStructureFieldAsPyObject(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr, bool useNumPyArrays)
{
    bp::dict pyDict;
    structureFieldToPyDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays);
    return pyDict;
}

//
// Add PV Structure Array => PY {}
// 
void addStructureArrayFieldToDict(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr, bp::dict& pyDict, bool useNumPyArrays) 
{
    bp::list pyList;
    pvd::PVStructureArrayPtr structureArrayPtr = getStructureArrayField(fieldName, pvStructurePtr);
    int nDataElements = structureArrayPtr->getLength();
    pvd::PVStructureArray::const_svector arrayData(structureArrayPtr->view());
    for (int i = 0; i < nDataElements; ++i) {
        bp::dict pyDict2;
        structureToPyDict(arrayData[i], pyDict2, useNumPyArrays);   
        pyList.append(pyDict2);   
    }
    pyDict[fieldName] = pyList;
}

bp::object getStructureArrayFieldAsPyObject(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr, bool useNumPyArrays)
{
    bp::dict pyDict;
    addStructureArrayFieldToDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays); 
    return pyDict[fieldName];
}

//
// Get Union PV Structure Pointer
// 
pvd::PVStructurePtr getUnionPvStructurePtr(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::PVUnionPtr pvUnionPtr = pvStructurePtr->getSubField<pvd::PVUnion>(fieldName);
    std::string unionFieldName = PvaConstants::ValueFieldKey;
    pvd::PVFieldPtr pvField;
    if (!pvUnionPtr->getUnion()->isVariant()) {
        unionFieldName = pvUnionPtr->getSelectedFieldName();
        if (unionFieldName != "") {
            pvField = pvUnionPtr->select(unionFieldName);
        }
    }
    else {
        pvField = pvUnionPtr->get();
    }
    pvd::PVStructurePtr unionPvStructurePtr;
    if(pvField) {
        pvd::StructureConstPtr unionStructurePtr = pvd::getFieldCreate()->createFieldBuilder()->add(unionFieldName, pvField->getField())->createStructure();
        unionPvStructurePtr = pvd::getPVDataCreate()->createPVStructure(unionStructurePtr);
#if PVA_API_VERSION == 440
        pvd::Convert::getConvert()->copy(pvField, unionPvStructurePtr->getSubField(unionFieldName));
#else
        unionPvStructurePtr->getSubField(unionFieldName)->copy(*pvField);
#endif // if PVA_API_VERSION == 440
    }
    else {
        unionPvStructurePtr = pvd::getPVDataCreate()->createPVStructure(pvd::getFieldCreate()->createStructure());
    }
    return unionPvStructurePtr;
}

//
// Add PV Union => PY {}
// 
void addUnionFieldToDict(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr, bp::dict& pyDict, bool useNumPyArrays)
{
    pvd::PVStructurePtr unionPvStructurePtr = getUnionPvStructurePtr(fieldName, pvStructurePtr);
    bp::dict pyDict2;
    structureToPyDict(unionPvStructurePtr, pyDict2, useNumPyArrays);
    bp::tuple pyTuple = bp::make_tuple(pyDict2);
    pyDict[fieldName] = pyTuple;
}

bp::object getUnionFieldAsPyObject(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr, bool useNumPyArrays)
{
    bp::dict pyDict;
    addUnionFieldToDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays);
    return pyDict[fieldName];
}

//
// Add PV Union Array => PY {}
// 
void addUnionArrayFieldToDict(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr, bp::dict& pyDict, bool useNumPyArrays)
{
    pvd::PVUnionArrayPtr pvUnionArrayPtr = pvStructurePtr->getSubField<pvd::PVUnionArray>(fieldName);
    pvd::shared_vector<const pvd::PVUnionPtr> pvUnions = pvUnionArrayPtr->view();
    bp::list pyList;
    for(size_t i = 0 ; i < pvUnions.size(); ++i) {
        pvd::PVUnionPtr pvUnionPtr = pvUnions[i];
        std::string fieldName = PvaConstants::ValueFieldKey;
        pvd::PVFieldPtr pvField;
        if (!pvUnionPtr->getUnion()->isVariant()) {
            fieldName = pvUnionPtr->getSelectedFieldName();
            pvField = pvUnionPtr->select(fieldName);
        }
        else {
            pvField = pvUnionPtr->get();
        }

        pvd::PVStructurePtr unionPvStructurePtr;
        pvd::StructureConstPtr unionStructurePtr = pvd::getFieldCreate()->createFieldBuilder()->add(fieldName, pvField->getField())->createStructure();
        unionPvStructurePtr = pvd::getPVDataCreate()->createPVStructure(unionStructurePtr);
#if PVA_API_VERSION == 440
        pvd::Convert::getConvert()->copy(pvField, unionPvStructurePtr->getSubField(fieldName));
#else
        unionPvStructurePtr->getSubField(fieldName)->copy(*pvField);
#endif // if PVA_API_VERSION == 440

        bp::dict pyDict2;
        structureToPyDict(unionPvStructurePtr, pyDict2, useNumPyArrays);
        bp::tuple pyTuple = bp::make_tuple(pyDict2);
        pyList.append(pyTuple);
    }
    pyDict[fieldName] = pyList;
}

bp::object getUnionArrayFieldAsPyObject(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr, bool useNumPyArrays)
{
    bp::dict pyDict;
    addUnionArrayFieldToDict(fieldName, pvStructurePtr, pyDict, useNumPyArrays);
    return pyDict[fieldName];
}

//
// Conversion Structure => PY {}
//
void structureToPyDict(const pvd::StructureConstPtr& structurePtr, bp::dict& pyDict)
{
    pvd::StringArray fieldNames = structurePtr->getFieldNames();
    for (unsigned int i = 0; i < fieldNames.size(); ++i) {
        std::string fieldName = fieldNames[i];
        pvd::FieldConstPtr fieldPtr = structurePtr->getField(fieldName);
        fieldToPyDict(fieldPtr, fieldName, pyDict);
    }
}

//
// Conversion Union => PY ()
//
bp::tuple unionToPyTuple(const pvd::UnionConstPtr& unionPtr)
{
    // Variant union is represented as empty tuple
    if (unionPtr->isVariant()) {
        return bp::make_tuple();
    }

    // Restricted union is represented as dict inside tuple
    pvd::StringArray fieldNames = unionPtr->getFieldNames();
    bp::dict pyDict;
    for (unsigned int i = 0; i < fieldNames.size(); ++i) {
        std::string fieldName = fieldNames[i];
        pvd::FieldConstPtr fieldPtr = unionPtr->getField(fieldName);
        fieldToPyDict(fieldPtr, fieldName, pyDict);
    }
    bp::tuple pyTuple = bp::make_tuple(pyDict);
    return pyTuple;
}

//
// Conversion Field => PY {}
//
void fieldToPyDict(const pvd::FieldConstPtr& fieldPtr, const std::string& fieldName, bp::dict& pyDict)
{
    pvd::Type type = fieldPtr->getType();
    switch (type) {
        case pvd::scalar: {
            pvd::ScalarConstPtr scalarPtr = std::tr1::static_pointer_cast<const pvd::Scalar>(fieldPtr);
            pvd::ScalarType scalarType = scalarPtr->getScalarType();
            pyDict[fieldName] = static_cast<PvType::ScalarType>(scalarType);
            break;
        }
        case pvd::scalarArray: {
            pvd::ScalarArrayConstPtr scalarArrayPtr = std::tr1::static_pointer_cast<const pvd::ScalarArray>(fieldPtr);
            pvd::ScalarType scalarType = scalarArrayPtr->getElementType();
            bp::list pyList;
            pyList.append(static_cast<PvType::ScalarType>(scalarType));
            pyDict[fieldName] = pyList;
            break;
        }
        case pvd::structure: {
            pvd::StructureConstPtr structurePtr2 = std::tr1::static_pointer_cast<const pvd::Structure>(fieldPtr);
            bp::dict pyDict2;
            structureToPyDict(structurePtr2, pyDict2);
            pyDict[fieldName] = pyDict2;
            break;
        }
        case pvd::structureArray: {
            pvd::StructureArrayConstPtr structureArrayPtr = std::tr1::static_pointer_cast<const pvd::StructureArray>(fieldPtr);
            pvd::StructureConstPtr structurePtr2 = structureArrayPtr->getStructure(); 
            bp::dict pyDict2;
            structureToPyDict(structurePtr2, pyDict2);
            bp::list pyList;
            pyList.append(pyDict2);
            pyDict[fieldName] = pyList;
            break;
        }
        case pvd::union_: {
            pvd::UnionConstPtr unionPtr = std::tr1::static_pointer_cast<const pvd::Union>(fieldPtr);
            pyDict[fieldName] = unionToPyTuple(unionPtr);
            break;
        }
        case pvd::unionArray: {
            pvd::UnionArrayConstPtr unionArrayPtr = std::tr1::static_pointer_cast<const pvd::UnionArray>(fieldPtr);
            pvd::UnionConstPtr unionPtr = unionArrayPtr->getUnion();
            bp::list pyList;
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
void copyStructureToStructure(const pvd::PVStructurePtr& srcPvStructurePtr, pvd::PVStructurePtr& destPvStructurePtr)
{
#if PVA_API_VERSION >= 481
    if(*(srcPvStructurePtr->getStructure()) == *(destPvStructurePtr->getStructure())) {
        destPvStructurePtr->copyUnchecked(*srcPvStructurePtr);
    }
    else {
        copyStructureToStructure2(srcPvStructurePtr, destPvStructurePtr);
    }
#else
    copyStructureToStructure2(srcPvStructurePtr, destPvStructurePtr);
#endif // if PVA_API_VERSION >= 481
}


// Method used for older epics releases
void copyStructureToStructure2(const pvd::PVStructurePtr& srcPvStructurePtr, pvd::PVStructurePtr& destPvStructurePtr)
{
    pvd::StructureConstPtr srcStructurePtr = srcPvStructurePtr->getStructure();
    pvd::StringArray fieldNames = srcStructurePtr->getFieldNames();
    for (unsigned int i = 0; i < fieldNames.size(); ++i) {
        std::string fieldName = fieldNames[i];
        pvd::PVFieldPtr pvFieldPtr = srcPvStructurePtr->getSubField(fieldName);
        pvd::FieldConstPtr fieldPtr = pvFieldPtr->getField();
        pvd::Type type = fieldPtr->getType();
        switch (type) {
            case pvd::scalar: {
                pvd::ScalarConstPtr scalarPtr = std::tr1::static_pointer_cast<const pvd::Scalar>(fieldPtr);
                pvd::ScalarType scalarType = scalarPtr->getScalarType();
                copyScalarToStructure(fieldName, scalarType, srcPvStructurePtr, destPvStructurePtr);
                break;
            }
            case pvd::scalarArray: {
                pvd::ScalarArrayConstPtr scalarArrayPtr = std::tr1::static_pointer_cast<const pvd::ScalarArray>(fieldPtr);
                pvd::ScalarType scalarType = scalarArrayPtr->getElementType();
                copyScalarArrayToStructure(fieldName, scalarType, srcPvStructurePtr, destPvStructurePtr);
                break;
            }
            case pvd::structure: {
                copyStructureToStructure(fieldName, srcPvStructurePtr, destPvStructurePtr);
                break;
            }
            case pvd::structureArray: {
                copyStructureArrayToStructure(fieldName, srcPvStructurePtr, destPvStructurePtr);
                break;
            }
            case pvd::union_: {
                pvd::PVUnionPtr pvFrom = srcPvStructurePtr->getSubField<pvd::PVUnion>(fieldName);
                pvd::PVUnionPtr pvTo = destPvStructurePtr->getSubField<pvd::PVUnion>(fieldName);
#if PVA_API_VERSION == 440
                pvd::Convert::getConvert()->copyUnion(pvFrom, pvTo);
#else
                pvTo->copy(*pvFrom);
#endif // if PVA_API_VERSION == 440
                break;
            }
            case pvd::unionArray: {
                pvd::PVUnionArrayPtr pvFrom = srcPvStructurePtr->getSubField<pvd::PVUnionArray>(fieldName);
                pvd::PVUnionArrayPtr pvTo = destPvStructurePtr->getSubField<pvd::PVUnionArray>(fieldName);
                pvTo->replace(pvFrom->view());
                break;
            }
            default: {
                throw PvaException("Unrecognized field type: %d", type);
            }
        }
    }
}

void copyStructureToStructure(const std::string& fieldName, const pvd::PVStructurePtr& srcPvStructurePtr, pvd::PVStructurePtr& destPvStructurePtr)
{
    pvd::PVStructurePtr destPvStructurePtr2 = destPvStructurePtr->getSubField<pvd::PVStructure>(fieldName);
    if (destPvStructurePtr2) {
        pvd::PVStructurePtr srcPvStructurePtr2 = srcPvStructurePtr->getSubField<pvd::PVStructure>(fieldName);
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
void copyStructureArrayToStructure(const std::string& fieldName, const pvd::PVStructurePtr& srcPvStructurePtr, pvd::PVStructurePtr& destPvStructurePtr)
{
    pvd::PVStructureArrayPtr destPvStructureArrayPtr = getStructureArrayField(fieldName, destPvStructurePtr);
    if (!destPvStructureArrayPtr) {
        throw FieldNotFound("Destination structure has no structure array field " + fieldName);
    }
    pvd::StructureArrayConstPtr destStructureArrayPtr = destPvStructureArrayPtr->getStructureArray();
    pvd::StructureConstPtr structurePtr = destStructureArrayPtr->getStructure();

    pvd::PVStructureArrayPtr srcPvStructureArrayPtr = getStructureArrayField(fieldName, srcPvStructurePtr);
    if (!srcPvStructureArrayPtr) {
        throw FieldNotFound("Source structure has no structure array field " + fieldName);
    }


    int nElements = srcPvStructureArrayPtr->getLength();
    pvd::PVStructureArray::const_svector srcPvStructures(srcPvStructureArrayPtr->view());
    pvd::PVStructureArray::svector destPvStructures(nElements);

    for (int i = 0; i < nElements; i++) {
        pvd::PVStructurePtr destPvStructurePtr2 = pvd::getPVDataCreate()->createPVStructure(structurePtr);
        pvd::PVStructurePtr srcPvStructurePtr2 = srcPvStructures[i];
        copyStructureToStructure(srcPvStructurePtr2, destPvStructurePtr2);
        destPvStructures[i] = destPvStructurePtr2;
    }
    destPvStructureArrayPtr->setCapacity(nElements);
    destPvStructureArrayPtr->replace(freeze(destPvStructures));
}

//
// Copy PV Scalar => PV Structure
//
void copyScalarToStructure(const std::string& fieldName, pvd::ScalarType scalarType, const pvd::PVStructurePtr& srcPvStructurePtr, pvd::PVStructurePtr& destPvStructurePtr)
{
    switch (scalarType) {
        case pvd::pvBoolean: {
            pvd::PVBooleanPtr fieldPtr = destPvStructurePtr->getSubField<pvd::PVBoolean>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<pvd::PVBoolean>(fieldName)->get());
            }
            break;
        }
        case pvd::pvByte: {
            pvd::PVBytePtr fieldPtr = destPvStructurePtr->getSubField<pvd::PVByte>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<pvd::PVByte>(fieldName)->get());
            }
            break;
        }
        case pvd::pvUByte: {
            pvd::PVUBytePtr fieldPtr = destPvStructurePtr->getSubField<pvd::PVUByte>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<pvd::PVUByte>(fieldName)->get());
            }
            break;
        }
        case pvd::pvShort: {
            pvd::PVShortPtr fieldPtr = destPvStructurePtr->getSubField<pvd::PVShort>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<pvd::PVShort>(fieldName)->get());
            }
            break;
        }
        case pvd::pvUShort: {
            pvd::PVUShortPtr fieldPtr = destPvStructurePtr->getSubField<pvd::PVUShort>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<pvd::PVUShort>(fieldName)->get());
            }
            break;
        }
        case pvd::pvInt: {
            pvd::PVIntPtr fieldPtr = destPvStructurePtr->getSubField<pvd::PVInt>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<pvd::PVInt>(fieldName)->get());
            }
            break;
        }
        case pvd::pvUInt: {
            pvd::PVUIntPtr fieldPtr = destPvStructurePtr->getSubField<pvd::PVUInt>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<pvd::PVUInt>(fieldName)->get());
            }
            break;
        }
        case pvd::pvLong: {
            pvd::PVLongPtr fieldPtr = destPvStructurePtr->getSubField<pvd::PVLong>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<pvd::PVLong>(fieldName)->get());
            }
            break;
        }
        case pvd::pvULong: {
            pvd::PVULongPtr fieldPtr = destPvStructurePtr->getSubField<pvd::PVULong>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<pvd::PVULong>(fieldName)->get());
            }
            break;
        }
        case pvd::pvFloat: {
            pvd::PVFloatPtr fieldPtr = destPvStructurePtr->getSubField<pvd::PVFloat>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<pvd::PVFloat>(fieldName)->get());
            }
            break;
        }
        case pvd::pvDouble: {
            pvd::PVDoublePtr fieldPtr = destPvStructurePtr->getSubField<pvd::PVDouble>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<pvd::PVDouble>(fieldName)->get());
            }
            break;
        }
        case pvd::pvString: {
            pvd::PVStringPtr fieldPtr = destPvStructurePtr->getSubField<pvd::PVString>(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getSubField<pvd::PVString>(fieldName)->get());
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
void copyScalarArrayToStructure(const std::string& fieldName, pvd::ScalarType scalarType, const pvd::PVStructurePtr& srcPvStructurePtr, pvd::PVStructurePtr& destPvStructurePtr)
{
    pvd::PVScalarArrayPtr srcPvScalarArrayPtr = getScalarArrayField(fieldName, scalarType, srcPvStructurePtr);
    pvd::PVScalarArrayPtr destPvScalarArrayPtr = getScalarArrayField(fieldName, scalarType, destPvStructurePtr);

    switch (scalarType) {
        case pvd::pvBoolean: {
            copyScalarArrayToScalarArray<pvd::PVBooleanArray, pvd::boolean>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case pvd::pvByte: {
            copyScalarArrayToScalarArray<pvd::PVByteArray, pvd::int8>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case pvd::pvUByte: {
            copyScalarArrayToScalarArray<pvd::PVUByteArray, pvd::uint8>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case pvd::pvShort: {
            copyScalarArrayToScalarArray<pvd::PVShortArray, pvd::int16>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case pvd::pvUShort: {
            copyScalarArrayToScalarArray<pvd::PVUShortArray, pvd::uint16>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case pvd::pvInt: {
            copyScalarArrayToScalarArray<pvd::PVIntArray, pvd::int32>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case pvd::pvUInt: {
            copyScalarArrayToScalarArray<pvd::PVUIntArray, pvd::uint32>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case pvd::pvLong: {
            copyScalarArrayToScalarArray<pvd::PVLongArray, pvd::int64>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case pvd::pvULong: {
            copyScalarArrayToScalarArray<pvd::PVULongArray, pvd::uint64>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case pvd::pvFloat: {
            copyScalarArrayToScalarArray<pvd::PVFloatArray, float>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case pvd::pvDouble: {
            copyScalarArrayToScalarArray<pvd::PVDoubleArray, double>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case pvd::pvString: {
            copyScalarArrayToScalarArray<pvd::PVStringArray, std::string>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
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
bp::list createStructureList(PvType::ScalarType pvType)
{
    bp::list pyList;
    pyList.append(pvType);
    return pyList;
}

bp::list createStructureList(const bp::dict& pyDict)
{
    bp::list pyList;
    pyList.append(pyDict);
    return pyList;
}

pvd::StructureConstPtr createStructureFromDict(const bp::dict& pyDict, const std::string& structureId)
{
    pvd::FieldConstPtrArray fields;
    pvd::StringArray names;
    updateFieldArrayFromDict(pyDict, fields, names);

    std::string structureName = StringUtility::trim(structureId);
    if (structureName.length()) {
        return pvd::getFieldCreate()->createStructure(structureName, names, fields);
    }
    return pvd::getFieldCreate()->createStructure(names, fields);
}

pvd::UnionConstPtr createUnionFromDict(const bp::dict& pyDict, const std::string& structureId)
{
    pvd::FieldConstPtrArray fields;
    pvd::StringArray names;
    updateFieldArrayFromDict(pyDict, fields, names);

    std::string structureName = StringUtility::trim(structureId);
    if (structureName.length()) {
        return pvd::getFieldCreate()->createUnion(structureName, names, fields);
    }
    return pvd::getFieldCreate()->createUnion(names, fields);
}

void updateFieldArrayFromDict(const bp::dict& pyDict, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    bp::list fieldNames = pyDict.keys();
    for (int i = 0; i < bp::len(fieldNames); i++) {
        bp::object fieldNameObject = fieldNames[i];
        bp::extract<std::string> fieldNameExtract(fieldNameObject);
        std::string fieldName;
        if (fieldNameExtract.check()) {
            fieldName = fieldNameExtract();
        }
        else {
            throw InvalidArgument("Dictionary key is used as field name and must be a string");
        }

        // Check for Scalar
        bp::object valueObject = pyDict[fieldNameObject];
        if (updateFieldArrayFromInt(valueObject, fieldName, fields, names)) {
            continue;
        }

        // Check for list: []
        // Type of the first element in the list will determine PV list type
        bp::extract<bp::list> listExtract(valueObject);
        if (listExtract.check()) {
            bp::list pyList = listExtract();
            int listSize = bp::len(pyList);
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

pvd::PVStructurePtr createUnionPvStructure(const pvd::PVUnionPtr& pvUnionPtr, const std::string& fieldName)
{
    pvd::PVFieldPtr pvField = pvUnionPtr->get();
    pvd::StringArray names(1);
    pvd::PVFieldPtrArray pvfields(1);
    names[0] = PvaConstants::ValueFieldKey;
    if (!pvUnionPtr->getUnion()->isVariant()) {
        std::string unionFieldName = pvUnionPtr->getSelectedFieldName();
        if (unionFieldName != "") {
            names[0] = unionFieldName;
        }
    }
    pvfields[0] = pvField;
    pvd::PVStructurePtr pv = pvd::getPVDataCreate()->createPVStructure(names, pvfields);
    return pv;
}

pvd::PVStructurePtr createUnionFieldPvStructure(pvd::UnionConstPtr unionPtr, const std::string& fieldName)
{
    pvd::StringArray names(1);
    pvd::PVFieldPtrArray pvfields(1);
    names[0] = fieldName;
    pvd::FieldConstPtr field;
    if (unionPtr->isVariant()) {
        field = pvd::getFieldCreate()->createVariantUnion();
    } 
    else {
        field = unionPtr->getField(fieldName);
    }
    pvfields[0] = pvd::getPVDataCreate()->createPVField(field);
    return pvd::getPVDataCreate()->createPVStructure(names,pvfields);
}

void addScalarField(const std::string& fieldName, pvd::ScalarType scalarType, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    fields.push_back(pvd::getFieldCreate()->createScalar(scalarType));
    names.push_back(fieldName);
}

void addScalarArrayField(const std::string& fieldName, pvd::ScalarType scalarType, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    fields.push_back(pvd::getFieldCreate()->createScalarArray(scalarType));
    names.push_back(fieldName);
}

void addStructureField(const std::string& fieldName, const bp::dict& pyDict, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    fields.push_back(createStructureFromDict(pyDict));
    names.push_back(fieldName);
}

void addStructureField(const std::string& fieldName, const PvObject & pvObject, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    fields.push_back(pvObject.getPvStructurePtr()->getStructure());
    names.push_back(fieldName);   
}

void addStructureArrayField(const std::string& fieldName, const bp::dict& pyDict, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    fields.push_back(pvd::getFieldCreate()->createStructureArray(createStructureFromDict(pyDict)));
    names.push_back(fieldName);
}

void addStructureArrayField(const std::string& fieldName, const PvObject & pvObject, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    fields.push_back(pvd::getFieldCreate()->createStructureArray(pvObject.getPvStructurePtr()->getStructure()));
    names.push_back(fieldName);   
}

void addUnionField(const std::string& fieldName, const bp::dict& pyDict, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    fields.push_back(createUnionFromDict(pyDict));
    names.push_back(fieldName);
}

void addUnionArrayField(const std::string& fieldName, const bp::dict& pyDict, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    fields.push_back(pvd::getFieldCreate()->createUnionArray(createUnionFromDict(pyDict)));
    names.push_back(fieldName);
}

void addVariantUnionField(const std::string& fieldName, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    fields.push_back(pvd::getFieldCreate()->createVariantUnion());
    names.push_back(fieldName);
}

void addVariantUnionArrayField(const std::string& fieldName, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    fields.push_back(pvd::getFieldCreate()->createUnionArray(pvd::getFieldCreate()->createVariantUnion()));
    names.push_back(fieldName);
}

bool updateFieldArrayFromInt(const bp::object& pyObject, const std::string& fieldName, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    // *******************************************************
    // bp::extract<int>(pyDict) results in segmentation fault
    // with numpy>=1.21.0 
    // for now, determine if we have scalar type using class name
    std::string pyClassName = bp::extract<std::string>(pyObject.attr("__class__").attr("__name__"));
    if (pyClassName != PVAPY_SCALAR_TYPE_CLASS_NAME) {
        return false;
    }

    bp::extract<int> scalarExtract(pyObject);
    //if (!scalarExtract.check()) {
    //    return false;
    //}
    // *******************************************************

    int scalarExtractValue = scalarExtract();
    pvd::ScalarType scalarType = static_cast<pvd::ScalarType>(scalarExtractValue);
    addScalarField(fieldName, scalarType, fields, names);
    return true;
}

bool updateFieldArrayFromIntList(const bp::object& pyObject, const std::string& fieldName, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    // *******************************************************
    // bp::extract<int>(pyDict) results in segmentation fault
    // with numpy>=1.21.0 
    // for now, determine if we have scalar type using class name
    std::string pyClassName = bp::extract<std::string>(pyObject.attr("__class__").attr("__name__"));
    if (pyClassName != PVAPY_SCALAR_TYPE_CLASS_NAME) {
        return false;
    }

    bp::extract<int> arrayScalarExtract(pyObject);
    //if (!arrayScalarExtract.check()) {
    //    return false;
    //}
    // *******************************************************

    int arrayScalarExtractValue = arrayScalarExtract();
    pvd::ScalarType scalarType = static_cast<pvd::ScalarType>(arrayScalarExtractValue);
    addScalarArrayField(fieldName, scalarType, fields, names);
    return true;
}

bool updateFieldArrayFromDict(const bp::object& pyObject, const std::string& fieldName, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    bp::extract<bp::dict> dictExtract(pyObject);
    if (!dictExtract.check()) {
        return false;
    }

    bp::dict pyDict2 = dictExtract();
    int dictSize = bp::len(pyDict2);
    if (!dictSize) {
        throw InvalidArgument("PV type dict provided for field name %s must be non-empty.", fieldName.c_str());
    }
    addStructureField(fieldName, pyDict2, fields, names);
    return true;
}

bool updateFieldArrayFromDictList(const bp::object& pyObject, const std::string& fieldName, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    bp::extract<bp::dict> dictExtract(pyObject);
    if (!dictExtract.check()) {
        return false;
    }

    bp::dict pyDict2 = dictExtract();
    int dictSize = bp::len(pyDict2);
    if (!dictSize) {
        throw InvalidArgument("PV type dict provided for field name %s must be non-empty.", fieldName.c_str());
    }
    addStructureArrayField(fieldName, pyDict2, fields, names);
    return true;
}

bool updateFieldArrayFromTuple(const bp::object& pyObject, const std::string& fieldName, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    bp::extract<bp::tuple> tupleExtract(pyObject);
    if (!tupleExtract.check()) {
        return false;
    }

    bp::tuple pyTuple = tupleExtract();
    int tupleSize = bp::len(pyTuple);
    switch (tupleSize) {
        case (0): {
            addVariantUnionField(fieldName, fields, names);
            break;
        }
        case (1): {
            bp::extract<bp::dict> dictExtract(pyTuple[0]);
            if (dictExtract.check()) {
                bp::dict pyDict2 = dictExtract();
                int dictSize = bp::len(pyDict2);
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

bool updateFieldArrayFromTupleList(const bp::object& pyObject, const std::string& fieldName, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    bp::extract<bp::tuple> tupleExtract(pyObject);
    if (!tupleExtract.check()) {
        return false;
    }

    bp::tuple pyTuple = tupleExtract();
    int tupleSize = bp::len(pyTuple);
    switch (tupleSize) {
        case (0): {
            addVariantUnionArrayField(fieldName, fields, names);
            break;
        }
        case (1): {
            bp::extract<bp::dict> dictExtract(pyTuple[0]);
            if (dictExtract.check()) {
                bp::dict pyDict2 = dictExtract();
                int dictSize = bp::len(pyDict2);
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

bool updateFieldArrayFromPvObject(const bp::object& pyObject, const std::string& fieldName, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    bp::extract<PvObject> pvObjectExtract(pyObject);
    if (!pvObjectExtract.check()) {
        return false;
    }

    PvObject pvObject = pvObjectExtract();
    bp::dict pyDict2 = pvObject.getStructureDict();
    int dictSize = bp::len(pyDict2);
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

bool updateFieldArrayFromPvObjectList(const bp::object& pyObject, const std::string& fieldName, pvd::FieldConstPtrArray& fields, pvd::StringArray& names)
{
    bp::extract<PvObject> pvObjectExtract(pyObject);
    if (!pvObjectExtract.check()) {
        return false;
    }

    PvObject pvObject = pvObjectExtract();
    bp::dict pyDict2 = pvObject.getStructureDict();
    int dictSize = bp::len(pyDict2);
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
bp::dict extractUnionStructureDict(const bp::dict& pyDict)
{
    // Do not duplicate value key, if one is provided, if it is a single
    // dictionary key, and if it holds dictionary or tuple holding dictionary
    bp::dict pyDict2;
    if (pyDict.has_key(PvaConstants::ValueFieldKey) && bp::len(pyDict) == 1) {
        bp::object pyObject = pyDict[PvaConstants::ValueFieldKey];
        bp::extract<bp::tuple> tupleExtract(pyObject);

        // Look for dict inside tuple
        if (tupleExtract.check()) {
            bp::tuple pyTuple = tupleExtract();
            if (bp::len(pyTuple) == 1) {
                bp::extract<bp::dict> dictExtract(pyTuple[0]);
                if (dictExtract.check()) {
                    return dictExtract();
                }
            }
        }
        
        // Look for dict
        bp::extract<bp::dict> dictExtract(pyObject);
        if (dictExtract.check()) {
            return dictExtract();
        }
    }

    // We could not find union in the structure, simply use provided dict
    return pyDict;
}

//
// Check if field path is a scalar array of chars.
// Allow notation like 'x.y.z' for the field path.
// 
bool isFieldPathCharScalarArray(const std::string& fieldPath, const pvd::PVStructurePtr& pvStructurePtr) 
{
    pvd::ScalarArrayConstPtr scalarArrayPtr = getFieldPathAsScalarArray(fieldPath, pvStructurePtr);
    bool isCharArray = false;
    if (scalarArrayPtr) {
        pvd::ScalarType scalarType = scalarArrayPtr->getElementType();
        if (scalarType == pvd::pvByte || scalarType == pvd::pvUByte) {
            isCharArray = true;
        }
    }
    return isCharArray;
}

//
// Get field path as a scalar array.
// Allow notation like 'x.y.z' for the field path.
// 
pvd::ScalarArrayConstPtr getFieldPathAsScalarArray(const std::string& fieldPath, const pvd::PVStructurePtr& pvStructurePtr) 
{
    std::vector<std::string> fieldNames = StringUtility::split(fieldPath);
    pvd::PVStructurePtr pvStructurePtr2 = getParentStructureForFieldPath(fieldNames, pvStructurePtr);

    // Last field in the path is what we want.
    int nElements = fieldNames.size();
    std::string fieldName = fieldNames[nElements-1];
    pvd::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr2);
    pvd::Type type = fieldPtr->getType();
    pvd::ScalarArrayConstPtr scalarArrayPtr;
    if (type == pvd::scalarArray) {
        scalarArrayPtr = std::tr1::static_pointer_cast<const pvd::ScalarArray>(fieldPtr);
    }
    return scalarArrayPtr;
}

//
// Return structure field as python object. Allow notation like 'x.y.z'
// for the field path.
//
bp::object getFieldPathAsPyObject(const std::string& fieldPath, const pvd::PVStructurePtr& pvStructurePtr, bool useNumPyArrays)
{
    std::vector<std::string> fieldNames = StringUtility::split(fieldPath);
    pvd::PVStructurePtr pvStructurePtr2 = getParentStructureForFieldPath(fieldNames, pvStructurePtr);

    // Last field in the path is what we want.
    int nElements = fieldNames.size();
    std::string fieldName = fieldNames[nElements-1];
    pvd::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr2);
    pvd::Type type = fieldPtr->getType();
    switch (type) {
        case pvd::scalar: {
            return getScalarFieldAsPyObject(fieldName, pvStructurePtr2);
        }
        case pvd::scalarArray: {
            return getScalarArrayFieldAsPyObject(fieldName, pvStructurePtr2, useNumPyArrays);
        }
        case pvd::structure: {
            return getStructureFieldAsPyObject(fieldName, pvStructurePtr2, useNumPyArrays);
        }
        case pvd::structureArray: {
            return getStructureArrayFieldAsPyObject(fieldName, pvStructurePtr2, useNumPyArrays);
        }
        case pvd::union_: {
            return getUnionFieldAsPyObject(fieldName, pvStructurePtr2, useNumPyArrays);
        }
        case pvd::unionArray: {
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
void setPyObjectToFieldPath(const bp::object& pyObject, const std::string& fieldPath, const pvd::PVStructurePtr& pvStructurePtr)
{
    std::vector<std::string> fieldNames = StringUtility::split(fieldPath);
    pvd::PVStructurePtr pvStructurePtr2 = getParentStructureForFieldPath(fieldNames, pvStructurePtr);
    int nElements = fieldNames.size();
    
    // Last field in the path is what we want.
    std::string fieldName = fieldNames[nElements-1];
    pyObjectToField(pyObject, fieldName, pvStructurePtr2);
}

#if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1

//
// Conversion PV Scalar Array => NumPy Array
//
np::ndarray getScalarArrayFieldAsNumPyArray(const std::string& fieldName, const pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::ScalarType scalarType = getScalarArrayType(fieldName, pvStructurePtr);
    pvd::PVScalarArrayPtr pvScalarArrayPtr = pvStructurePtr->getSubField<pvd::PVScalarArray>(fieldName);

    switch (scalarType) {
        case pvd::pvBoolean: {
            return getScalarArrayAsNumPyArray<pvd::PVBooleanArray, pvd::boolean>(pvScalarArrayPtr);
        }
        case pvd::pvByte: {
            return getScalarArrayAsNumPyArray<pvd::PVByteArray, int8_t>(pvScalarArrayPtr);
        }
        case pvd::pvUByte: {
            return getScalarArrayAsNumPyArray<pvd::PVUByteArray, uint8_t>(pvScalarArrayPtr);
        }
        case pvd::pvShort: {
            return getScalarArrayAsNumPyArray<pvd::PVShortArray, int16_t>(pvScalarArrayPtr);
        }
        case pvd::pvUShort: {
            return getScalarArrayAsNumPyArray<pvd::PVUShortArray, uint16_t>(pvScalarArrayPtr);
        }
        case pvd::pvInt: {
            return getScalarArrayAsNumPyArray<pvd::PVIntArray, int32_t>(pvScalarArrayPtr);
        }
        case pvd::pvUInt: {
            return getScalarArrayAsNumPyArray<pvd::PVUIntArray, uint32_t>(pvScalarArrayPtr);
        }
        case pvd::pvLong: {
            return getScalarArrayAsNumPyArray<pvd::PVLongArray, int64_t>(pvScalarArrayPtr);
        }
        case pvd::pvULong: {
            return getScalarArrayAsNumPyArray<pvd::PVULongArray, uint64_t>(pvScalarArrayPtr);
        }
        case pvd::pvFloat: {
            return getScalarArrayAsNumPyArray<pvd::PVFloatArray, float>(pvScalarArrayPtr);
        }
        case pvd::pvDouble: {
            return getScalarArrayAsNumPyArray<pvd::PVDoubleArray, double>(pvScalarArrayPtr);
        }
        default: {
            throw PvaException("Unrecognized scalar type: %d", scalarType);
        }
    }
}

//
// Conversion NumPy Array => PV Scalar Array 
//
void setScalarArrayFieldFromNumPyArray(const np::ndarray& ndArray, const std::string& fieldName, pvd::PVStructurePtr& pvStructurePtr)
{
    pvd::ScalarType scalarType = getScalarArrayType(fieldName, pvStructurePtr);
    switch (scalarType) {
        case pvd::pvBoolean: {
            setScalarArrayFieldFromNumPyArrayImpl<pvd::boolean, bool>(ndArray, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvByte: {
            setScalarArrayFieldFromNumPyArrayImpl<pvd::int8, boost::int8_t>(ndArray, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvUByte: {
            setScalarArrayFieldFromNumPyArrayImpl<pvd::uint8, boost::uint8_t>(ndArray, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvShort: {
            setScalarArrayFieldFromNumPyArrayImpl<pvd::int16, boost::int16_t>(ndArray, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvUShort: {
            setScalarArrayFieldFromNumPyArrayImpl<pvd::uint16, boost::uint16_t>(ndArray, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvInt: {
            setScalarArrayFieldFromNumPyArrayImpl<pvd::int32, boost::int32_t>(ndArray, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvUInt: {
            setScalarArrayFieldFromNumPyArrayImpl<pvd::uint32, boost::uint32_t>(ndArray, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvLong: {
            setScalarArrayFieldFromNumPyArrayImpl<pvd::int64, boost::int64_t>(ndArray, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvULong: {
            setScalarArrayFieldFromNumPyArrayImpl<pvd::uint64, boost::uint64_t>(ndArray, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvFloat: {
            setScalarArrayFieldFromNumPyArrayImpl<float, float>(ndArray, fieldName, pvStructurePtr);
            break;
        }
        case pvd::pvDouble: {
            setScalarArrayFieldFromNumPyArrayImpl<double, double>(ndArray, fieldName, pvStructurePtr);
            break;
        }
        default: {
            throw PvaException("Unrecognized scalar type: %d", scalarType);
        }
    }
}

#endif // if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1

} // namespace PyPvDataUtility


