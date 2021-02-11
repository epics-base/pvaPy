// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution


#if PVA_API_VERSION >= 482
#include <pv/json.h>
#include <pv/bitSet.h>
#endif // if PVA_API_VERSION >= 482

#include "boost/python.hpp"
#include "boost/python/object.hpp"
#include "boost/python/tuple.hpp"
#include "boost/python/extract.hpp"
#include "boost/python/stl_iterator.hpp"

#include "PvObject.h"
#include "PvType.h"
#include "PvaConstants.h"
#include "PvaException.h"
#include "PyPvDataUtility.h"
#include "StringUtility.h"
#include "InvalidArgument.h"
#include "InvalidRequest.h"
#include "FieldNotFound.h"

namespace pvd = epics::pvData;
namespace bp = boost::python;
#if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1
namespace np = numpy_;
#endif // if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1

#if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1
#include NUMPY_HEADER_FILE
const bool PvObject::UseNumPyArraysDefault(true);
#else
const bool PvObject::UseNumPyArraysDefault(false);
#endif // if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1

// Constants
const char* PvObject::ValueFieldKey(PVA_VALUE_FIELD_KEY);
const char* PvObject::StructureId(PVA_STRUCTURE_ID);

// Constructors
PvObject::PvObject(const pvd::PVStructurePtr& pvStructurePtr_)
    : numPyInitialized(initializeBoostNumPy()),
    pvStructurePtr(pvStructurePtr_),
    dataType(PvType::Structure),
    useNumPyArrays(UseNumPyArraysDefault)
{
}

PvObject::PvObject(const bp::dict& structureDict, const std::string& structureId)
    : numPyInitialized(initializeBoostNumPy()),
    pvStructurePtr(pvd::getPVDataCreate()->createPVStructure(PyPvDataUtility::createStructureFromDict(structureDict, structureId))),
    dataType(PvType::Structure),
    useNumPyArrays(UseNumPyArraysDefault)
{
}

PvObject::PvObject(const bp::dict& structureDict, const bp::dict& valueDict, const std::string& structureId)
    : numPyInitialized(initializeBoostNumPy()),
    pvStructurePtr(pvd::getPVDataCreate()->createPVStructure(PyPvDataUtility::createStructureFromDict(structureDict, structureId))),
    dataType(PvType::Structure),
    useNumPyArrays(UseNumPyArraysDefault)
{
    PyPvDataUtility::pyDictToStructure(valueDict, pvStructurePtr);
}

PvObject::PvObject(const PvObject& pvObject)
    : numPyInitialized(initializeBoostNumPy()),
    pvStructurePtr(pvObject.pvStructurePtr),
    dataType(pvObject.dataType),
    useNumPyArrays(pvObject.useNumPyArrays)
{
}

// Destructor
PvObject::~PvObject()
{
}

// Static methods
bool PvObject::isPvObjectInstance(const bp::object& pyObject)
{
    bp::extract<PvObject> extractPvObject(pyObject);
    if (extractPvObject.check()) {
        return true;
    }
    return false;
}

bool PvObject::pvObjectToPyDict(const bp::object& pyObject, bp::object& pyDict)
{
    bp::extract<PvObject> extractPvObject(pyObject);
    if (extractPvObject.check()) {
        PvObject o = extractPvObject();
        pyDict = o.toDict();
        return true;
    }
    return false;
}


// Operators/conversion methods
pvd::PVStructurePtr PvObject::getPvStructurePtr() const
{
    return pvStructurePtr;
}

pvd::StructureConstPtr PvObject::getStructurePtr() const
{
    return pvStructurePtr->getStructure();
}

PvObject::operator pvd::PVStructurePtr()
{
    return pvStructurePtr;
}

PvObject::operator bp::dict() const
{
    return toDict();
}

bp::dict PvObject::toDict() const
{
    bp::dict pyDict;
    PyPvDataUtility::structureToPyDict(pvStructurePtr, pyDict, useNumPyArrays);
    return pyDict;
}

// Introspection
bp::dict PvObject::getStructureDict() const 
{
    bp::dict pyDict;
    PyPvDataUtility::structureToPyDict(getStructurePtr(), pyDict);
    return pyDict;
}

PvType::DataType PvObject::getDataType()
{
    return dataType;
}

std::ostream& operator<<(std::ostream& out, const PvObject& pvObject)
{
    out << *(pvObject.pvStructurePtr.get());
    return out;
}

