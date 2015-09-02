// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvDouble.h"
#include "PvType.h"
#include "PyUtility.h"

boost::python::dict PvDouble::createStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = PvType::Double;
    return pyDict;
}

PvDouble::PvDouble()
    : PvScalar(createStructureDict())
{
    set(0);
}

PvDouble::PvDouble(double d)
    : PvScalar(createStructureDict())
{
    set(d);
}

PvDouble::~PvDouble()
{
}

void PvDouble::set(double d) 
{
    pvStructurePtr->getSubField<epics::pvData::PVDouble>(ValueFieldKey)->put(d);
}

double PvDouble::get() const 
{
    return pvStructurePtr->getSubField<epics::pvData::PVDouble>(ValueFieldKey)->get();
}


