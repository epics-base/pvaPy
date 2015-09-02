// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

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
    pvStructurePtr->getSubField<epics::pvData::PVFloat>(ValueFieldKey)->put(f);
}

float PvFloat::get() const 
{
    return pvStructurePtr->getSubField<epics::pvData::PVFloat>(ValueFieldKey)->get();
}