pvd::PVStructurePtr& operator<<(pvd::PVStructurePtr& pvStructurePtr, const PvObject& pvObject)
{
    PyPvDataUtility::copyStructureToStructure(pvObject.getPvStructurePtr(), pvStructurePtr);
    return pvStructurePtr;
}

//
// Dictionary methods
//
bool PvObject::has_key(const std::string& fieldPath) const
{
    return hasField(fieldPath);
}

bp::list PvObject::items() const
{
    return toDict().items();
}

bp::list PvObject::keys() const
{
    return toDict().keys();
}

bp::list PvObject::values() const 
{
    return toDict().values();
}

//
// Has field?
//
bool PvObject::hasField(const std::string& fieldPath) const
{
    try {
        PyPvDataUtility::checkFieldPathExists(fieldPath, pvStructurePtr);
        return true;
    }
    catch (PvaException) {
        return false;
    }
}

//
// Object set/get
//
void PvObject::set(const bp::dict& pyDict)
{
    PyPvDataUtility::pyDictToStructure(pyDict, pvStructurePtr);
}

void PvObject::set(const PvObject& pvObject)
{
    PyPvDataUtility::pyDictToStructure(pvObject.toDict(), pvStructurePtr);
}

bp::dict PvObject::get() const
{
    return toDict();
}

void PvObject::setPyObject(const std::string& fieldPath, const bp::object& pyObject)
{
    PyPvDataUtility::setPyObjectToFieldPath(pyObject, fieldPath, pvStructurePtr);
}

void PvObject::setPyObject(const bp::object& pyObject)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setPyObject(key, pyObject);
}

bp::object PvObject::getPyObject(const std::string& fieldPath) const
{
    return PyPvDataUtility::getFieldPathAsPyObject(fieldPath, pvStructurePtr, useNumPyArrays);
}

bp::object PvObject::getPyObject() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getPyObject(key);
}

std::string PvObject::getAsString(const std::string& fieldPath) const
{
    if (PyPvDataUtility::isFieldPathCharScalarArray(fieldPath, pvStructurePtr)) {
        bp::object o = PyPvDataUtility::getFieldPathAsPyObject(fieldPath, pvStructurePtr, false);
        return PyUtility::extractStringFromPyList(o);
    }
    else {
        bp::object o = PyPvDataUtility::getFieldPathAsPyObject(fieldPath, pvStructurePtr, useNumPyArrays);
        return PyUtility::extractStringFromPyObject(o);
    }
}

std::string PvObject::getAsString() const
{
    std::string fieldPath = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getAsString(fieldPath);
}

// Boolean modifiers/accessors
void PvObject::setBoolean(const std::string& key, bool value)
{
    PyPvDataUtility::getBooleanField(key, pvStructurePtr)->put(value);
}

void PvObject::setBoolean(bool value)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setBoolean(key, value);
}

bool PvObject::getBoolean(const std::string& key) const
{
    return PyPvDataUtility::getBooleanField(key, pvStructurePtr)->get();
}
bool PvObject::getBoolean() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getBoolean(key);
}

// Byte modifiers/accessors
void PvObject::setByte(const std::string& key, char value)
{
    PyPvDataUtility::getByteField(key, pvStructurePtr)->put(value);
}

void PvObject::setByte(char value)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setByte(key, value);
}

char PvObject::getByte(const std::string& key) const
{
    return PyPvDataUtility::getByteField(key, pvStructurePtr)->get();
}

char PvObject::getByte() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getByte(key);
}

// UByte modifiers/accessors
void PvObject::setUByte(const std::string& key, unsigned char value)
{
    PyPvDataUtility::getUByteField(key, pvStructurePtr)->put(value);
}

void PvObject::setUByte(unsigned char value)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setUByte(key, value);
}

unsigned char PvObject::getUByte(const std::string& key) const
{
    return PyPvDataUtility::getUByteField(key, pvStructurePtr)->get();
}

unsigned char PvObject::getUByte() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getUByte(key);
}

// Short modifiers/accessors
void PvObject::setShort(const std::string& key, short value)
{
    PyPvDataUtility::getShortField(key, pvStructurePtr)->put(value);
}

void PvObject::setShort(short value)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setShort(key, value);
}

short PvObject::getShort(const std::string& key) const
{
    return PyPvDataUtility::getShortField(key, pvStructurePtr)->get();
}

