// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvUByte.h"
#include "PvType.h"
#include "PyUtility.h"

boost::python::dict PvUByte::createStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = PvType::UByte;
    return pyDict;
}

PvUByte::PvUByte()
    : PvScalar(createStructureDict())
{
    set(0);
}

PvUByte::PvUByte(unsigned char uc)
    : PvScalar(createStructureDict())
{
    set(uc);
}

PvUByte::~PvUByte()
{
}

void PvUByte::set(unsigned char uc) 
{
    pvStructurePtr->getSubField<epics::pvData::PVUByte>(ValueFieldKey)->put(uc);
}

unsigned char PvUByte::get() const 
{
    return pvStructurePtr->getSubField<epics::pvData::PVUByte>(ValueFieldKey)->get();
}


