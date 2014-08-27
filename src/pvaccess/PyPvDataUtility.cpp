#include "PyPvDataUtility.h"
#include "PvType.h"
#include "FieldNotFound.h"
#include "InvalidDataType.h"

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

//
// Field retrieval
//
epics::pvData::FieldConstPtr getField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::PVFieldPtr pvFieldPtr = pvStructurePtr->getSubField(fieldName);
    if (!pvFieldPtr) {
        throw FieldNotFound("Object does not have field " + fieldName);
    }
    return pvFieldPtr->getField();
}

epics::pvData::ScalarConstPtr getScalarField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr);
    epics::pvData::ScalarConstPtr scalarPtr = std::tr1::static_pointer_cast<const epics::pvData::Scalar>(fieldPtr);
    if (!scalarPtr) {
        throw InvalidDataType("Field " + fieldName + " is not a scalar");
    }
    return scalarPtr;
}

epics::pvData::PVScalarArrayPtr getScalarArrayField(const std::string& fieldName, epics::pvData::ScalarType scalarType, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::PVScalarArrayPtr pvScalarArrayPtr = pvStructurePtr->getScalarArrayField(fieldName, scalarType);
    if (!pvScalarArrayPtr) {
        throw FieldNotFound("Object does not have scalar array field %s of type %d", fieldName.c_str(), scalarType);
    }
    return pvScalarArrayPtr; 
}

epics::pvData::StructureConstPtr getStructure(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::FieldConstPtr fieldPtr = getField(fieldName, pvStructurePtr);
    epics::pvData::StructureConstPtr structurePtr = std::tr1::static_pointer_cast<const epics::pvData::Structure>(fieldPtr);
    if (!structurePtr) {
        throw InvalidDataType("Field " + fieldName + " is not a structure");
    }
    return structurePtr;
}

epics::pvData::PVStructurePtr getStructureField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::PVStructurePtr pvStructurePtr2 = pvStructurePtr->getStructureField(fieldName);
    if (!pvStructurePtr2) {
        throw FieldNotFound("Object does not have structure field " + fieldName);
    }
    return pvStructurePtr2;
}

epics::pvData::PVStructureArrayPtr getStructureArrayField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::PVStructureArrayPtr pvStructureArrayPtr = pvStructurePtr->getStructureArrayField(fieldName);
    if (!pvStructureArrayPtr) {
        throw FieldNotFound("Object does not have structure array field " + fieldName);
    }
    return pvStructureArrayPtr;
}

epics::pvData::PVBooleanPtr getBooleanField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    epics::pvData::PVBooleanPtr fieldPtr = pvStructurePtr->getBooleanField(fieldName);
    if (!fieldPtr) {
        throw FieldNotFound("Object does not have boolean field " + fieldName);
    }
    return fieldPtr;
}

epics::pvData::PVBytePtr getByteField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    epics::pvData::PVBytePtr fieldPtr = pvStructurePtr->getByteField(fieldName);
    if (!fieldPtr) {
        throw FieldNotFound("Object does not have byte field " + fieldName);
    }
    return fieldPtr;
}

epics::pvData::PVUBytePtr getUByteField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    epics::pvData::PVUBytePtr fieldPtr = pvStructurePtr->getUByteField(fieldName);
    if (!fieldPtr) {
        throw FieldNotFound("Object does not have unsigned byte field " + fieldName);
    }
    return fieldPtr;
}

epics::pvData::PVShortPtr getShortField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    epics::pvData::PVShortPtr fieldPtr = pvStructurePtr->getShortField(fieldName);
    if (!fieldPtr) {
        throw FieldNotFound("Object does not have short field " + fieldName);
    }
    return fieldPtr;
}

epics::pvData::PVUShortPtr getUShortField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    epics::pvData::PVUShortPtr fieldPtr = pvStructurePtr->getUShortField(fieldName);
    if (!fieldPtr) {
        throw FieldNotFound("Object does not have unsigned short field " + fieldName);
    }
    return fieldPtr;
}

epics::pvData::PVIntPtr getIntField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    epics::pvData::PVIntPtr fieldPtr = pvStructurePtr->getIntField(fieldName);
    if (!fieldPtr) {
        throw FieldNotFound("Object does not have int field " + fieldName);
    }
    return fieldPtr;
}