short PvObject::getShort() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getShort(key);
}

// UShort modifiers/accessors
void PvObject::setUShort(const std::string& key, unsigned short value)
{
    PyPvDataUtility::getUShortField(key, pvStructurePtr)->put(value);
}

void PvObject::setUShort(unsigned short value)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setUShort(key, value);
}

unsigned short PvObject::getUShort(const std::string& key) const
{
    return PyPvDataUtility::getUShortField(key, pvStructurePtr)->get();
}

unsigned short PvObject::getUShort() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getUShort(key);
}

// Int modifiers/accessors
void PvObject::setInt(const std::string& key, int value)
{
    PyPvDataUtility::getIntField(key, pvStructurePtr)->put(value);
}

void PvObject::setInt(int value)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setInt(key, value);
}

int PvObject::getInt(const std::string& key) const
{
    return PyPvDataUtility::getIntField(key, pvStructurePtr)->get();
}

int PvObject::getInt() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getInt(key);
}

// UInt modifiers/accessors
void PvObject::setUInt(const std::string& key, unsigned int value)
{
    PyPvDataUtility::getUIntField(key, pvStructurePtr)->put(value);
}

void PvObject::setUInt(unsigned int value)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setUInt(key, value);
}

unsigned int PvObject::getUInt(const std::string& key) const
{
    return PyPvDataUtility::getUIntField(key, pvStructurePtr)->get();
}

unsigned int PvObject::getUInt() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getUInt(key);
}

// Long modifiers/accessors
void PvObject::setLong(const std::string& key, long long value)
{
    PyPvDataUtility::getLongField(key, pvStructurePtr)->put(value);
}

void PvObject::setLong(long long value)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setLong(key, value);
}

long long PvObject::getLong(const std::string& key) const
{
    return PyPvDataUtility::getLongField(key, pvStructurePtr)->get();
}

long long PvObject::getLong() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getLong(key);
}

// ULong modifiers/accessors
void PvObject::setULong(const std::string& key, unsigned long long value)
{
    PyPvDataUtility::getULongField(key, pvStructurePtr)->put(value);
}

void PvObject::setULong(unsigned long long value)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setULong(key, value);
}

unsigned long long PvObject::getULong(const std::string& key) const
{
    return PyPvDataUtility::getULongField(key, pvStructurePtr)->get();
}

unsigned long long PvObject::getULong() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getULong(key);
}

// Float modifiers/accessors
void PvObject::setFloat(const std::string& key, float value)
{
    PyPvDataUtility::getFloatField(key, pvStructurePtr)->put(value);
}

void PvObject::setFloat(float value)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setFloat(key, value);
}

float PvObject::getFloat(const std::string& key) const
{
    return PyPvDataUtility::getFloatField(key, pvStructurePtr)->get();
}

float PvObject::getFloat() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getFloat(key);
}

// Double modifiers/accessors
void PvObject::setDouble(const std::string& key, double value)
{
    PyPvDataUtility::getDoubleField(key, pvStructurePtr)->put(value);
}

void PvObject::setDouble(double value)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setDouble(key, value);
}

double PvObject::getDouble(const std::string& key) const
{
    return PyPvDataUtility::getDoubleField(key, pvStructurePtr)->get();
}

double PvObject::getDouble() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getDouble(key);
}

// String modifiers/accessors
void PvObject::setString(const std::string& key, const std::string& value)
{
    PyPvDataUtility::getStringField(key, pvStructurePtr)->put(value);
}

void PvObject::setString(const std::string& value)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setString(key, value);
}

std::string PvObject::getString(const std::string& key) const
{
    return PyPvDataUtility::getStringField(key, pvStructurePtr)->get();
}

std::string PvObject::getString() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getString(key);
}

// Scalar array modifiers/accessors
void PvObject::setScalarArray(const std::string& key, const bp::object& pyObject)
{
    PyPvDataUtility::pyObjectToScalarArrayField(pyObject, key, pvStructurePtr);
}

void PvObject::setScalarArray(const bp::object& pyObject)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setScalarArray(key, pyObject);
}

bp::object PvObject::getScalarArray(const std::string& key) const
{
    return PyPvDataUtility::getScalarArrayFieldAsPyObject(key, pvStructurePtr, useNumPyArrays);
}

bp::object PvObject::getScalarArray() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getScalarArray(key);
}

