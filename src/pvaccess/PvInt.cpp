// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvInt.h"
#include "PvType.h"
#include "PyUtility.h"

boost::python::dict PvInt::createStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = PvType::Int;
    return pyDict;
}

PvInt::PvInt()
    : PvScalar(createStructureDict())
{
    set(0);
}

PvInt::PvInt(int i)
    : PvScalar(createStructureDict())
{
    set(i);
}

PvInt::~PvInt()
{
}

void PvInt::set(int i) 
{
    pvStructurePtr->getSubField<epics::pvData::PVInt>(ValueFieldKey)->put(i);
}

int PvInt::get() const 
{
    return pvStructurePtr->getSubField<epics::pvData::PVInt>(ValueFieldKey)->get();
}


