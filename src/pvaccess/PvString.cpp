// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvString.h"
#include "PvType.h"
#include "PyUtility.h"

boost::python::dict PvString::createStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = PvType::String;
    return pyDict;
}

PvString::PvString()
    : PvScalar(createStructureDict())
{
    set(0);
}

PvString::PvString(const std::string& s)
    : PvScalar(createStructureDict())
{
    set(s);
}

PvString::~PvString()
{
}

void PvString::set(const std::string& s) 
{
    pvStructurePtr->getSubField<epics::pvData::PVString>(ValueFieldKey)->put(s);
}

std::string PvString::get() const 
{
    return pvStructurePtr->getSubField<epics::pvData::PVString>(ValueFieldKey)->get();
}


