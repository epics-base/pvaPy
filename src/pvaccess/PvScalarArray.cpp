#include "PvScalarArray.h"
#include "PyPvDataUtility.h"

boost::python::dict PvScalarArray::createStructureDict(PvType::ScalarType scalarType)
{
    boost::python::list pyList;
    pyList.append(scalarType);
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = pyList;
    return pyDict;
}

PvScalarArray::PvScalarArray(PvType::ScalarType scalarType)
    : PvObject(createStructureDict(scalarType))
{
}

PvScalarArray::PvScalarArray(const PvScalarArray& pvScalarArray)
    : PvObject(pvScalarArray.pvStructurePtr)
{
}

PvScalarArray::~PvScalarArray()
{
}

PvScalarArray::operator boost::python::list() const
{
    return toList();
}

boost::python::list PvScalarArray::toList() const
{
    boost::python::list pyList;
    PyPvDataUtility::scalarArrayFieldToPyList(ValueFieldKey, pvStructurePtr, pyList);
    return pyList;
}

void PvScalarArray::set(const boost::python::list& pyList)
{
    PyPvDataUtility::pyListToScalarArrayField(pyList, ValueFieldKey, pvStructurePtr);
}

boost::python::list PvScalarArray::get() const
{
    return toList();
}

