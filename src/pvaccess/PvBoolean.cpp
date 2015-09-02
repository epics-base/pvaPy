// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

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
    pvStructurePtr->getSubField<epics::pvData::PVBoolean>(ValueFieldKey)->put(b);
}

bool PvBoolean::get() const 
{
    return pvStructurePtr->getSubField<epics::pvData::PVBoolean>(ValueFieldKey)->get();
}


