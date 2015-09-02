// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef NT_TABLE_H
#define NT_TABLE_H

#include <string>
#include "boost/python/dict.hpp"
#include "boost/python/list.hpp"
#include "PvObject.h"
#include "PvType.h"
#include "PvTimeStamp.h"
#include "PvAlarm.h"
#include "NtType.h"

class NtTable : public NtType
{
public:
    // Constants
    static const char* StructureId;

    static const char* LabelsFieldKey;
    static const char* DescriptorFieldKey;
    static const char* TimeStampFieldKey;
    static const char* AlarmFieldKey;

    // Static methods
    static std::string getColumnName(int column);
    static boost::python::dict createStructureDict(int nColumns, PvType::ScalarType scalarType);
    static boost::python::dict createStructureDict(const boost::python::list& scalarTypePyList);

    // Instance methods
    NtTable(int nColumns, PvType::ScalarType scalarType);
    NtTable(const boost::python::list& scalarTypePyList);
    NtTable(const PvObject& pvObject);
    NtTable(const NtTable& ntTable);
    virtual ~NtTable();

    virtual int getNColumns() const;
    virtual void setLabels(const boost::python::list& pyList);
    virtual boost::python::list getLabels() const;
    virtual void setColumn(int column, const boost::python::list& pyList);
    virtual boost::python::list getColumn(int column) const;
    virtual void setDescriptor(const std::string& descriptor);
    virtual std::string getDescriptor() const;
    virtual void setTimeStamp(const PvTimeStamp& pvTimeStamp);
    virtual PvTimeStamp getTimeStamp() const;
    virtual void setAlarm(const PvAlarm& pvAlarm);
    virtual PvAlarm getAlarm() const;
private:
    int nColumns;
};

#endif
