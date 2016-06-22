// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvObject.h"
#include "PvType.h"
#include "PvaConstants.h"
#include "PvaException.h"
#include "PyPvDataUtility.h"
#include "StringUtility.h"
#include "InvalidArgument.h"
#include "InvalidRequest.h"
#include "FieldNotFound.h"
#include "boost/python/object.hpp"
#include "boost/python/tuple.hpp"
#include "boost/python/extract.hpp"
#include "boost/python/stl_iterator.hpp"

#if defined HAVE_BOOST_NUM_PY && HAVE_BOOST_NUM_PY == 1
#include "boost/numpy.hpp"
const bool PvObject::UseNumPyArraysDefault(true);
#else
const bool PvObject::UseNumPyArraysDefault(false);
#endif // if defined HAVE_BOOST_NUM_PY && HAVE_BOOST_NUM_PY == 1

// Constants
const char* PvObject::ValueFieldKey(PvaConstants::ValueFieldKey);
const char* PvObject::StructureId("structure");

// Constructors
PvObject::PvObject(const epics::pvData::PVStructurePtr& pvStructurePtr_)
    : pvStructurePtr(pvStructurePtr_),
    dataType(PvType::Structure),
    useNumPyArrays(UseNumPyArraysDefault)
{
}

PvObject::PvObject(const boost::python::dict& structureDict, const std::string& structureId)
    : pvStructurePtr(epics::pvData::getPVDataCreate()->createPVStructure(PyPvDataUtility::createStructureFromDict(structureDict, structureId))),
    dataType(PvType::Structure),
    useNumPyArrays(UseNumPyArraysDefault)
{
}

PvObject::PvObject(const boost::python::dict& structureDict, const boost::python::dict& valueDict, const std::string& structureId)
    : pvStructurePtr(epics::pvData::getPVDataCreate()->createPVStructure(PyPvDataUtility::createStructureFromDict(structureDict, structureId))),
    dataType(PvType::Structure),
    useNumPyArrays(UseNumPyArraysDefault)
{
    PyPvDataUtility::pyDictToStructure(valueDict, pvStructurePtr);
}

PvObject::PvObject(const PvObject& pvObject)
    : pvStructurePtr(pvObject.pvStructurePtr),
    dataType(pvObject.dataType),
    useNumPyArrays(pvObject.useNumPyArrays)
{
}

// Destructor
PvObject::~PvObject()
{
}

// Operators/conversion methods
epics::pvData::PVStructurePtr PvObject::getPvStructurePtr() const
{
    return pvStructurePtr;
}

epics::pvData::StructureConstPtr PvObject::getStructurePtr() const
{
    return pvStructurePtr->getStructure();
}

PvObject::operator epics::pvData::PVStructurePtr()
{
    return pvStructurePtr;
}

PvObject::operator boost::python::dict() const
{
    return toDict();
}

boost::python::dict PvObject::toDict() const
{
    boost::python::dict pyDict;
    PyPvDataUtility::structureToPyDict(pvStructurePtr, pyDict, useNumPyArrays);
    return pyDict;
}

// Introspection
boost::python::dict PvObject::getStructureDict() const 
{
    boost::python::dict pyDict;
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

epics::pvData::PVStructurePtr& operator<<(epics::pvData::PVStructurePtr& pvStructurePtr, const PvObject& pvObject)
{
    PyPvDataUtility::copyStructureToStructure(pvObject.getPvStructurePtr(), pvStructurePtr);
    return pvStructurePtr;
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
void PvObject::set(const boost::python::dict& pyDict)
{
    PyPvDataUtility::pyDictToStructure(pyDict, pvStructurePtr);
}

boost::python::dict PvObject::get() const
{
    return toDict();
}

void PvObject::setPyObject(const std::string& fieldPath, const boost::python::object& pyObject)
{
    PyPvDataUtility::setPyObjectToFieldPath(pyObject, fieldPath, pvStructurePtr);
}

void PvObject::setPyObject(const boost::python::object& pyObject)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setPyObject(key, pyObject);
}

boost::python::object PvObject::getPyObject(const std::string& fieldPath) const
{
    return PyPvDataUtility::getFieldPathAsPyObject(fieldPath, pvStructurePtr, useNumPyArrays);
}

boost::python::object PvObject::getPyObject() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getPyObject(key);
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
void PvObject::setScalarArray(const std::string& key, const boost::python::list& pyList)
{
    PyPvDataUtility::pyListToScalarArrayField(pyList, key, pvStructurePtr);
}

void PvObject::setScalarArray(const boost::python::list& pyList)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setScalarArray(key, pyList);
}

boost::python::list PvObject::getScalarArray(const std::string& key) const
{
    boost::python::list pyList;
    PyPvDataUtility::scalarArrayFieldToPyList(key, pvStructurePtr, pyList);
    return pyList;
}

boost::python::list PvObject::getScalarArray() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getScalarArray(key);
}