epics::pvData::PVUIntPtr getUIntField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    epics::pvData::PVUIntPtr fieldPtr = pvStructurePtr->getUIntField(fieldName);
    if (!fieldPtr) {
        throw FieldNotFound("Object does not have unsigned int field " + fieldName);
    }
    return fieldPtr;
}

epics::pvData::PVLongPtr getLongField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    epics::pvData::PVLongPtr fieldPtr = pvStructurePtr->getLongField(fieldName);
    if (!fieldPtr) {
        throw FieldNotFound("Object does not have long field " + fieldName);
    }
    return fieldPtr;
}

epics::pvData::PVULongPtr getULongField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    epics::pvData::PVULongPtr fieldPtr = pvStructurePtr->getULongField(fieldName);
    if (!fieldPtr) {
        throw FieldNotFound("Object does not have unsigned long field " + fieldName);
    }
    return fieldPtr;
}

epics::pvData::PVFloatPtr getFloatField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    epics::pvData::PVFloatPtr fieldPtr = pvStructurePtr->getFloatField(fieldName);
    if (!fieldPtr) {
        throw FieldNotFound("Object does not have float field " + fieldName);
    }
    return fieldPtr;
}

epics::pvData::PVDoublePtr getDoubleField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    epics::pvData::PVDoublePtr fieldPtr = pvStructurePtr->getDoubleField(fieldName);
    if (!fieldPtr) {
        throw FieldNotFound("Object does not have double field " + fieldName);
    }
    return fieldPtr;
}

epics::pvData::PVStringPtr getStringField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    epics::pvData::PVStringPtr fieldPtr = pvStructurePtr->getStringField(fieldName);
    if (!fieldPtr) {
        throw FieldNotFound("Object does not have string field " + fieldName);
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
        throw InvalidDataType("Object does not have scalar array field " + fieldName);
    }
    epics::pvData::ScalarArrayConstPtr scalarArrayPtr = std::tr1::static_pointer_cast<const epics::pvData::ScalarArray>(fieldPtr);
    epics::pvData::ScalarType scalarType = scalarArrayPtr->getElementType();
    return scalarType;
}

