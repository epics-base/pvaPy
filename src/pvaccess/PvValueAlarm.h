// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PV_VALUE_ALARM_H
#define PV_VALUE_ALARM_H

#include "boost/python/object.hpp"
#include "boost/python/dict.hpp"
#include "PvType.h"
#include "PvObject.h"

class PvValueAlarm : public PvObject
{
public:
    // Constants
    static const char* StructureId;

    static const char* ActiveFieldKey;
    static const char* LowAlarmLimitFieldKey;
    static const char* LowWarningLimitFieldKey;
    static const char* HighWarningLimitFieldKey;
    static const char* HighAlarmLimitFieldKey;
    static const char* LowAlarmSeverityFieldKey;
    static const char* LowWarningSeverityFieldKey;
    static const char* HighWarningSeverityFieldKey;
    static const char* HighAlarmSeverityFieldKey;
    static const char* HysteresisFieldKey;

    // Static methods
    static boost::python::dict createStructureDict(PvType::ScalarType scalarType);

    // Instance methods
    PvValueAlarm(PvType::ScalarType scalarType);
    PvValueAlarm(const boost::python::dict& pyDict, const std::string& structureId=StructureId);
    PvValueAlarm(const epics::pvData::PVStructurePtr& pvStructurePtr);
    PvValueAlarm(const PvValueAlarm& pvAlarm); 
    virtual ~PvValueAlarm();

    virtual void setActive(bool value);
    virtual bool getActive() const;

    virtual void setLowAlarmLimit(const boost::python::object& value);
    virtual boost::python::object getLowAlarmLimit() const;
    virtual void setLowWarningLimit(const boost::python::object& value);
    virtual boost::python::object getLowWarningLimit() const;
    virtual void setHighWarningLimit(const boost::python::object& value);
    virtual boost::python::object getHighWarningLimit() const;
    virtual void setHighAlarmLimit(const boost::python::object& value);
    virtual boost::python::object getHighAlarmLimit() const;

    virtual void setLowAlarmSeverity(int value);
    virtual int getLowAlarmSeverity() const;
    virtual void setLowWarningSeverity(int value);
    virtual int getLowWarningSeverity() const;
    virtual void setHighWarningSeverity(int value);
    virtual int getHighWarningSeverity() const;
    virtual void setHighAlarmSeverity(int value);
    virtual int getHighAlarmSeverity() const;

    virtual void setHysteresis(char value);
    virtual char getHysteresis() const;

};


#endif
