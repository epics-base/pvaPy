#include "PvObject.h"
#include "PvType.h"
#include "PvaConstants.h"
#include "PvaException.h"
#include "PyPvDataUtility.h"
#include "StringUtility.h"
#include "InvalidRequest.h"
#include "InvalidDataType.h"
#include "InvalidArgument.h"
#include "FieldNotFound.h"
#include "boost/python/object.hpp"
#include "boost/python/tuple.hpp"
#include "boost/python/extract.hpp"
#include "boost/python/stl_iterator.hpp"

// Constants
const char* PvObject::ValueFieldKey(PvaConstants::ValueFieldKey);
const char* PvObject::StructureId("structure");

// Constructors
PvObject::PvObject(const epics::pvData::PVStructurePtr& pvStructurePtr_)
    : pvStructurePtr(pvStructurePtr_),
    dataType(PvType::Structure)
{
}

PvObject::PvObject(const boost::python::dict& pyDict, const std::string& structureId)
    : pvStructurePtr(epics::pvData::getPVDataCreate()->createPVStructure(createStructureFromDict(pyDict, structureId))),
    dataType(PvType::Structure)
{
}

PvObject::PvObject(const PvObject& pvObject)
    : pvStructurePtr(pvObject.pvStructurePtr),
    dataType(pvObject.dataType)
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
    PyPvDataUtility::structureToPyDict(pvStructurePtr, pyDict);
    return pyDict;
}

boost::python::dict PvObject::getStructureDict() 
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

// Boolean modifiers/accessors
void PvObject::setBoolean(const std::string& key, bool value)
{
    PyPvDataUtility::getBooleanField(key, pvStructurePtr)->put(value);
}

void PvObject::setBoolean(bool value)
{
    std::string key = PyPvDataUtility::getValueOrSelectedUnionFieldName(pvStructurePtr);
    PyPvDataUtility::getBooleanField(key, pvStructurePtr)->put(value);
}

bool PvObject::getBoolean(const std::string& key) const
{
    return PyPvDataUtility::getBooleanField(key, pvStructurePtr)->get();
}
bool PvObject::getBoolean() const
{
    std::string key = PyPvDataUtility::getValueOrSelectedUnionFieldName(pvStructurePtr);
    return PyPvDataUtility::getBooleanField(key, pvStructurePtr)->get();
}

// Byte modifiers/accessors
void PvObject::setByte(const std::string& key, char value)
{
    PyPvDataUtility::getByteField(key, pvStructurePtr)->put(value);
}

void PvObject::setByte(char value)
{
    PyPvDataUtility::getByteField(ValueFieldKey, pvStructurePtr)->put(value);
}

char PvObject::getByte(const std::string& key) const
{
    return PyPvDataUtility::getByteField(key, pvStructurePtr)->get();
}

// UByte modifiers/accessors
void PvObject::setUByte(const std::string& key, unsigned char value)
{
    PyPvDataUtility::getUByteField(key, pvStructurePtr)->put(value);
}

void PvObject::setUByte(unsigned char value)
{
    PyPvDataUtility::getUByteField(ValueFieldKey, pvStructurePtr)->put(value);
}

unsigned char PvObject::getUByte(const std::string& key) const
{
    return PyPvDataUtility::getUByteField(key, pvStructurePtr)->get();
}

// Short modifiers/accessors
void PvObject::setShort(const std::string& key, short value)
{
    PyPvDataUtility::getShortField(key, pvStructurePtr)->put(value);
}

void PvObject::setShort(short value)
{
    PyPvDataUtility::getShortField(ValueFieldKey, pvStructurePtr)->put(value);
}

short PvObject::getShort(const std::string& key) const
{
    return PyPvDataUtility::getShortField(key, pvStructurePtr)->get();
}

// UShort modifiers/accessors
void PvObject::setUShort(const std::string& key, unsigned short value)
{
    PyPvDataUtility::getUShortField(key, pvStructurePtr)->put(value);
}

