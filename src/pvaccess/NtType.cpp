// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

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

