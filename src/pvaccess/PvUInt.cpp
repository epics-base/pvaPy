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
    pvStructurePtr->getUIntField(ValueFieldKey)->put(ui);
}

unsigned int PvUInt::get() const 
{
    return pvStructurePtr->getUIntField(ValueFieldKey)->get();
}


