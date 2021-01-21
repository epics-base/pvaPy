// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PV_CONTROL_H
#define PV_CONTROL_H

#include "boost/python/dict.hpp"
#include "PvObject.h"

class PvControl : public PvObject
{
public:
    // Constants
    static const char* StructureId;

    static const char* LimitLowFieldKey;
    static const char* LimitHighFieldKey;
    static const char* MinStepFieldKey;

    // Static methods
    static boost::python::dict createStructureDict();

    // Instance methods
    PvControl();
    PvControl(double limitLow, double limitHigh, double minStep);
    PvControl(const boost::python::dict& pyDict, const std::string& structureId=StructureId);
    PvControl(const epics::pvData::PVStructurePtr& pvStructurePtr);
    PvControl(const PvControl& pvCOntrol); 
    virtual ~PvControl();

    virtual void setLimitLow(double value);
    virtual double getLimitLow() const;
    virtual void setLimitHigh(double value);
    virtual double getLimitHigh() const;
    virtual void setMinStep(double value);
    virtual double getMinStep() const;

};


#endif
