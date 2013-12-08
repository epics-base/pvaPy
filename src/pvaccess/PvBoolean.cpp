#include "PvBoolean.h"
#include "PvType.h"
#include "PyUtility.h"

boost::python::dict PvBoolean::createStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = PvType::Boolean;
    return pyDict;
}

PvBoolean::PvBoolean()
    : PvScalar(createStructureDict())
{
    set(0);
}

PvBoolean::PvBoolean(int i)
    : PvScalar(createStructureDict())
{
    set(i);
}

PvBoolean::~PvBoolean()
{
}

void PvBoolean::set(bool b) 
{
    pvStructurePtr->getBooleanField(ValueFieldKey)->put(b);
}

bool PvBoolean::get() const 
{
    return pvStructurePtr->getBooleanField(ValueFieldKey)->get();
}


