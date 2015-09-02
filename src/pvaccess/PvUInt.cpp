// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvUInt.h"
#include "PvType.h"
#include "PyUtility.h"

boost::python::dict PvUInt::createStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = PvType::UInt;
    return pyDict;
}

PvUInt::PvUInt()
    : PvScalar(createStructureDict())
{
    set(0);
}

PvUInt::PvUInt(unsigned int ui)
    : PvScalar(createStructureDict())
{
    set(ui);
}

PvUInt::~PvUInt()
{
}

void PvUInt::set(unsigned int ui) 
{
    pvStructurePtr->getSubField<epics::pvData::PVUInt>(ValueFieldKey)->put(ui);
}

unsigned int PvUInt::get() const 
{
    return pvStructurePtr->getSubField<epics::pvData::PVUInt>(ValueFieldKey)->get();
}