// Structure modifiers/accessors
void PvObject::setStructure(const std::string& key, const boost::python::dict& pyDict)
{
    PyPvDataUtility::pyDictToStructureField(pyDict, key, pvStructurePtr);
}

void PvObject::setStructure(const boost::python::dict& pyDict)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setStructure(key, pyDict);
}

boost::python::dict PvObject::getStructure(const std::string& key) const
{
    boost::python::dict pyDict;
    PyPvDataUtility::structureFieldToPyDict(key, pvStructurePtr, pyDict, useNumPyArrays);
    return pyDict;
}

boost::python::dict PvObject::getStructure() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getStructure(key);
}

// Structure array modifiers/accessors
void PvObject::setStructureArray(const std::string& key, const boost::python::list& pyList)
{
    PyPvDataUtility::pyListToStructureArrayField(pyList, key, pvStructurePtr);
}

void PvObject::setStructureArray(const boost::python::list& pyList)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setStructureArray(key, pyList);
}

boost::python::list PvObject::getStructureArray(const std::string& key) const
{
    boost::python::list pyList;
    PyPvDataUtility::structureArrayFieldToPyList(key, pvStructurePtr, pyList, useNumPyArrays);
    return pyList;
}

boost::python::list PvObject::getStructureArray() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getStructureArray(key);
}

// Union fields
void PvObject::setUnion(const std::string& key, const PvObject& value)
{
    epics::pvData::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    std::string keyFrom = PyPvDataUtility::getValueOrSingleFieldName(value.getPvStructurePtr());
    epics::pvData::PVFieldPtr pvFrom = PyPvDataUtility::getSubField(keyFrom, value.getPvStructurePtr());
    PyPvDataUtility::setUnionField(pvFrom, pvUnionPtr);
}

void PvObject::setUnion(const PvObject& value)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setUnion(key, value);
}

void PvObject::setUnion(const std::string& key, const boost::python::dict& pyDict)
{
    PyPvDataUtility::pyDictToUnionField(pyDict, key, pvStructurePtr);
}

void PvObject::setUnion(const boost::python::dict& pyDict)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setUnion(key, pyDict);
}

void PvObject::setUnion(const std::string& key, const boost::python::tuple& pyTuple)
{
    PyPvDataUtility::pyTupleToUnionField(pyTuple, key, pvStructurePtr);
}

void PvObject::setUnion(const boost::python::tuple& pyTuple)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setUnion(key, pyTuple);
}

PvObject PvObject::getUnion(const std::string& key) const
{
    epics::pvData::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    std::string fieldName = pvUnionPtr->getSelectedFieldName();
    if (!fieldName.size()) {
        throw InvalidRequest("No field has been selected for union %s.", key.c_str());
    }
    return PvObject(PyPvDataUtility::createUnionPvStructure(pvUnionPtr, fieldName));
}

PvObject PvObject::getUnion() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getUnion(key);
}

boost::python::list PvObject::getUnionFieldNames(const std::string& key) const
{
    epics::pvData::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    epics::pvData::StringArray names = pvUnionPtr->getUnion()->getFieldNames();
    boost::python::list pyList;
    PyPvDataUtility::stringArrayToPyList(names, pyList);
    return pyList;
}

boost::python::list PvObject::getUnionFieldNames() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getUnionFieldNames(key);
}

std::string PvObject::getSelectedUnionFieldName(const std::string& key) const
{
    epics::pvData::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    return pvUnionPtr->getSelectedFieldName();
}

std::string PvObject::getSelectedUnionFieldName() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getSelectedUnionFieldName(key);
}

PvObject PvObject::selectUnionField(const std::string& key, const std::string& fieldName) const
{
    epics::pvData::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    try {
        epics::pvData::PVFieldPtr pvField = pvUnionPtr->select(fieldName);
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
    epics::pvData::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    return pvUnionPtr->getUnion()->isVariant();
}

bool PvObject::isUnionVariant() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return isUnionVariant(key);
}

PvObject PvObject::createUnionField(const std::string& key, const std::string& fieldName) const
{
    epics::pvData::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    return PvObject(PyPvDataUtility::createUnionFieldPvStructure(pvUnionPtr->getUnion(), fieldName));
}

PvObject PvObject::createUnionField(const std::string& fieldName) const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return createUnionField(key, fieldName);
}