void PvObject::setUShort(unsigned short value)
{
    PyPvDataUtility::getUShortField(ValueFieldKey, pvStructurePtr)->put(value);
}

unsigned short PvObject::getUShort(const std::string& key) const
{
    return PyPvDataUtility::getUShortField(key, pvStructurePtr)->get();
}

// Int modifiers/accessors
void PvObject::setInt(const std::string& key, int value)
{
    PyPvDataUtility::getIntField(key, pvStructurePtr)->put(value);
}

void PvObject::setInt(int value)
{
    PyPvDataUtility::getIntField(ValueFieldKey, pvStructurePtr)->put(value);
}

int PvObject::getInt(const std::string& key) const
{
    return PyPvDataUtility::getIntField(key, pvStructurePtr)->get();
}

// UInt modifiers/accessors
void PvObject::setUInt(const std::string& key, unsigned int value)
{
    PyPvDataUtility::getUIntField(key, pvStructurePtr)->put(value);
}

void PvObject::setUInt(unsigned int value)
{
    PyPvDataUtility::getUIntField(ValueFieldKey, pvStructurePtr)->put(value);
}

unsigned int PvObject::getUInt(const std::string& key) const
{
    return PyPvDataUtility::getUIntField(key, pvStructurePtr)->get();
}

// Long modifiers/accessors
void PvObject::setLong(const std::string& key, long long value)
{
    PyPvDataUtility::getLongField(key, pvStructurePtr)->put(value);
}

void PvObject::setLong(long long value)
{
    PyPvDataUtility::getLongField(ValueFieldKey, pvStructurePtr)->put(value);
}

long long PvObject::getLong(const std::string& key) const
{
    return PyPvDataUtility::getLongField(key, pvStructurePtr)->get();
}

// ULong modifiers/accessors
void PvObject::setULong(const std::string& key, unsigned long long value)
{
    PyPvDataUtility::getULongField(key, pvStructurePtr)->put(value);
}

void PvObject::setULong(unsigned long long value)
{
    PyPvDataUtility::getULongField(ValueFieldKey, pvStructurePtr)->put(value);
}

unsigned long long PvObject::getULong(const std::string& key) const
{
    return PyPvDataUtility::getULongField(key, pvStructurePtr)->get();
}

// Float modifiers/accessors
void PvObject::setFloat(const std::string& key, float value)
{
    PyPvDataUtility::getFloatField(key, pvStructurePtr)->put(value);
}

void PvObject::setFloat(float value)
{
    PyPvDataUtility::getFloatField(ValueFieldKey, pvStructurePtr)->put(value);
}

float PvObject::getFloat(const std::string& key) const
{
    return PyPvDataUtility::getFloatField(key, pvStructurePtr)->get();
}

// Double modifiers/accessors
void PvObject::setDouble(const std::string& key, double value)
{
    PyPvDataUtility::getDoubleField(key, pvStructurePtr)->put(value);
}

void PvObject::setDouble(double value)
{
    std::string key = PyPvDataUtility::getValueOrSelectedUnionFieldName(pvStructurePtr);
    PyPvDataUtility::getDoubleField(key, pvStructurePtr)->put(value);
}

double PvObject::getDouble(const std::string& key) const
{
    return PyPvDataUtility::getDoubleField(key, pvStructurePtr)->get();
}

double PvObject::getDouble() const
{
    std::string key = PyPvDataUtility::getValueOrSelectedUnionFieldName(pvStructurePtr);
    return PyPvDataUtility::getDoubleField(key, pvStructurePtr)->get();
}

// String modifiers/accessors
void PvObject::setString(const std::string& key, const std::string& value)
{
    PyPvDataUtility::getStringField(key, pvStructurePtr)->put(value);
}

void PvObject::setString(const std::string& value)
{
    PyPvDataUtility::getStringField(ValueFieldKey, pvStructurePtr)->put(value);
}

std::string PvObject::getString(const std::string& key) const
{
    return PyPvDataUtility::getStringField(key, pvStructurePtr)->get();
}

