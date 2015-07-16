#include "PvLong.h"
#include "PvType.h"
#include "PyUtility.h"

boost::python::dict PvLong::createStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = PvType::Long;
    return pyDict;
}

PvLong::PvLong()
    : PvScalar(createStructureDict())
{
    set(0);
}

PvLong::PvLong(long long ll)
    : PvScalar(createStructureDict())
{
    set(ll);
}

PvLong::~PvLong()
{
}

void PvLong::set(long long ll) 
{
    pvStructurePtr->getSubField<epics::pvData::PVLong>(ValueFieldKey)->put(ll);
}

long long PvLong::get() const 
{
    return pvStructurePtr->getSubField<epics::pvData::PVLong>(ValueFieldKey)->get();
}