// UnionArray fields
void PvObject::setUnionArray(const std::string& key, const boost::python::list& pyList)
{
    epics::pvData::PVUnionArrayPtr pvUnionArrayPtr = PyPvDataUtility::getUnionArrayField(key, pvStructurePtr);
    epics::pvData::UnionConstPtr unionPtr = pvUnionArrayPtr->getUnionArray()->getUnion();
    int listSize = boost::python::len(pyList);
    epics::pvData::PVUnionArray::svector data(listSize);
    for(size_t i = 0; i < data.size(); ++i) {
        epics::pvData::PVUnionPtr pvUnionPtr = epics::pvData::getPVDataCreate()->createPVUnion(unionPtr);
        boost::python::object pyObject = pyList[i];
        boost::python::extract<boost::python::tuple> extractTuple(pyObject);
        if (extractTuple.check()) {
            boost::python::tuple pyTuple = extractTuple();
            // Extract dictionary within tuple
            if (boost::python::len(pyTuple) != 1) {
                throw InvalidArgument("PV union tuple must have exactly one element.");
            }
            boost::python::object pyObject = pyTuple[0];
            boost::python::dict pyDict = PyUtility::extractValueFromPyObject<boost::python::dict>(pyObject);
            PyPvDataUtility::pyDictToUnion(pyDict, pvUnionPtr);
            data[i] = pvUnionPtr;
            continue;
        } 

        boost::python::extract<boost::python::dict> extractDict(pyObject);
        if (extractDict.check()) {
            boost::python::dict pyDict = extractDict();
            PyPvDataUtility::pyDictToUnion(pyDict, pvUnionPtr);
            data[i] = pvUnionPtr;
            continue;
        } 

        boost::python::extract<PvObject> extractPvObject(pyObject);
        if (extractPvObject.check()) {
            PvObject pvObject = extractPvObject();
            std::string keyFrom = PyPvDataUtility::getValueOrSingleFieldName(pvObject.getPvStructurePtr());
            epics::pvData::PVFieldPtr pvFrom = PyPvDataUtility::getSubField(keyFrom, pvObject.getPvStructurePtr());
            PyPvDataUtility::setUnionField(pvFrom, pvUnionPtr);
            data[i] = pvUnionPtr;
            continue;
        }

        throw InvalidArgument("Python object representing an union must be PvObject, tuple containing dictionary, or dictionary."); 
    } 

    pvUnionArrayPtr->replace(freeze(data));
}

void PvObject::setUnionArray(const boost::python::list& pyList)
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    setUnionArray(key, pyList);
}

boost::python::list PvObject::getUnionArray(const std::string& key) const
{
    epics::pvData::PVUnionArrayPtr pvUnionArrayPtr = PyPvDataUtility::getUnionArrayField(key, pvStructurePtr);
    epics::pvData::PVUnionArray::const_svector data = pvUnionArrayPtr->view();
    boost::python::list pyList;
    for(size_t i = 0; i < data.size(); ++i) {
        std::string fieldName = data[i]->getSelectedFieldName();
        pyList.append(PvObject(PyPvDataUtility::createUnionPvStructure(data[i], fieldName)));
    }
    return pyList;
}

boost::python::list PvObject::getUnionArray() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getUnionArray(key);
}

bool PvObject::isUnionArrayVariant(const std::string& key) const
{
    epics::pvData::PVUnionArrayPtr pvUnionArrayPtr = PyPvDataUtility::getUnionArrayField(key, pvStructurePtr);
    return pvUnionArrayPtr->getUnionArray()->getUnion()->isVariant();
}

bool PvObject::isUnionArrayVariant() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return isUnionArrayVariant(key);
}

boost::python::list PvObject::getUnionArrayFieldNames(const std::string& key) const
{
    epics::pvData::PVUnionArrayPtr pvUnionArrayPtr = PyPvDataUtility::getUnionArrayField(key, pvStructurePtr);
    epics::pvData::StringArray names = pvUnionArrayPtr->getUnionArray()->getUnion()->getFieldNames();
    boost::python::list pyList;
    PyPvDataUtility::stringArrayToPyList(names, pyList);
    return pyList;
}

boost::python::list PvObject::getUnionArrayFieldNames() const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return getUnionArrayFieldNames(key);
}

PvObject PvObject::createUnionArrayElementField(const std::string& key, const std::string& fieldName) const
{
    epics::pvData::PVUnionArrayPtr pvUnionArrayPtr = PyPvDataUtility::getUnionArrayField(key, pvStructurePtr);
    return PvObject(PyPvDataUtility::createUnionFieldPvStructure(pvUnionArrayPtr->getUnionArray()->getUnion(), fieldName));
}

PvObject PvObject::createUnionArrayElementField(const std::string& fieldName) const
{
    std::string key = PyPvDataUtility::getValueOrSingleFieldName(pvStructurePtr);
    return createUnionArrayElementField(key, fieldName);
}

// Methods specific to Boost NumPy 
#if defined HAVE_BOOST_NUM_PY && HAVE_BOOST_NUM_PY == 1
bool PvObject::boostNumPyInitialized(initializeBoostNumPy());
bool PvObject::initializeBoostNumPy() 
{
    boost::numpy::initialize();
    return true;
}

void PvObject::setUseNumPyArraysFlag(bool useNumPyArrays)
{
    this->useNumPyArrays = useNumPyArrays; 
}

bool PvObject::getUseNumPyArraysFlag() const
{
    return useNumPyArrays;
}

#endif // if defined HAVE_BOOST_NUM_PY && HAVE_BOOST_NUM_PY == 1


