// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PV_DISPLAY_H
#define PV_DISPLAY_H

#include "boost/python/dict.hpp"
#include "PvObject.h"

class PvDisplay : public PvObject
{
public:
    // Constants
    static const char* StructureId;

    static const char* LimitLowFieldKey;
    static const char* LimitHighFieldKey;
    static const char* DescriptionFieldKey;
    static const char* FormatFieldKey;
    static const char* UnitsFieldKey;

    // Static methods
    static boost::python::dict createStructureDict();

    // Instance methods
    PvDisplay();
    PvDisplay(double limitLow, double limitHigh, const std::string& description, const std::string& format, const std::string& units);
    PvDisplay(const boost::python::dict& pyDict, const std::string& structureId=StructureId);
    PvDisplay(const epics::pvData::PVStructurePtr& pvStructurePtr);
    PvDisplay(const PvDisplay& pvDisplay); 
    virtual ~PvDisplay();

    virtual void setLimitLow(double value);
    virtual double getLimitLow() const;
    virtual void setLimitHigh(double value);
    virtual double getLimitHigh() const;
    virtual void setDescription(const std::string& description);
    virtual std::string getDescription() const;
    virtual void setFormat(const std::string& format);
    virtual std::string getFormat() const;
    virtual void setUnits(const std::string& units);
    virtual std::string getUnits() const;

};


#endif
