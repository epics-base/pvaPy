// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "NtType.h"

namespace bp = boost::python;

const char* NtType::StructureId("epics:nt/NTType:1.0");

NtType::NtType(const bp::dict& pyDict, const std::string& structureId, const bp::dict& structureFieldIdDict)
    : PvObject(pyDict, structureId, structureFieldIdDict)
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

