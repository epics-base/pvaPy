#include "NtTable.h"
#include "StringUtility.h"
#include "PyPvDataUtility.h"
#include "InvalidArgument.h"

const char* NtTable::StructureId("epics:nt/NTTable:1.0");
const char* NtTable::LabelsFieldKey("labels");
const char* NtTable::DescriptorFieldKey("descriptor");
const char* NtTable::TimeStampFieldKey("timeStamp");
const char* NtTable::AlarmFieldKey("alarm");

boost::python::dict NtTable::createStructureDict(int nColumns, PvType::ScalarType scalarType)
{
    if (nColumns < 0) {
        throw InvalidArgument("Number of columns cannot be negative.");
    }
    boost::python::list pyList;
    pyList.append(PvType::String);
    boost::python::dict pyDict;
    pyDict[LabelsFieldKey] = pyList;
    boost::python::dict pyDict2;
    for (int column = 0; column < nColumns; column++) {
        std::string columnName = getColumnName(column);
        boost::python::list pyList2;
        pyList2.append(scalarType);
        pyDict2[columnName] = pyList2;
    }
    pyDict[ValueFieldKey] = pyDict2;
    pyDict[DescriptorFieldKey] = PvType::String;
    pyDict[TimeStampFieldKey] = PvTimeStamp::createStructureDict();
    pyDict[AlarmFieldKey] = PvAlarm::createStructureDict();
    return pyDict;
}

boost::python::dict NtTable::createStructureDict(const boost::python::list& scalarTypePyList)
{
    boost::python::list pyList;
    pyList.append(PvType::String);
    boost::python::dict pyDict;
    pyDict[LabelsFieldKey] = pyList;
    boost::python::dict pyDict2;
    for (int column = 0; column < boost::python::len(scalarTypePyList); column++) {
        boost::python::extract<int> scalarTypeExtract(scalarTypePyList[column]);
        if (scalarTypeExtract.check()) {
            PvType::ScalarType scalarType = static_cast<PvType::ScalarType>(scalarTypeExtract());
            boost::python::list pyList2;
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

std::string NtTable::getColumnName(int column) 
{
    std::string columnName = "column" + StringUtility::toString(column);
    return columnName;
}

NtTable::NtTable(int nColumns_, PvType::ScalarType scalarType)
    : NtType(createStructureDict(nColumns_, scalarType), StructureId),
    nColumns(nColumns_)
{
}

NtTable::NtTable(const boost::python::list& scalarTypePyList)
    : NtType(createStructureDict(scalarTypePyList), StructureId),
    nColumns(boost::python::len(scalarTypePyList))
{
}

NtTable::NtTable(const PvObject& pvObject)
    : NtType(pvObject.getPvStructurePtr()),
    nColumns(0)
{
    PyPvDataUtility::checkFieldExists(LabelsFieldKey, pvStructurePtr);
    PyPvDataUtility::checkFieldExists(ValueFieldKey, pvStructurePtr);
    set(pvObject);
    epics::pvData::PVScalarArrayPtr pvScalarArrayPtr = PyPvDataUtility::getScalarArrayField(LabelsFieldKey, epics::pvData::pvString, pvStructurePtr);
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

void NtTable::setLabels(const boost::python::list& pyList)
{
    if (boost::python::len(pyList) != nColumns) {
        throw InvalidArgument("Number of column labels must be %d.", nColumns);
    }
    PyPvDataUtility::pyListToScalarArrayField(pyList, LabelsFieldKey, pvStructurePtr);
}

void NtTable::setColumn(int column, const boost::python::list& pyList)
{
    if (column < 0 || column >= nColumns) {
        throw InvalidArgument("Column index must be in range [0,%d].", nColumns-1);
    }
    std::string columnName = getColumnName(column);
    epics::pvData::PVStructurePtr pvStructurePtr2 = PyPvDataUtility::getStructureField(ValueFieldKey, pvStructurePtr);
    PyPvDataUtility::pyListToScalarArrayField(pyList, columnName, pvStructurePtr2);
}

boost::python::list NtTable::getLabels() const
{
    boost::python::list pyList;
    PyPvDataUtility::scalarArrayFieldToPyList(LabelsFieldKey, pvStructurePtr, pyList);
    return pyList;
}

boost::python::list NtTable::getColumn(int column) const
{
    if (column < 0 || column >= nColumns) {
        throw InvalidArgument("Column index must be in range [0,%d].", nColumns-1);
    }
    std::string columnName = getColumnName(column);
    boost::python::list pyList;
    epics::pvData::PVStructurePtr pvStructurePtr2 = PyPvDataUtility::getStructureField(ValueFieldKey, pvStructurePtr);
    PyPvDataUtility::scalarArrayFieldToPyList(columnName, pvStructurePtr2, pyList);
    return pyList;
}
void NtTable::setDescriptor(const std::string& descriptor)
{
        pvStructurePtr->getStringField(DescriptorFieldKey)->put(descriptor);
}

std::string NtTable::getDescriptor() const
{
        return pvStructurePtr->getStringField(DescriptorFieldKey)->get();
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