// Scalar array modifiers/accessors
void PvObject::setScalarArray(const std::string& key, const boost::python::list& pyList)
{
    PyPvDataUtility::pyListToScalarArrayField(pyList, key, pvStructurePtr);
}

void PvObject::setScalarArray(const boost::python::list& pyList)
{
    setScalarArray(ValueFieldKey, pyList);
}

boost::python::list PvObject::getScalarArray(const std::string& key) const
{
    boost::python::list pyList;
    PyPvDataUtility::scalarArrayFieldToPyList(key, pvStructurePtr, pyList);
    return pyList;
}

// Structure modifiers/accessors
void PvObject::setStructure(const std::string& key, const boost::python::dict& pyDict)
{
    PyPvDataUtility::pyDictToStructureField(pyDict, key, pvStructurePtr);
}

void PvObject::setStructure(const boost::python::dict& pyDict)
{
    setStructure(ValueFieldKey, pyDict);
}

boost::python::dict PvObject::getStructure(const std::string& key) const
{
    boost::python::dict pyDict;
    PyPvDataUtility::structureFieldToPyDict(key, pvStructurePtr, pyDict);
    return pyDict;
}

// Structure array modifiers/accessors
void PvObject::setStructureArray(const std::string& key, const boost::python::list& pyList)
{
    PyPvDataUtility::pyListToStructureArrayField(pyList, key, pvStructurePtr);
}

void PvObject::setStructureArray(const boost::python::list& pyList)
{
    setStructureArray(ValueFieldKey, pyList);
}

boost::python::list PvObject::getStructureArray(const std::string& key) const
{
    boost::python::list pyList;
    PyPvDataUtility::structureArrayFieldToPyList(key, pvStructurePtr, pyList);
    return pyList;
}

// Union fields
bool PvObject::isUnionVariant(const std::string& key) const
{
    epics::pvData::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    return pvUnionPtr->getUnion()->isVariant();
}

boost::python::list PvObject::getUnionFieldNames(const std::string& key) const
{
    epics::pvData::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    epics::pvData::StringArray names = pvUnionPtr->getUnion()->getFieldNames();
    boost::python::list pyList;
    PyPvDataUtility::stringArrayToPyList(names, pyList);
    return pyList;
}

PvObject PvObject::createUnionField(const std::string& fieldName, const std::string& key) const
{
    epics::pvData::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    return PvObject(PyPvDataUtility::createUnionFieldPvStructure(pvUnionPtr->getUnion(), fieldName));
}

PvObject PvObject::createUnionArrayElementField(const std::string& fieldName, const std::string& key) const
{
    epics::pvData::PVUnionArrayPtr pvUnionArrayPtr = PyPvDataUtility::getUnionArrayField(key, pvStructurePtr);
    return PvObject(PyPvDataUtility::createUnionFieldPvStructure(pvUnionArrayPtr->getUnionArray()->getUnion(), fieldName));
}

PvObject PvObject::selectUnionField(const std::string& fieldName, const std::string& key) const
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

std::string PvObject::getSelectedUnionFieldName(const std::string& key) const
{
    epics::pvData::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    return pvUnionPtr->getSelectedFieldName();
}

PvObject PvObject::getUnion(const std::string& key) const
{
    epics::pvData::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    std::string fieldName = pvUnionPtr->getSelectedFieldName();
    return PvObject(PyPvDataUtility::createUnionPvStructure(pvUnionPtr, fieldName));
}

void PvObject::setUnion(const PvObject& value, const std::string& key)
{
    epics::pvData::PVUnionPtr pvUnionPtr = PyPvDataUtility::getUnionField(key, pvStructurePtr);
    epics::pvData::PVFieldPtr pvFrom = PyPvDataUtility::getSubField(ValueFieldKey, value.getPvStructurePtr());
    setUnionField(pvFrom, pvUnionPtr);
}

