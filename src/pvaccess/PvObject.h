// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PV_OBJECT_H
#define PV_OBJECT_H

#include <iostream>
#include "pv/pvData.h"
#include "pvapy.environment.h"
#include "boost/python/dict.hpp"
#include "boost/python/list.hpp"

#include "PvType.h"


class PvObject 
{
public:

    // Constants
    static const char* ValueFieldKey;
    static const char* StructureId;
    static const bool UseNumPyArraysDefault;

    // Static methods
    static bool initializeBoostNumPy();
    static bool isPvObjectInstance(const boost::python::object& pyObject);
    static bool pvObjectToPyDict(const boost::python::object& pyObject, boost::python::object& pyDict);

    // Constructors
    PvObject(const epics::pvData::PVStructurePtr& pvStructurePtr);
    PvObject(const boost::python::dict& structureDict);
    PvObject(const boost::python::dict& structureDict, const std::string& structureId);
    PvObject(const boost::python::dict& structureDict, const std::string& structureId, const boost::python::dict& structureFieldIdDict);
    PvObject(const boost::python::dict& structureDict, const boost::python::dict& valueDict);
    PvObject(const boost::python::dict& structureDict, const boost::python::dict& valueDict, const std::string& structureId);
    PvObject(const PvObject& pvObject);

    // Destructor
    virtual ~PvObject();

    // Operators
    epics::pvData::PVStructurePtr getPvStructurePtr() const;
    epics::pvData::StructureConstPtr getStructurePtr() const;
    operator epics::pvData::PVStructurePtr();
    operator boost::python::dict() const;
    boost::python::dict toDict() const;
    boost::python::dict getStructureDict() const;
    PvType::DataType getDataType();
    friend std::ostream& operator<<(std::ostream& out, const PvObject& pvObject);
    friend epics::pvData::PVStructurePtr& operator<<(epics::pvData::PVStructurePtr& pvStructurePtr, const PvObject& pvObject);

    // Copy
    PvObject copy();

    // Dictionary methods
    bool has_key(const std::string& fieldPath) const;
    boost::python::list items() const;
    boost::python::list keys() const;
    boost::python::list values() const;

    // Has field?
    bool hasField(const std::string& fieldPath) const;

    // Object set/get
    void set(const boost::python::dict& pyDict);
    void set(const PvObject& pvObject);
    boost::python::dict get() const;

    void setPyObject(const std::string& fieldPath, const boost::python::object& pyObject);
    void setPyObject(const boost::python::object& pyObject);
    boost::python::object getPyObject(const std::string& fieldPath) const;
    boost::python::object getPyObject() const;
    std::string getAsString(const std::string& fieldPath) const;
    std::string getAsString() const;

    // Boolean fields
    void setBoolean(const std::string& key, bool value);
    void setBoolean(bool value);
    bool getBoolean(const std::string& key) const;
    bool getBoolean() const;

    // Byte fields
    void setByte(const std::string& key, char value);
    void setByte(char value);
    char getByte(const std::string& key) const;
    char getByte() const;

    // UByte fields
    void setUByte(const std::string& key, unsigned char value);
    void setUByte(unsigned char value);
    unsigned char getUByte(const std::string& key) const;
    unsigned char getUByte() const;

    // Short fields
    void setShort(const std::string& key, short value);
    void setShort(short value);
    short getShort(const std::string& key) const;
    short getShort() const;

    // UShort fields
    void setUShort(const std::string& key, unsigned short value);
    void setUShort(unsigned short value);
    unsigned short getUShort(const std::string& key) const;
    unsigned short getUShort() const;

    // Int fields
    void setInt(const std::string& key, int value);
    void setInt(int value);
    int getInt(const std::string& key) const;
    int getInt() const;

    // UInt fields
    void setUInt(const std::string& key, unsigned int value);
    void setUInt(unsigned int value);
    unsigned int getUInt(const std::string& key) const;
    unsigned int getUInt() const;

