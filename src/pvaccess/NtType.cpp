#include "NtType.h"

const char* NtType::StructureId("uri:ev4:nt/2012/pwd:NTType");

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

