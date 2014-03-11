#include "NtType.h"

NtType::NtType(const boost::python::dict& pyDict)
    : PvObject(pyDict)
{
}

NtType::NtType(const NtType& ntType)
    : PvObject(ntType.pvStructurePtr)
{
}

NtType::NtType(const epics::pvData::PVStructurePtr& pvStructurePtr)
    : PvObject(pvStructurePtr)
{
}

NtType::~NtType()
{
}

