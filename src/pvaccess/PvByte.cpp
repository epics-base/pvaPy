// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvByte.h"
#include "PvType.h"
#include "PyUtility.h"

boost::python::dict PvByte::createStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = PvType::Byte;
    return pyDict;
}

PvByte::PvByte()
    : PvScalar(createStructureDict())
{
    set(0);
}

PvByte::PvByte(char c)
    : PvScalar(createStructureDict())
{
    set(c);
}

PvByte::~PvByte()
{
}

void PvByte::set(char c) 
{
    pvStructurePtr->getSubField<epics::pvData::PVByte>(ValueFieldKey)->put(c);
}

char PvByte::get() const 
{
    return pvStructurePtr->getSubField<epics::pvData::PVByte>(ValueFieldKey)->get();
}


