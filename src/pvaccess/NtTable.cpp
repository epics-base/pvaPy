// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python.hpp"
#include "NtTable.h"
#include "StringUtility.h"
#include "PyPvDataUtility.h"
#include "InvalidArgument.h"

namespace pvd = epics::pvData;
namespace bp = boost::python;

const char* NtTable::StructureId("epics:nt/NTTable:1.0");
const char* NtTable::LabelsFieldKey("labels");

bp::dict NtTable::createStructureDict(int nColumns, PvType::ScalarType scalarType)
{
    if (nColumns < 0) {
        throw InvalidArgument("Number of columns cannot be negative.");
    }
    bp::list pyList;
    pyList.append(PvType::String);
    bp::dict pyDict;
    pyDict[LabelsFieldKey] = pyList;
    bp::dict pyDict2;
    for (int column = 0; column < nColumns; column++) {
        std::string columnName = getColumnName(column);
        bp::list pyList2;
        pyList2.append(scalarType);
        pyDict2[columnName] = pyList2;
    }
    pyDict[ValueFieldKey] = pyDict2;
    pyDict[DescriptorFieldKey] = PvType::String;
    pyDict[AlarmFieldKey] = PvAlarm::createStructureDict();
    pyDict[TimeStampFieldKey] = PvTimeStamp::createStructureDict();
    return pyDict;
}

bp::dict NtTable::createStructureDict(const bp::list& scalarTypePyList)
{
    bp::list pyList;
    pyList.append(PvType::String);
    bp::dict pyDict;
    pyDict[LabelsFieldKey] = pyList;
    bp::dict pyDict2;
    for (int column = 0; column < bp::len(scalarTypePyList); column++) {
        bp::extract<int> scalarTypeExtract(scalarTypePyList[column]);
        if (scalarTypeExtract.check()) {
            PvType::ScalarType scalarType = static_cast<PvType::ScalarType>(scalarTypeExtract());
            bp::list pyList2;
            pyList2.append(scalarType);
            std::string columnName = getColumnName(column);
            pyDict2[columnName] = pyList2;
        }
        else {
            throw InvalidArgument("Element list must be valid PV scalar type.");
        }
    }
    pyDict[ValueFieldKey] = pyDict2;
    pyDict[DescriptorFieldKey] = PvType::String;
    pyDict[TimeStampFieldKey] = PvTimeStamp::createStructureDict();
    pyDict[AlarmFieldKey] = PvAlarm::createStructureDict();
    return pyDict;
}

bp::dict NtTable::createStructureFieldIdDict()
{
    bp::dict structureFieldIdDict;
    structureFieldIdDict[AlarmFieldKey] = PvAlarm::StructureId;
    structureFieldIdDict[TimeStampFieldKey] = PvTimeStamp::StructureId;
    return structureFieldIdDict;
}

std::string NtTable::getColumnName(int column) 
{
    std::string columnName = "column" + StringUtility::toString(column);
    return columnName;
}

NtTable::NtTable(int nColumns_, PvType::ScalarType scalarType)
    : NtType(createStructureDict(nColumns_, scalarType), StructureId, createStructureFieldIdDict()),
    nColumns(nColumns_)
{
}

NtTable::NtTable(const bp::list& scalarTypePyList)
    : NtType(createStructureDict(scalarTypePyList), StructureId, createStructureFieldIdDict()),
    nColumns(bp::len(scalarTypePyList))
{
}

NtTable::NtTable(const PvObject& pvObject)
    : NtType(pvObject.getPvStructurePtr()),
    nColumns(0)
{
    PyPvDataUtility::checkFieldExists(LabelsFieldKey, pvStructurePtr);
    PyPvDataUtility::checkFieldExists(ValueFieldKey, pvStructurePtr);
    set(pvObject);
    pvd::PVScalarArrayPtr pvScalarArrayPtr = PyPvDataUtility::getScalarArrayField(LabelsFieldKey, pvd::pvString, pvStructurePtr);
    nColumns = pvScalarArrayPtr->getLength();
}

NtTable::NtTable(const NtTable& ntTable)
    : NtType(ntTable.pvStructurePtr)
{
}

NtTable::~NtTable()
{
}

int NtTable::getNColumns() const 
{
    return nColumns;
}

void NtTable::setLabels(const bp::list& pyList)
{
    if (bp::len(pyList) != nColumns) {
        throw InvalidArgument("Number of column labels must be %d.", nColumns);
    }
    PyPvDataUtility::pyListToScalarArrayField(pyList, LabelsFieldKey, pvStructurePtr);
}

void NtTable::setColumn(int column, const bp::list& pyList)
{
    if (column < 0 || column >= nColumns) {
        throw InvalidArgument("Column index must be in range [0,%d].", nColumns-1);
    }
    std::string columnName = getColumnName(column);
    pvd::PVStructurePtr pvStructurePtr2 = PyPvDataUtility::getStructureField(ValueFieldKey, pvStructurePtr);
    PyPvDataUtility::pyListToScalarArrayField(pyList, columnName, pvStructurePtr2);
}

bp::list NtTable::getLabels() const
{
    bp::list pyList;
    PyPvDataUtility::scalarArrayFieldToPyList(LabelsFieldKey, pvStructurePtr, pyList);
    return pyList;
}

bp::list NtTable::getColumn(int column) const
{
    if (column < 0 || column >= nColumns) {
        throw InvalidArgument("Column index must be in range [0,%d].", nColumns-1);
    }
    std::string columnName = getColumnName(column);
    bp::list pyList;
    pvd::PVStructurePtr pvStructurePtr2 = PyPvDataUtility::getStructureField(ValueFieldKey, pvStructurePtr);
    PyPvDataUtility::scalarArrayFieldToPyList(columnName, pvStructurePtr2, pyList);
    return pyList;
}
void NtTable::setDescriptor(const std::string& descriptor)
{
    pvStructurePtr->getSubField<pvd::PVString>(DescriptorFieldKey)->put(descriptor);
}

std::string NtTable::getDescriptor() const
{
    return pvStructurePtr->getSubField<pvd::PVString>(DescriptorFieldKey)->get();
}

PvTimeStamp NtTable::getTimeStamp() const
{
    return PvTimeStamp(PyPvDataUtility::getStructureField(TimeStampFieldKey, pvStructurePtr));
}

void NtTable::setTimeStamp(const PvTimeStamp& pvTimeStamp)
{
    PyPvDataUtility::pyDictToStructureField(pvTimeStamp, TimeStampFieldKey, pvStructurePtr);
}

PvAlarm NtTable::getAlarm() const
{
    return PvAlarm(PyPvDataUtility::getStructureField(AlarmFieldKey, pvStructurePtr));
}

void NtTable::setAlarm(const PvAlarm& pvAlarm)
{
    PyPvDataUtility::pyDictToStructureField(pvAlarm, AlarmFieldKey, pvStructurePtr);
}

