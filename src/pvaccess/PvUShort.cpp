#include "PvUShort.h"
#include "PvType.h"
#include "PyUtility.h"

boost::python::dict PvUShort::createStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = PvType::UShort;
    return pyDict;
}

PvUShort::PvUShort()
    : PvScalar(createStructureDict())
{
    set(0);
}

PvUShort::PvUShort(unsigned short us)
    : PvScalar(createStructureDict())
{
    set(us);
}

PvUShort::~PvUShort()
{
}

void PvUShort::set(unsigned short us) 
{
    pvStructurePtr->getUShortField(ValueFieldKey)->put(us);
}

unsigned short PvUShort::get() const 
{
    return pvStructurePtr->getUShortField(ValueFieldKey)->get();
}