// Structure modifiers/accessors
void PvObject::setStructure(const std::string& key, const PvObject& pvObject)
{
    pvd::PVStructurePtr pvStructurePtr2 = PyPvDataUtility::getStructureField(key, pvStructurePtr);
    PyPvDataUtility::copyStructureToStructure(pvObject.getPvStructurePtr(), pvStructurePtr2);
}

void PvObject::setStructure(const PvObject& pvObject)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setStructure(key, pvObject);
}

void PvObject::setStructure(const std::string& key, const bp::dict& pyDict)
{
    PyPvDataUtility::pyDictToStructureField(pyDict, key, pvStructurePtr);
}

void PvObject::setStructure(const bp::dict& pyDict)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setStructure(key, pyDict);
}

bp::dict PvObject::getStructure(const std::string& key) const
{
    bp::dict pyDict;
    PyPvDataUtility::structureFieldToPyDict(key, pvStructurePtr, pyDict, useNumPyArrays);
    return pyDict;
}

bp::dict PvObject::getStructure() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getStructure(key);
}

// Structure array modifiers/accessors
void PvObject::setStructureArray(const std::string& key, const bp::list& pyList)
{
    PyPvDataUtility::pyListToStructureArrayField(pyList, key, pvStructurePtr);
}

void PvObject::setStructureArray(const bp::list& pyList)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setStructureArray(key, pyList);
}

bp::list PvObject::getStructureArray(const std::string& key) const
{
    bp::list pyList;
    PyPvDataUtility::structureArrayFieldToPyList(key, pvStructurePtr, pyList, useNumPyArrays);
    return pyList;
}

bp::list PvObject::getStructureArray() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getStructureArray(key);
}

// Union fields
void PvObject::setUnion(const std::string& key, const PvObject& value)
{
    pvd::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    std::string keyFrom = PyPvDataUtility::getValueOrSingleFieldName(value.getPvStructurePtr());
    pvd::PVFieldPtr pvFrom = PyPvDataUtility::getSubField(keyFrom, value.getPvStructurePtr());
    PyPvDataUtility::setUnionField(pvFrom, pvUnionPtr);
}

void PvObject::setUnion(const PvObject& value)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setUnion(key, value);
}

void PvObject::setUnion(const std::string& key, const bp::dict& pyDict)
{
    PyPvDataUtility::pyDictToUnionField(pyDict, key, pvStructurePtr);
}

void PvObject::setUnion(const bp::dict& pyDict)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setUnion(key, pyDict);
}

void PvObject::setUnion(const std::string& key, const bp::tuple& pyTuple)
{
    PyPvDataUtility::pyTupleToUnionField(pyTuple, key, pvStructurePtr);
}

void PvObject::setUnion(const bp::tuple& pyTuple)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setUnion(key, pyTuple);
}

PvObject PvObject::getUnion(const std::string& key) const
{
    pvd::PVStructurePtr unionPvStructurePtr = PyPvDataUtility::getUnionPvStructurePtr(key, pvStructurePtr);
    return PvObject(unionPvStructurePtr); 
}

PvObject PvObject::getUnion() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getUnion(key);
}

bp::list PvObject::getUnionFieldNames(const std::string& key) const
{
    pvd::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    pvd::StringArray names = pvUnionPtr->getUnion()->getFieldNames();
    bp::list pyList;
    PyPvDataUtility::stringArrayToPyList(names, pyList);
    return pyList;
}

bp::list PvObject::getUnionFieldNames() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getUnionFieldNames(key);
}

std::string PvObject::getSelectedUnionFieldName(const std::string& key) const
{
    pvd::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    return pvUnionPtr->getSelectedFieldName();
}

std::string PvObject::getSelectedUnionFieldName() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getSelectedUnionFieldName(key);
}

PvObject PvObject::selectUnionField(const std::string& key, const std::string& fieldName) const
{
    pvd::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    try {
        pvd::PVFieldPtr pvField = pvUnionPtr->select(fieldName);
    } 
    catch (std::runtime_error e) {
        throw FieldNotFound("Unknown field name: %s", fieldName.c_str());
    }
    return getUnion(key);
}

PvObject PvObject::selectUnionField(const std::string& fieldName) const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return selectUnionField(key, fieldName);
}

bool PvObject::isUnionVariant(const std::string& key) const
{
    pvd::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    return pvUnionPtr->getUnion()->isVariant();
}

bool PvObject::isUnionVariant() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return isUnionVariant(key);
}

