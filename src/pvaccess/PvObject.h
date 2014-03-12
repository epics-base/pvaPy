#ifndef PV_OBJECT_H
#define PV_OBJECT_H

#include <iostream>
#include "pv/pvData.h"
#include "boost/python/dict.hpp"
#include "boost/python/list.hpp"

class PvObject
{
public:

    // Constants
    static const char* ValueFieldKey;
    static const char* StructureId;

    // Constructors
    PvObject(const epics::pvData::PVStructurePtr& pvStructurePtr);
    PvObject(const boost::python::dict& pyDict, const std::string& structureId=StructureId);
    PvObject(const PvObject& pvObject);

    // Destructor
    virtual ~PvObject();

    // Operators
    epics::pvData::PVStructurePtr getPvStructurePtr() const;
    epics::pvData::StructureConstPtr getStructurePtr() const;
    operator epics::pvData::PVStructurePtr();
    operator boost::python::dict() const;
    boost::python::dict toDict() const;
    boost::python::dict getStructureDict();
    friend std::ostream& operator<<(std::ostream& out, const PvObject& pvObject);
    friend epics::pvData::PVStructurePtr& operator<<(epics::pvData::PVStructurePtr& pvStructurePtr, const PvObject& pvObject);

    // Object set/get
    void set(const boost::python::dict& pyDict);
    boost::python::dict get() const;

    // Boolean fields
    void setBoolean(const std::string& key, bool value);
    void setBoolean(bool value);
    bool getBoolean(const std::string& key=ValueFieldKey) const;

    // Byte fields
    void setByte(const std::string& key, char value);
    void setByte(char value);
    char getByte(const std::string& key=ValueFieldKey) const;

    // UByte fields
    void setUByte(const std::string& key, unsigned char value);
    void setUByte(unsigned char value);
    unsigned char getUByte(const std::string& key=ValueFieldKey) const;

    // Short fields
    void setShort(const std::string& key, short value);
    void setShort(short value);
    short getShort(const std::string& key=ValueFieldKey) const;

    // UShort fields
    void setUShort(const std::string& key, unsigned short value);
    void setUShort(unsigned short value);
    unsigned short getUShort(const std::string& key=ValueFieldKey) const;

    // Int fields
    void setInt(const std::string& key, int value);
    void setInt(int value);
    int getInt(const std::string& key=ValueFieldKey) const;

    // UInt fields
    void setUInt(const std::string& key, unsigned int value);
    void setUInt(unsigned int value);
    unsigned int getUInt(const std::string& key=ValueFieldKey) const;

    // Long fields
    void setLong(const std::string& key, long long value);
    void setLong(long long value);
    long long getLong(const std::string& key=ValueFieldKey) const;

    // ULong fields
    void setULong(const std::string& key, unsigned long long value);
    void setULong(unsigned long long value);
    unsigned long long getULong(const std::string& key=ValueFieldKey) const;

    // Float fields
    void setFloat(const std::string& key, float value);
    void setFloat(float value);
    float getFloat(const std::string& key=ValueFieldKey) const;

    // Double fields
    void setDouble(const std::string& key, double value);
    void setDouble(double value);
    double getDouble(const std::string& key=ValueFieldKey) const;

    // String fields
    void setString(const std::string& key, const std::string& value);
    void setString(const std::string& value);
    std::string getString(const std::string& key=ValueFieldKey) const;

    // Scalar array fields
    void setScalarArray(const std::string& key, const boost::python::list& pyList);
    void setScalarArray(const boost::python::list& pyList);
    boost::python::list getScalarArray(const std::string& key=ValueFieldKey) const;

    // Structure fields
    void setStructure(const std::string& key, const boost::python::dict& pyDict);
    void setStructure(const boost::python::dict& pyDict);
    boost::python::dict getStructure(const std::string& key=ValueFieldKey) const;

    // Structure array fields
    void setStructureArray(const std::string& key, const boost::python::list& pyList);
    void setStructureArray(const boost::python::list& pyList);
    boost::python::list getStructureArray(const std::string& key=ValueFieldKey) const;

protected:
    epics::pvData::PVStructurePtr pvStructurePtr;
private:

    // Static helper methods
    static epics::pvData::StructureConstPtr createStructureFromDict(const boost::python::dict& pyDict, const std::string& structureId="");
    static void addScalarField(const std::string& fieldName, epics::pvData::ScalarType scalarType, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names);
    static void addScalarArrayField(const std::string& fieldName, epics::pvData::ScalarType scalarType, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names);
    static void addStructureField(const std::string& fieldName, const boost::python::dict& pyDict, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names);
    static void addStructureArrayField(const std::string& fieldName, const boost::python::dict& pyDict, epics::pvData::FieldConstPtrArray& fields, epics::pvData::StringArray& names);

};

#endif

