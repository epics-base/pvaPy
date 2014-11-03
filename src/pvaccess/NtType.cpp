#include "NtType.h"

const char* NtType::StructureId("epics:nt/NTType:1.0");

NtType::NtType(const boost::python::dict& pyDict, const std::string& structureId)
    : PvObject(pyDict, structureId)
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

