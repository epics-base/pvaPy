// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvDisplay.h"
#include "PvType.h"


namespace pvd = epics::pvData;
namespace bp = boost::python;

const char* PvDisplay::StructureId("display_t");

const char* PvDisplay::LimitLowFieldKey("limitLow");
const char* PvDisplay::LimitHighFieldKey("limitHigh");
const char* PvDisplay::DescriptionFieldKey("description");
const char* PvDisplay::FormatFieldKey("format");
const char* PvDisplay::UnitsFieldKey("units");

boost::python::dict PvDisplay::createStructureDict()
{
    // StandardField implementation is using signed types for some reason.
    bp::dict structureDict;
    structureDict[LimitLowFieldKey] = PvType::Double;
    structureDict[LimitHighFieldKey] = PvType::Double;
    structureDict[DescriptionFieldKey] = PvType::String;
    structureDict[FormatFieldKey] = PvType::String;
    structureDict[UnitsFieldKey] = PvType::String;
    return structureDict;
}

PvDisplay::PvDisplay()
    : PvObject(createStructureDict(), StructureId)
{
}

PvDisplay::PvDisplay(double limitLow, double limitHigh, const std::string& description, const std::string& format, const std::string& units)
    : PvObject(createStructureDict(), StructureId)
{
    setLimitLow(limitLow);
    setLimitHigh(limitHigh);
    setDescription(description);
    setFormat(format);
    setUnits(units);
}

PvDisplay::PvDisplay(const boost::python::dict& pyDict, const std::string& structureId)
    : PvObject(pyDict, structureId)
{
}

PvDisplay::PvDisplay(const PvDisplay& pvDisplay)
    : PvObject(pvDisplay.pvStructurePtr)
{
}

PvDisplay::PvDisplay(const epics::pvData::PVStructurePtr& pvStructurePtr)
    : PvObject(pvStructurePtr)
{
}

PvDisplay::~PvDisplay()
{
}

void PvDisplay::setLimitLow(double value)
{
    pvStructurePtr->getSubField<epics::pvData::PVDouble>(LimitLowFieldKey)->put(value);
}

double PvDisplay::getLimitLow() const
{
    return pvStructurePtr->getSubField<epics::pvData::PVDouble>(LimitLowFieldKey)->get();
}

void PvDisplay::setLimitHigh(double value)
{
    pvStructurePtr->getSubField<epics::pvData::PVDouble>(LimitHighFieldKey)->put(value);
}

double PvDisplay::getLimitHigh() const
{
    return pvStructurePtr->getSubField<epics::pvData::PVDouble>(LimitHighFieldKey)->get();
}

void PvDisplay::setDescription(const std::string& description)
{
    pvStructurePtr->getSubField<epics::pvData::PVString>(DescriptionFieldKey)->put(description);
}

std::string PvDisplay::getDescription() const
{
    return pvStructurePtr->getSubField<epics::pvData::PVString>(DescriptionFieldKey)->get();
}

void PvDisplay::setFormat(const std::string& format)
{
    pvStructurePtr->getSubField<epics::pvData::PVString>(FormatFieldKey)->put(format);
}

std::string PvDisplay::getFormat() const
{
    return pvStructurePtr->getSubField<epics::pvData::PVString>(FormatFieldKey)->get();
}

void PvDisplay::setUnits(const std::string& units)
{
    pvStructurePtr->getSubField<epics::pvData::PVString>(UnitsFieldKey)->put(units);
}

std::string PvDisplay::getUnits() const
{
    return pvStructurePtr->getSubField<epics::pvData::PVString>(UnitsFieldKey)->get();
}

