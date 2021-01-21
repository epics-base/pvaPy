// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvControl.h"
#include "PvType.h"


namespace epvd = epics::pvData;
namespace bp = boost::python;

const char* PvControl::StructureId("control_t");

const char* PvControl::LimitLowFieldKey("limitLow");
const char* PvControl::LimitHighFieldKey("limitHigh");
const char* PvControl::MinStepFieldKey("minStep");

bp::dict PvControl::createStructureDict()
{
    bp::dict structureDict;
    structureDict[LimitLowFieldKey] = PvType::Double;
    structureDict[LimitHighFieldKey] = PvType::Double;
    structureDict[MinStepFieldKey] = PvType::Double;
    return structureDict;
}

PvControl::PvControl()
    : PvObject(createStructureDict(), StructureId)
{
}

PvControl::PvControl(double limitLow, double limitHigh, double minStep)
    : PvObject(createStructureDict(), StructureId)
{
    setLimitLow(limitLow);
    setLimitHigh(limitHigh);
    setMinStep(minStep);
}

PvControl::PvControl(const boost::python::dict& pyDict, const std::string& structureId)
    : PvObject(pyDict, structureId)
{
}

PvControl::PvControl(const PvControl& pvControl)
    : PvObject(pvControl.pvStructurePtr)
{
}

PvControl::PvControl(const epvd::PVStructurePtr& pvStructurePtr)
    : PvObject(pvStructurePtr)
{
}

PvControl::~PvControl()
{
}

void PvControl::setLimitLow(double value)
{
    pvStructurePtr->getSubField<epvd::PVDouble>(LimitLowFieldKey)->put(value);
}

double PvControl::getLimitLow() const
{
    return pvStructurePtr->getSubField<epvd::PVDouble>(LimitLowFieldKey)->get();
}

void PvControl::setLimitHigh(double value)
{
    pvStructurePtr->getSubField<epvd::PVDouble>(LimitHighFieldKey)->put(value);
}

double PvControl::getLimitHigh() const
{
    return pvStructurePtr->getSubField<epvd::PVDouble>(LimitHighFieldKey)->get();
}

void PvControl::setMinStep(double value)
{
    pvStructurePtr->getSubField<epvd::PVDouble>(MinStepFieldKey)->put(value);
}

double PvControl::getMinStep() const
{
    return pvStructurePtr->getSubField<epvd::PVDouble>(MinStepFieldKey)->get();
}