// UnionArray fields
bool PvObject::isUnionArrayVariant(const std::string& key) const
{
    epics::pvData::PVUnionArrayPtr pvUnionArrayPtr = PyPvDataUtility::getUnionArrayField(key, pvStructurePtr);
    return pvUnionArrayPtr->getUnionArray()->getUnion()->isVariant();
}

boost::python::list PvObject::getUnionArrayFieldNames(const std::string& key) const
{
    epics::pvData::PVUnionArrayPtr pvUnionArrayPtr = PyPvDataUtility::getUnionArrayField(key, pvStructurePtr);
    epics::pvData::StringArray names = pvUnionArrayPtr->getUnionArray()->getUnion()->getFieldNames();
    boost::python::list pyList;
    PyPvDataUtility::stringArrayToPyList(names, pyList);
    return pyList;
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

void PvObject::setUnionArray(const boost::python::list& pyList, const std::string& key)
{
    epics::pvData::PVUnionArrayPtr pvUnionArrayPtr = PyPvDataUtility::getUnionArrayField(key, pvStructurePtr);
    epics::pvData::UnionConstPtr unionPtr = pvUnionArrayPtr->getUnionArray()->getUnion();
    int listSize = boost::python::len(pyList);
    epics::pvData::PVUnionArray::svector data(listSize);
    for(size_t i = 0; i < data.size(); ++i) {
        PvObject pvObject = boost::python::extract<PvObject>(pyList[i]);
        epics::pvData::PVStructurePtr pv = pvObject.getPvStructurePtr();
        epics::pvData::PVFieldPtr pvFrom = pv->getSubField(key);
        epics::pvData::PVUnionPtr pvUnion = epics::pvData::getPVDataCreate()->createPVUnion(unionPtr);
        setUnionField(pvFrom, pvUnion);
        data[i] = pvUnion;
    }
    pvUnionArrayPtr->replace(freeze(data));
}

//
// Private helper methods.
//
epics::pvData::StructureConstPtr PvObject::createStructureFromDict(const boost::python::dict& pyDict, const std::string& structureId)
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

epics::pvData::UnionConstPtr PvObject::createUnionFromDict(const boost::python::dict& pyDict, const std::string& structureId)
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

void PvObject::updateFieldArrayFromDict(const boost::python::dict& pyDict, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names)
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

        // Check for Scalar/Union
        boost::python::object valueObject = pyDict[fieldNameObject];
        boost::python::extract<int> scalarExtract(valueObject);
        if (scalarExtract.check()) {
            int scalarExtractValue = scalarExtract();
            switch (scalarExtractValue) {
                case PvType::UNION: {
                    PyPvDataUtility::addUnionField(fieldName, pyDict, fields, names);
                    break;
                }
                case PvType::VARIANT: {
                    PyPvDataUtility::addVariantUnionField(fieldName, fields, names);
                    break;
                }
                default: {
                    epics::pvData::ScalarType scalarType = static_cast<epics::pvData::ScalarType>(scalarExtractValue);
                    PyPvDataUtility::addScalarField(fieldName, scalarType, fields, names);
                }
            }
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
            else {
                // [Scalar]|[Union] => ScalarArray|UnionArray
                boost::python::extract<int> arrayScalarExtract(pyList[0]);
                if (arrayScalarExtract.check()) {
                    int arrayScalarExtractValue = arrayScalarExtract();
                    switch (arrayScalarExtractValue) {
                        case PvType::UNION: {
                            PyPvDataUtility::addUnionArrayField(fieldName, pyDict, fields, names);
                            break;
                        }
                        case PvType::VARIANT: {
                            PyPvDataUtility::addVariantUnionArrayField(fieldName, fields, names);
                            break;
                        }
                        default: {
                            epics::pvData::ScalarType scalarType = static_cast<epics::pvData::ScalarType>(arrayScalarExtractValue);
                            PyPvDataUtility::addScalarArrayField(fieldName, scalarType, fields, names);
                        }
                    }
                    continue;
                }

                // [{}] => StructureArray
                boost::python::extract<boost::python::dict> dictExtract(pyList[0]);
                if (dictExtract.check()) {
                    boost::python::dict pyDict2 = dictExtract();
                    int dictSize = boost::python::len(pyDict2);
                    if (!dictSize) {
                        throw InvalidArgument("PV type dict provided for field name %s must be non-empty.", fieldName.c_str());
                    }
                    PyPvDataUtility::addStructureArrayField(fieldName, pyDict2, fields, names);
                    continue;
                }

                // [PvObject] => StructureArray
                boost::python::extract<PvObject> pvObjectExtract(pyList[0]);
                if (pvObjectExtract.check()) {
                    PvObject pvObject = pvObjectExtract();
                    boost::python::dict pyDict2 = pvObject.getStructureDict();
                    int dictSize = boost::python::len(pyDict2);
                    if (!dictSize) {
                        throw InvalidArgument("PV object dict provided for field name %s must be non-empty.", fieldName.c_str());
                    }
                    PvType::DataType dataType = pvObject.getDataType();
                    switch (dataType) {
                        case PvType::Union: {
                             PyPvDataUtility::addUnionArrayField(fieldName, pyDict2, fields, names);
                            break;
                        }
                        case PvType::Variant: {
                            PyPvDataUtility::addVariantUnionArrayField(fieldName, fields, names);
                            break;
                        }
                        default: {
                            PyPvDataUtility::addStructureArrayField(fieldName, pyDict2, fields, names);
                        }
                    }
                    continue;
                }
                else {
                    // Invalid request.
                    throw InvalidArgument("Unrecognized list type for field name %s", fieldName.c_str());
                }
            }
            continue;
        } 

        // Check for dict: {} => Structure
        boost::python::extract<boost::python::dict> dictExtract(valueObject);
        if (dictExtract.check()) {
            boost::python::dict pyDict2 = dictExtract();
            int dictSize = boost::python::len(pyDict2);
            if (!dictSize) {
                throw InvalidArgument("PV type dict provided for field name %s must be non-empty.", fieldName.c_str());
            }
            PyPvDataUtility::addStructureField(fieldName, pyDict2, fields, names);
            continue;
        }

        // Check for PvObject: PvObject => Structure
        boost::python::extract<PvObject> pvObjectExtract(valueObject);
        if (pvObjectExtract.check()) {
            PvObject pvObject = pvObjectExtract();
            boost::python::dict pyDict2 = pvObject.getStructureDict();
            int dictSize = boost::python::len(pyDict2);
            if (!dictSize) {
                throw InvalidArgument("PV object dict provided for field name %s must be non-empty.", fieldName.c_str());
            }
            PvType::DataType dataType = pvObject.getDataType();
            switch (dataType) {
                case PvType::Union: {
                    PyPvDataUtility::addUnionField(fieldName, pyDict2, fields, names);
                    break;
                }
                case PvType::Variant: {
                    PyPvDataUtility::addVariantUnionField(fieldName, fields, names);
                    break;
                }
                default: {
                    PyPvDataUtility::addStructureField(fieldName, pyDict2, fields, names);
                }
            }
            continue;
        }
        else {
            // Invalid request.
            throw InvalidArgument("Unrecognized structure type for field name %s", fieldName.c_str());
        }
    }
}

void PvObject::setUnionField(const epics::pvData::PVFieldPtr& pvFrom, epics::pvData::PVUnionPtr pvUnionPtr)
{
    if(pvUnionPtr->getUnion()->isVariant()) {
        pvUnionPtr->set(pvFrom);
    } 
    else {
        epics::pvData::FieldConstPtr field = pvFrom->getField();
        int fieldIndex = -1;
        epics::pvData::FieldConstPtrArray fields = pvUnionPtr->getUnion()->getFields();
        for (size_t i = 0; i < fields.size(); ++i) {
            if (fields[i]==field) {
                fieldIndex = i;
                break;
            }
        }
        if (fieldIndex < 0) {
            throw InvalidArgument("Illegal union value.");
        }
        pvUnionPtr->set(fieldIndex, pvFrom);
    }
}