//
// Conversion PY object => PV Scalar
//
void pyObjectToScalarField(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::ScalarType scalarType = getScalarType(fieldName, pvStructurePtr);
    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            epics::pvData::PVBooleanPtr fieldPtr = pvStructurePtr->getBooleanField(fieldName);
            bool value = PyUtility::extractValueFromPyObject<bool>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::boolean>(value));
            break;
        }
        case epics::pvData::pvByte: {
            epics::pvData::PVBytePtr fieldPtr = pvStructurePtr->getByteField(fieldName);
            char value = PyUtility::extractValueFromPyObject<char>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::int8>(value));
            break;
        }
        case epics::pvData::pvUByte: {
            epics::pvData::PVUBytePtr fieldPtr = pvStructurePtr->getUByteField(fieldName);
            unsigned char value = PyUtility::extractValueFromPyObject<unsigned char>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::uint8>(value));
            break;
        }
        case epics::pvData::pvShort: {
            epics::pvData::PVShortPtr fieldPtr = pvStructurePtr->getShortField(fieldName);
            short value = PyUtility::extractValueFromPyObject<short>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::int16>(value));
            break;
        }
        case epics::pvData::pvUShort: {
            epics::pvData::PVUShortPtr fieldPtr = pvStructurePtr->getUShortField(fieldName);
            unsigned short value = PyUtility::extractValueFromPyObject<unsigned short>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::uint16>(value));
            break;
        }
        case epics::pvData::pvInt: {
            epics::pvData::PVIntPtr fieldPtr = pvStructurePtr->getIntField(fieldName);
            int value = PyUtility::extractValueFromPyObject<int>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::int32>(value));
            break;
        }
        case epics::pvData::pvUInt: {
            epics::pvData::PVUIntPtr fieldPtr = pvStructurePtr->getUIntField(fieldName);
            unsigned int value = PyUtility::extractValueFromPyObject<unsigned int>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::uint32>(value));
            break;
        }
        case epics::pvData::pvLong: {
            epics::pvData::PVLongPtr fieldPtr = pvStructurePtr->getLongField(fieldName);
            long long value = PyUtility::extractValueFromPyObject<long long>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::int64>(value));
            break;
        }
        case epics::pvData::pvULong: {
            epics::pvData::PVULongPtr fieldPtr = pvStructurePtr->getULongField(fieldName);
            unsigned long long value = PyUtility::extractValueFromPyObject<unsigned long long>(pyObject);
            fieldPtr->put(static_cast<epics::pvData::uint64>(value));
            break;
        }
        case epics::pvData::pvFloat: {
            epics::pvData::PVFloatPtr fieldPtr = pvStructurePtr->getFloatField(fieldName);
            float value = PyUtility::extractValueFromPyObject<float>(pyObject);
            fieldPtr->put(value);
            break;
        }
        case epics::pvData::pvDouble: {
            epics::pvData::PVDoublePtr fieldPtr = pvStructurePtr->getDoubleField(fieldName);
            double value = PyUtility::extractValueFromPyObject<double>(pyObject);
            fieldPtr->put(value);
            break;
        }
        case epics::pvData::pvString: {
            epics::pvData::PVStringPtr fieldPtr = pvStructurePtr->getStringField(fieldName);
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
// Conversion PV Scalar Array => PY List
//
void scalarArrayFieldToPyList(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::list& pyList)
{
    epics::pvData::ScalarType scalarType = getScalarArrayType(fieldName, pvStructurePtr);
    epics::pvData::PVScalarArrayPtr pvScalarArrayPtr = pvStructurePtr->getScalarArrayField(fieldName, scalarType);
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            scalarArrayToPyList<epics::pvData::PVBooleanArray, epics::pvData::BooleanArrayData>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvByte: {
            scalarArrayToPyList<epics::pvData::PVByteArray, epics::pvData::ByteArrayData>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvUByte: {
            scalarArrayToPyList<epics::pvData::PVUByteArray, epics::pvData::UByteArrayData>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvShort: {
            scalarArrayToPyList<epics::pvData::PVShortArray, epics::pvData::ShortArrayData>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvUShort: {
            scalarArrayToPyList<epics::pvData::PVUShortArray, epics::pvData::UShortArrayData>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvInt: {
            scalarArrayToPyList<epics::pvData::PVIntArray, epics::pvData::IntArrayData>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvUInt: {
            scalarArrayToPyList<epics::pvData::PVUIntArray, epics::pvData::UIntArrayData>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvLong: {
            scalarArrayToPyList<epics::pvData::PVLongArray, epics::pvData::LongArrayData>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvULong: {
            scalarArrayToPyList<epics::pvData::PVULongArray, epics::pvData::ULongArrayData>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvFloat: {
            scalarArrayToPyList<epics::pvData::PVFloatArray, epics::pvData::FloatArrayData>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvDouble: {
            scalarArrayToPyList<epics::pvData::PVDoubleArray, epics::pvData::DoubleArrayData>(pvScalarArrayPtr, pyList);
            break;
        }
        case epics::pvData::pvString: {
            scalarArrayToPyList<epics::pvData::PVStringArray, epics::pvData::StringArrayData>(pvScalarArrayPtr, pyList);
            break;
        }
        default: {
            throw PvaException("Unrecognized scalar type: %d", scalarType);
        }
    }
#else
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
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
}

//
// Conversion PY List => PV Scalar Array
//
void pyListToScalarArrayField(const boost::python::list& pyList, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr) 
{
    epics::pvData::ScalarType scalarType = getScalarArrayType(fieldName, pvStructurePtr);
    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            pyListToScalarArrayField<epics::pvData::PVBooleanArray, epics::pvData::boolean, bool>(pyList, fieldName, scalarType, pvStructurePtr);
            break;
        }
        case epics::pvData::pvByte: {
            pyListToScalarArrayField<epics::pvData::PVByteArray, epics::pvData::int8, char>(pyList, fieldName, scalarType, pvStructurePtr);
            break;
        }
        case epics::pvData::pvUByte: {
            pyListToScalarArrayField<epics::pvData::PVUByteArray, epics::pvData::uint8, unsigned char>(pyList, fieldName, scalarType, pvStructurePtr);
            break;
        }
        case epics::pvData::pvShort: {
            pyListToScalarArrayField<epics::pvData::PVShortArray, epics::pvData::int16, short>(pyList, fieldName, scalarType, pvStructurePtr);
            break;
        }
        case epics::pvData::pvUShort: {
            pyListToScalarArrayField<epics::pvData::PVUShortArray, epics::pvData::uint16, unsigned short>(pyList, fieldName, scalarType, pvStructurePtr);
            break;
        }
        case epics::pvData::pvInt: {
            pyListToScalarArrayField<epics::pvData::PVIntArray, epics::pvData::int32, int>(pyList, fieldName, scalarType, pvStructurePtr);
            break;
        }
        case epics::pvData::pvUInt: {
            pyListToScalarArrayField<epics::pvData::PVUIntArray, epics::pvData::uint32, unsigned int>(pyList, fieldName, scalarType, pvStructurePtr);
            break;
        }
        case epics::pvData::pvLong: {
            pyListToScalarArrayField<epics::pvData::PVLongArray, epics::pvData::int64, long long>(pyList, fieldName, scalarType, pvStructurePtr);
            break;
        }
        case epics::pvData::pvULong: {
            pyListToScalarArrayField<epics::pvData::PVULongArray, epics::pvData::uint64, unsigned long long>(pyList, fieldName, scalarType, pvStructurePtr);
            break;
        }
        case epics::pvData::pvFloat: {
            pyListToScalarArrayField<epics::pvData::PVFloatArray, float, float>(pyList, fieldName, scalarType, pvStructurePtr);
            break;
        }
        case epics::pvData::pvDouble: {
            pyListToScalarArrayField<epics::pvData::PVDoubleArray, double, double>(pyList, fieldName, scalarType, pvStructurePtr);
            break;
        }
        case epics::pvData::pvString: {
            pyListToScalarArrayField<epics::pvData::PVStringArray, std::string, std::string>(pyList, fieldName, scalarType, pvStructurePtr);
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

        // Got key/field name, find it in pv structure
        epics::pvData::FieldConstPtr fieldPtr = getField(key, pvStructurePtr);
        epics::pvData::Type type = fieldPtr->getType();
        switch (type) {
            case epics::pvData::scalar: {
                pyObjectToScalarField(pyDict[keyObject], key, pvStructurePtr);
                break;
            }
            case epics::pvData::structure: {
                pyObjectToStructureField(pyDict[keyObject], key, pvStructurePtr);
                break;
            }
            case epics::pvData::scalarArray: {
                pyObjectToScalarArrayField(pyDict[keyObject], key, pvStructurePtr);
                break;
            }
            case epics::pvData::structureArray: {
                pyObjectToStructureArrayField(pyDict[keyObject], key, pvStructurePtr);
                break;
            }
            default: {
                throw PvaException("Unrecognized field type: %d", type);
            }
        } 
    }    
}

void pyDictToStructureField(const boost::python::dict& pyDict, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::PVStructurePtr pvStructurePtr2 = getStructureField(fieldName, pvStructurePtr);
    pyDictToStructure(pyDict, pvStructurePtr2);
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
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
    epics::pvData::PVStructurePtrArray pvStructures(listSize);
#else
    epics::pvData::PVStructureArray::svector pvStructures(listSize);
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
    for (int i = 0; i < listSize; i++) {
        boost::python::extract<boost::python::dict> dictExtract(pyList[i]);
        if (dictExtract.check()) {
            boost::python::dict pyDict = dictExtract();
            epics::pvData::PVStructurePtr pvStructure = epics::pvData::getPVDataCreate()->createPVStructure(structurePtr);
            pyDictToStructure(pyDict, pvStructure);
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
            pvStructures[i] = pvStructure;
#else
            pvStructures[i] = pvStructure;
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
        }
        else {
            throw InvalidDataType("Invalid data type for element %d", i);
        }
    }
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
    pvStructureArrayPtr->put(0, listSize, pvStructures, 0);
#else
    pvStructureArrayPtr->setCapacity(listSize);
    pvStructureArrayPtr->replace(freeze(pvStructures));
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
}

//
// Conversion PV Structure Array => PY [{}] 
//
void structureArrayFieldToPyList(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::list& pyList)
{
    epics::pvData::PVStructureArrayPtr pvStructureArrayPtr = getStructureArrayField(fieldName, pvStructurePtr);
    int nDataElements = pvStructureArrayPtr->getLength();
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
    epics::pvData::StructureArrayData arrayData;
    pvStructureArrayPtr->get(0, nDataElements, arrayData);
#else
    epics::pvData::PVStructureArray::const_svector arrayData(pvStructureArrayPtr->view());
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
    for (int i = 0; i < nDataElements; ++i) {
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
        epics::pvData::PVStructurePtr pvStructure = arrayData.data[i];
#else
        epics::pvData::PVStructurePtr pvStructure = arrayData[i];
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
        boost::python::dict pyDict;
        structureToPyDict(pvStructure, pyDict);
        pyList.append(pyDict);
    }
}

//
// Conversion PV Structure => PY {}
//
void structureToPyDict(const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict)
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
                addScalarArrayFieldToDict(fieldName, scalarType, pvStructurePtr, pyDict);
                break;
            }
            case epics::pvData::structure: {
                addStructureFieldToDict(fieldName, pvStructurePtr, pyDict);
                break;
            }
            case epics::pvData::structureArray: {
                addStructureArrayFieldToDict(fieldName, pvStructurePtr, pyDict);
                break;
            }
            default: {
                throw PvaException("Unrecognized field type: %d", type);
            }
        }
    }
}

void structureFieldToPyDict(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict)
{
    structureToPyDict(getStructureField(fieldName, pvStructurePtr), pyDict);
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

//
// Add PV Scalar Array => PY {}
// 
void addScalarArrayFieldToDict(const std::string& fieldName, epics::pvData::ScalarType scalarType, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict)
{
    boost::python::list pyList;
    scalarArrayFieldToPyList(fieldName, pvStructurePtr, pyList);
    pyDict[fieldName] = pyList;
}

//
// Add PV Structure => PY {}
// 
void addStructureFieldToDict(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict)
{
    boost::python::dict pyDict2;
    structureFieldToPyDict(fieldName, pvStructurePtr, pyDict2);
    pyDict[fieldName] = pyDict2;
}

//
// Add PV Structure Array => PY {}
// 
void addStructureArrayFieldToDict(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict) 
{
    boost::python::list pyList;
    epics::pvData::PVStructureArrayPtr structureArrayPtr = getStructureArrayField(fieldName, pvStructurePtr);
    int nDataElements = structureArrayPtr->getLength();
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
    epics::pvData::StructureArrayData arrayData;
    structureArrayPtr->get(0, nDataElements, arrayData);
#else
    epics::pvData::PVStructureArray::const_svector arrayData(structureArrayPtr->view());
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
    for (int i = 0; i < nDataElements; ++i) {
        boost::python::dict pyDict2;
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
        structureToPyDict(arrayData.data[i], pyDict2);   
#else
        structureToPyDict(arrayData[i], pyDict2);   
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
        pyList.append(pyDict2);   
    }
    pyDict[fieldName] = pyList;
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
            default: {
                throw PvaException("Unrecognized field type: %d", type);
            }
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
            default: {
                throw PvaException("Unrecognized field type: %d", type);
            }
        }
    }
}

void copyStructureToStructure(const std::string& fieldName, const epics::pvData::PVStructurePtr& srcPvStructurePtr, epics::pvData::PVStructurePtr& destPvStructurePtr)
{
    epics::pvData::PVStructurePtr destPvStructurePtr2 = destPvStructurePtr->getStructureField(fieldName);
    if (destPvStructurePtr2) {
        epics::pvData::PVStructurePtr srcPvStructurePtr2 = srcPvStructurePtr->getStructureField(fieldName);
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
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
    epics::pvData::StructureArrayData srcPvStructures;
    srcPvStructureArrayPtr->get(0, nElements, srcPvStructures);
    epics::pvData::PVStructurePtrArray destPvStructures(nElements);
#else
    epics::pvData::PVStructureArray::const_svector srcPvStructures(srcPvStructureArrayPtr->view());
    epics::pvData::PVStructureArray::svector destPvStructures(nElements);
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430

    for (int i = 0; i < nElements; i++) {
        epics::pvData::PVStructurePtr destPvStructurePtr2 = epics::pvData::getPVDataCreate()->createPVStructure(structurePtr);
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
        epics::pvData::PVStructurePtr srcPvStructurePtr2 = srcPvStructures.data[i];
#else
        epics::pvData::PVStructurePtr srcPvStructurePtr2 = srcPvStructures[i];
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
        copyStructureToStructure(srcPvStructurePtr2, destPvStructurePtr2);
        destPvStructures[i] = destPvStructurePtr2;
    }
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
    destPvStructureArrayPtr->put(0, nElements, destPvStructures, 0);
#else
    destPvStructureArrayPtr->setCapacity(nElements);
    destPvStructureArrayPtr->replace(freeze(destPvStructures));
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
}

//
// Copy PV Scalar => PV Structure
//
void copyScalarToStructure(const std::string& fieldName, epics::pvData::ScalarType scalarType, const epics::pvData::PVStructurePtr& srcPvStructurePtr, epics::pvData::PVStructurePtr& destPvStructurePtr)
{
    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            epics::pvData::PVBooleanPtr fieldPtr = destPvStructurePtr->getBooleanField(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getBooleanField(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvByte: {
            epics::pvData::PVBytePtr fieldPtr = destPvStructurePtr->getByteField(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getByteField(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvUByte: {
            epics::pvData::PVUBytePtr fieldPtr = destPvStructurePtr->getUByteField(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getUByteField(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvShort: {
            epics::pvData::PVShortPtr fieldPtr = destPvStructurePtr->getShortField(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getShortField(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvUShort: {
            epics::pvData::PVUShortPtr fieldPtr = destPvStructurePtr->getUShortField(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getUShortField(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvInt: {
            epics::pvData::PVIntPtr fieldPtr = destPvStructurePtr->getIntField(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getIntField(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvUInt: {
            epics::pvData::PVUIntPtr fieldPtr = destPvStructurePtr->getUIntField(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getUIntField(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvLong: {
            epics::pvData::PVLongPtr fieldPtr = destPvStructurePtr->getLongField(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getLongField(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvULong: {
            epics::pvData::PVULongPtr fieldPtr = destPvStructurePtr->getULongField(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getULongField(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvFloat: {
            epics::pvData::PVFloatPtr fieldPtr = destPvStructurePtr->getFloatField(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getFloatField(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvDouble: {
            epics::pvData::PVDoublePtr fieldPtr = destPvStructurePtr->getDoubleField(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getDoubleField(fieldName)->get());
            }
            break;
        }
        case epics::pvData::pvString: {
            epics::pvData::PVStringPtr fieldPtr = destPvStructurePtr->getStringField(fieldName);
            if (fieldPtr) {
                fieldPtr->put(srcPvStructurePtr->getStringField(fieldName)->get());
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
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            copyScalarArrayToScalarArray<epics::pvData::PVBooleanArray, epics::pvData::BooleanArrayData>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvByte: {
            copyScalarArrayToScalarArray<epics::pvData::PVByteArray, epics::pvData::ByteArrayData>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvUByte: {
            copyScalarArrayToScalarArray<epics::pvData::PVUByteArray, epics::pvData::UByteArrayData>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvShort: {
            copyScalarArrayToScalarArray<epics::pvData::PVShortArray, epics::pvData::ShortArrayData>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvUShort: {
            copyScalarArrayToScalarArray<epics::pvData::PVUShortArray, epics::pvData::UShortArrayData>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvInt: {
            copyScalarArrayToScalarArray<epics::pvData::PVIntArray, epics::pvData::IntArrayData>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvUInt: {
            copyScalarArrayToScalarArray<epics::pvData::PVUIntArray, epics::pvData::UIntArrayData>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvLong: {
            copyScalarArrayToScalarArray<epics::pvData::PVLongArray, epics::pvData::LongArrayData>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvULong: {
            copyScalarArrayToScalarArray<epics::pvData::PVULongArray, epics::pvData::ULongArrayData>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvFloat: {
            copyScalarArrayToScalarArray<epics::pvData::PVFloatArray, epics::pvData::FloatArrayData>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvDouble: {
            copyScalarArrayToScalarArray<epics::pvData::PVDoubleArray, epics::pvData::DoubleArrayData>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        case epics::pvData::pvString: {
            copyScalarArrayToScalarArray<epics::pvData::PVStringArray, epics::pvData::StringArrayData>(srcPvScalarArrayPtr, destPvScalarArrayPtr);
            break;
        }
        default: {
            throw InvalidDataType("Unrecognized scalar type: %d", scalarType);
        }
    }
#else
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
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
}
    
} // namespace PyPvDataUtility


