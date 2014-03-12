#ifndef PV_ALARM_H
#define PV_ALARM_H

#include "boost/python/dict.hpp"
#include "PvObject.h"

class PvAlarm : public PvObject
{
public:
    // Constants
    static const char* StructureId;

    static const char* SeverityFieldKey;
    static const char* StatusFieldKey;
    static const char* MessageFieldKey;

    // Static methods
    static boost::python::dict createStructureDict();

    // Instance methods
    PvAlarm();
    PvAlarm(int severity, int status, const std::string& message);
    PvAlarm(const boost::python::dict& pyDict, const std::string& structureId=StructureId);
    PvAlarm(const epics::pvData::PVStructurePtr& pvStructurePtr);
    PvAlarm(const PvAlarm& pvAlarm); 
    virtual ~PvAlarm();

    virtual void setSeverity(int severity);
    virtual int getSeverity() const;
    virtual void setStatus(int status);
    virtual int getStatus() const;
    virtual void setMessage(const std::string& message);
    virtual std::string getMessage() const;

};


#endif
