#include "PvFloat.h"
#include "PvType.h"
#include "PyUtility.h"

boost::python::dict PvFloat::createStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = PvType::Float;
    return pyDict;
}

PvFloat::PvFloat()
    : PvScalar(createStructureDict())
{
    set(0);
}

PvFloat::PvFloat(float f)
    : PvScalar(createStructureDict())
{
    set(f);
}

PvFloat::~PvFloat()
{
}

void PvFloat::set(float f) 
{
    pvStructurePtr->getFloatField(ValueFieldKey)->put(f);
}

float PvFloat::get() const 
{
    return pvStructurePtr->getFloatField(ValueFieldKey)->get();
}


