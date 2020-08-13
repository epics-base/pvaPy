// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvCodec.h"
#include "PvType.h"

namespace pvd = epics::pvData;
namespace bp = boost::python;

const char* PvCodec::StructureId("codec_t");

const char* PvCodec::NameFieldKey("name");
const char* PvCodec::ParametersFieldKey("parameters");

bp::dict PvCodec::createStructureDict()
{
    bp::dict structureDict;
    structureDict[NameFieldKey] = PvType::String;
    structureDict[ParametersFieldKey] = bp::make_tuple();
    return structureDict;
}

PvCodec::PvCodec()
    : PvObject(createStructureDict(), StructureId)
{
}

PvCodec::PvCodec(const std::string& name, const PvObject& parameters)
    : PvObject(createStructureDict(), StructureId)
{
    setName(name);
    setParameters(parameters);
}

PvCodec::PvCodec(const boost::python::dict& structureDict, const std::string& structureId)
    : PvObject(structureDict, structureId)
{
}

PvCodec::PvCodec(const PvCodec& pvCodec)
    : PvObject(pvCodec.pvStructurePtr)
{
}

PvCodec::PvCodec(const epics::pvData::PVStructurePtr& pvStructurePtr)
    : PvObject(pvStructurePtr)
{
}

PvCodec::~PvCodec()
{
}

void PvCodec::setName(const std::string& name)
{
    pvStructurePtr->getSubField<pvd::PVString>(NameFieldKey)->put(name);
}

std::string PvCodec::getName() const
{
    return pvStructurePtr->getSubField<pvd::PVString>(NameFieldKey)->get();
}

void PvCodec::setParameters(const PvObject& parameters)
{
    setUnion(ParametersFieldKey, parameters);
}

PvObject PvCodec::getParameters() const
{
    return getUnion(ParametersFieldKey);
}


