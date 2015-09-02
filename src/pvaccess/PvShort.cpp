// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvShort.h"
#include "PvType.h"
#include "PyUtility.h"

boost::python::dict PvShort::createStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = PvType::Short;
    return pyDict;
}

PvShort::PvShort()
    : PvScalar(createStructureDict())
{
    set(0);
}

PvShort::PvShort(short s)
    : PvScalar(createStructureDict())
{
    set(s);
}

PvShort::~PvShort()
{
}

void PvShort::set(short s) 
{
    pvStructurePtr->getSubField<epics::pvData::PVShort>(ValueFieldKey)->put(s);
}

short PvShort::get() const 
{
    return pvStructurePtr->getSubField<epics::pvData::PVShort>(ValueFieldKey)->get();
}


