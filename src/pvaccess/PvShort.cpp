#include "PvShort.h"
#include "PvType.h"
#include "PyUtility.h"

boost::python::dict PvShort::createStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = PvType::Short;
    return pyDict;
}

PvShort::PvShort()
    : PvScalar(createStructureDict())
{
    set(0);
}

PvShort::PvShort(short s)
    : PvScalar(createStructureDict())
{
    set(s);
}

PvShort::~PvShort()
{
}

void PvShort::set(short s) 
{
    pvStructurePtr->getShortField(ValueFieldKey)->put(s);
}

short PvShort::get() const 
{
    return pvStructurePtr->getShortField(ValueFieldKey)->get();
}


