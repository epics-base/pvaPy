// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

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
    pvStructurePtr->getSubField<epics::pvData::PVUShort>(ValueFieldKey)->put(us);
}

unsigned short PvUShort::get() const 
{
    return pvStructurePtr->getSubField<epics::pvData::PVUShort>(ValueFieldKey)->get();
}


