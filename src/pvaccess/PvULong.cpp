#include "PvULong.h"
#include "PvType.h"
#include "PyUtility.h"

boost::python::dict PvULong::createStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = PvType::ULong;
    return pyDict;
}

PvULong::PvULong()
    : PvScalar(createStructureDict())
{
    set(0);
}

PvULong::PvULong(unsigned long long ull)
    : PvScalar(createStructureDict())
{
    set(ull);
}

PvULong::~PvULong()
{
}

void PvULong::set(unsigned long long ull) 
{
    pvStructurePtr->getSubField<epics::pvData::PVULong>(ValueFieldKey)->put(ull);
}

unsigned long long PvULong::get() const 
{
    return pvStructurePtr->getSubField<epics::pvData::PVULong>(ValueFieldKey)->get();
}