PvObject PvObject::createUnionField(const std::string& key, const std::string& fieldName) const
{
    pvd::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    return PvObject(PyPvDataUtility::createUnionFieldPvStructure(pvUnionPtr->getUnion(), fieldName));
}

PvObject PvObject::createUnionField(const std::string& fieldName) const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return createUnionField(key, fieldName);
}

// UnionArray fields
void PvObject::setUnionArray(const std::string& key, const bp::list& pyList)
{
    PyPvDataUtility::pyListToUnionArrayField(pyList, key, pvStructurePtr);
}

void PvObject::setUnionArray(const bp::list& pyList)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setUnionArray(key, pyList);
}

bp::list PvObject::getUnionArray(const std::string& key) const
{
    pvd::PVUnionArrayPtr pvUnionArrayPtr = PyPvDataUtility::getUnionArrayField(key, pvStructurePtr);
    pvd::PVUnionArray::const_svector data = pvUnionArrayPtr->view();
    bp::list pyList;
    for(size_t i = 0; i < data.size(); ++i) {
        std::string fieldName = data[i]->getSelectedFieldName();
        pyList.append(PvObject(PyPvDataUtility::createUnionPvStructure(data[i], fieldName)));
    }
    return pyList;
}

bp::list PvObject::getUnionArray() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getUnionArray(key);
}

bool PvObject::isUnionArrayVariant(const std::string& key) const
{
    pvd::PVUnionArrayPtr pvUnionArrayPtr = PyPvDataUtility::getUnionArrayField(key, pvStructurePtr);
    return pvUnionArrayPtr->getUnionArray()->getUnion()->isVariant();
}

bool PvObject::isUnionArrayVariant() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return isUnionArrayVariant(key);
}

bp::list PvObject::getUnionArrayFieldNames(const std::string& key) const
{
    pvd::PVUnionArrayPtr pvUnionArrayPtr = PyPvDataUtility::getUnionArrayField(key, pvStructurePtr);
    pvd::StringArray names = pvUnionArrayPtr->getUnionArray()->getUnion()->getFieldNames();
    bp::list pyList;
    PyPvDataUtility::stringArrayToPyList(names, pyList);
    return pyList;
}

bp::list PvObject::getUnionArrayFieldNames() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getUnionArrayFieldNames(key);
}

PvObject PvObject::createUnionArrayElementField(const std::string& key, const std::string& fieldName) const
{
    pvd::PVUnionArrayPtr pvUnionArrayPtr = PyPvDataUtility::getUnionArrayField(key, pvStructurePtr);
    return PvObject(PyPvDataUtility::createUnionFieldPvStructure(pvUnionArrayPtr->getUnionArray()->getUnion(), fieldName));
}

PvObject PvObject::createUnionArrayElementField(const std::string& fieldName) const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return createUnionArrayElementField(key, fieldName);
}

PvObject PvObject::copy()
{
    pvd::PVStructurePtr pvStructurePtr2(pvd::getPVDataCreate()->createPVStructure(pvStructurePtr->getStructure()));
    pvStructurePtr2->copyUnchecked(*pvStructurePtr);
    return PvObject(pvStructurePtr2); 
}

// Methods specific to Boost NumPy 
bool PvObject::boostNumPyInitialized(false);
bool PvObject::initializeBoostNumPy() 
{
    if (!boostNumPyInitialized) {
        boostNumPyInitialized = true;
#if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1
        numpy_::initialize();
#endif // if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1
    }
    return true;
}

#if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1

void PvObject::setUseNumPyArraysFlag(bool useNumPyArrays)
{
    this->useNumPyArrays = useNumPyArrays; 
}

bool PvObject::getUseNumPyArraysFlag() const
{
    return useNumPyArrays;
}

#endif // if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1

#if PVA_API_VERSION >= 482
std::string PvObject::toJSON(bool multiLine)
{
    if(!pvStructurePtr) throw PvaException("pvStructure is null");
    try {
        pvd::JSONPrintOptions opts;
        opts.ignoreUnprintable = true;
        opts.multiLine = multiLine;
        pvd::BitSetPtr bitSet(new pvd::BitSet(pvStructurePtr->getStructure()->getNumberFields()));
        bitSet->set(0);
        std::ostringstream strm;
        pvd::printJSON(strm,*pvStructurePtr,*bitSet,opts);
        return strm.str();
    } 
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
}
#endif // if PVA_API_VERSION >= 482


