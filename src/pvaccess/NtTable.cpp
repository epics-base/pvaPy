#include "NtTable.h"
#include "StringUtility.h"
#include "PyPvDataUtility.h"

const char* NtTable::LabelsFieldKey("labels");

boost::python::dict NtTable::createStructureDict(int nColumns, PvType::ScalarType scalarType)
{
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
    return pyDict;
}

std::string NtTable::getColumnName(int column) 
{
    std::string columnName = "column" + StringUtility::toString(column);
    return columnName;
}

NtTable::NtTable(int nColumns, PvType::ScalarType scalarType)
    : NtType(createStructureDict(nColumns, scalarType))
{
}

NtTable::NtTable(const NtTable& ntTable)
    : NtType(ntTable.pvStructurePtr)
{
}

NtTable::~NtTable()
{
}

void NtTable::setLabels(const boost::python::list& pyList)
{
    PyPvDataUtility::pyListToScalarArrayField(pyList, LabelsFieldKey, pvStructurePtr);
}

void NtTable::setColumn(int column, const boost::python::list& pyList)
{
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
    std::string columnName = getColumnName(column);
    boost::python::list pyList;
    epics::pvData::PVStructurePtr pvStructurePtr2 = PyPvDataUtility::getStructureField(ValueFieldKey, pvStructurePtr);
    PyPvDataUtility::scalarArrayFieldToPyList(columnName, pvStructurePtr2, pyList);
    return pyList;
}