    // Long fields
    void setLong(const std::string& key, long long value);
    void setLong(long long value);
    long long getLong(const std::string& key) const;
    long long getLong() const;

    // ULong fields
    void setULong(const std::string& key, unsigned long long value);
    void setULong(unsigned long long value);
    unsigned long long getULong(const std::string& key) const;
    unsigned long long getULong() const;

    // Float fields
    void setFloat(const std::string& key, float value);
    void setFloat(float value);
    float getFloat(const std::string& key) const;
    float getFloat() const;

    // Double fields
    void setDouble(const std::string& key, double value);
    void setDouble(double value);
    double getDouble(const std::string& key) const;
    double getDouble() const;

    // String fields
    void setString(const std::string& key, const std::string& value);
    void setString(const std::string& value);
    std::string getString(const std::string& key) const;
    std::string getString() const;

    // Scalar array fields
    void setScalarArray(const std::string& key, const boost::python::object& pyObject);
    void setScalarArray(const boost::python::object& pyObject);
    boost::python::object getScalarArray(const std::string& key) const;
    boost::python::object getScalarArray() const;

    // Structure fields
    void setStructure(const std::string& key, const PvObject& pvObject);
    void setStructure(const PvObject& pvObject);
    void setStructure(const std::string& key, const boost::python::dict& pyDict);
    void setStructure(const boost::python::dict& pyDict);
    boost::python::dict getStructure(const std::string& key) const;
    boost::python::dict getStructure() const;

    // Structure array fields
    void setStructureArray(const std::string& key, const boost::python::list& pyList);
    void setStructureArray(const boost::python::list& pyList);
    boost::python::list getStructureArray(const std::string& key) const;
    boost::python::list getStructureArray() const;

    // Union fields
    void setUnion(const std::string& key, const PvObject& value);
    void setUnion(const PvObject& value);
    void setUnion(const std::string& key, const boost::python::dict& pyDict);
    void setUnion(const boost::python::dict& pyDict);
    void setUnion(const std::string& key, const boost::python::tuple& pyTuple);
    void setUnion(const boost::python::tuple& pyTuple);
    PvObject getUnion(const std::string& key) const;
    PvObject getUnion() const;

    boost::python::list getUnionFieldNames(const std::string& key) const;
    boost::python::list getUnionFieldNames() const;
    std::string getSelectedUnionFieldName(const std::string& key) const;
    std::string getSelectedUnionFieldName() const;
    PvObject selectUnionField(const std::string& key, const std::string& fieldName) const;
    PvObject selectUnionField(const std::string& fieldName) const;
    bool isUnionVariant(const std::string& key) const;
    bool isUnionVariant() const;
    PvObject createUnionField(const std::string& key, const std::string& fieldName) const;
    PvObject createUnionField(const std::string& fieldName) const;

    // UnionArray fields
    void setUnionArray(const std::string& key, const boost::python::list& pyList);
    void setUnionArray(const boost::python::list& pyList);
    boost::python::list getUnionArray(const std::string& key) const;
    boost::python::list getUnionArray() const;

    boost::python::list getUnionArrayFieldNames(const std::string& key) const;
    boost::python::list getUnionArrayFieldNames() const;
    bool isUnionArrayVariant(const std::string& key) const;
    bool isUnionArrayVariant() const;
    PvObject createUnionArrayElementField(const std::string& key, const std::string& fieldName) const;
    PvObject createUnionArrayElementField(const std::string& fieldName) const;

#if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1
    void setUseNumPyArraysFlag(bool useNumPyArrays);
    bool getUseNumPyArraysFlag() const;
#endif // if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1

#if PVA_API_VERSION >= 482
    std::string toJSON(bool multiLine = false);
#endif // if PVA_API_VERSION >= 482

protected:
    bool numPyInitialized;
    epics::pvData::PVStructurePtr pvStructurePtr;
    PvType::DataType dataType;
    bool useNumPyArrays; 

private:
    static bool boostNumPyInitialized;
};

#endif

