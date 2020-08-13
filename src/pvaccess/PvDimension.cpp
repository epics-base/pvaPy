// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvDimension.h"
#include "PvType.h"

namespace pvd = epics::pvData;
namespace bp = boost::python;

const char* PvDimension::StructureId("dimension_t");

const char* PvDimension::SizeFieldKey("size");
const char* PvDimension::OffsetFieldKey("offset");
const char* PvDimension::FullSizeFieldKey("fullSize");
const char* PvDimension::BinningFieldKey("binning");
const char* PvDimension::ReverseFieldKey("reverse");

bp::dict PvDimension::createStructureDict()
{
    bp::dict structureDict;
    structureDict[SizeFieldKey] = PvType::Int;
    structureDict[OffsetFieldKey] = PvType::Int;
    structureDict[FullSizeFieldKey] = PvType::Int;
    structureDict[BinningFieldKey] = PvType::Int;
    structureDict[ReverseFieldKey] = PvType::Boolean;
    return structureDict;
}

PvDimension::PvDimension()
    : PvObject(createStructureDict(), StructureId)
{
}

PvDimension::PvDimension(int size, int offset, int fullSize, int binning, bool reverse)
    : PvObject(createStructureDict(), StructureId)
{
    setSize(size);
    setOffset(offset);
    setFullSize(fullSize);
    setBinning(binning);
    setReverse(reverse);
}

PvDimension::PvDimension(const boost::python::dict& structureDict, const std::string& structureId)
    : PvObject(structureDict, structureId)
{
}

PvDimension::PvDimension(const PvDimension& pvDimension)
    : PvObject(pvDimension.pvStructurePtr)
{
}

PvDimension::PvDimension(const epics::pvData::PVStructurePtr& pvStructurePtr)
    : PvObject(pvStructurePtr)
{
}

PvDimension::~PvDimension()
{
}

void PvDimension::setSize(int value)
{
    pvStructurePtr->getSubField<pvd::PVInt>(SizeFieldKey)->put(value);
}

int PvDimension::getSize() const
{
    return pvStructurePtr->getSubField<pvd::PVInt>(SizeFieldKey)->get();
}

void PvDimension::setOffset(int value)
{
    pvStructurePtr->getSubField<pvd::PVInt>(OffsetFieldKey)->put(value);
}

int PvDimension::getOffset() const
{
    return pvStructurePtr->getSubField<pvd::PVInt>(OffsetFieldKey)->get();
}

void PvDimension::setFullSize(int value)
{
    pvStructurePtr->getSubField<pvd::PVInt>(FullSizeFieldKey)->put(value);
}

int PvDimension::getFullSize() const
{
    return pvStructurePtr->getSubField<pvd::PVInt>(FullSizeFieldKey)->get();
}

void PvDimension::setBinning(int value)
{
    pvStructurePtr->getSubField<pvd::PVInt>(BinningFieldKey)->put(value);
}

int PvDimension::getBinning() const
{
    return pvStructurePtr->getSubField<pvd::PVInt>(BinningFieldKey)->get();
}

void PvDimension::setReverse(bool value)
{
    pvStructurePtr->getSubField<pvd::PVBoolean>(ReverseFieldKey)->put(value);
}

bool PvDimension::getReverse() const
{
    return pvStructurePtr->getSubField<pvd::PVBoolean>(ReverseFieldKey)->get();
}



