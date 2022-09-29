// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PV_TIME_STAMP_H
#define PV_TIME_STAMP_H

#include <boost/python/dict.hpp>
#include <epicsTime.h>
#include "PvObject.h"

class PvTimeStamp : public PvObject
{
public:
    // Constants
    static const double NanosecondsInSecond;

    static const char* StructureId;

    static const char* SecondsPastEpochFieldKey;
    static const char* NanosecondsFieldKey;
    static const char* UserTagFieldKey;

    static const int UnknownUserTag;

    // Static methods
    static boost::python::dict createStructureDict();
    static PvTimeStamp getCurrent();

    // Instance methods
    PvTimeStamp();
    PvTimeStamp(double time);
    PvTimeStamp(const epicsTimeStamp* ts);
    PvTimeStamp(const epicsTimeStamp& ts);
    PvTimeStamp(long long secondsPastEpoch, int nanoseconds);
    PvTimeStamp(long long secondsPastEpoch, int nanoseconds, int userTag);
    PvTimeStamp(const boost::python::dict& pyDict, const std::string& structureId=StructureId);
    PvTimeStamp(const epics::pvData::PVStructurePtr& pvStructurePtr);
    PvTimeStamp(const PvTimeStamp& pvTimeStamp); 
    virtual ~PvTimeStamp();

    virtual PvTimeStamp& operator=(const PvTimeStamp& pvTimeStamp);
    virtual operator double() const;
    virtual double operator-(const PvTimeStamp& t) const;
    virtual bool operator==(const PvTimeStamp& t) const;
    virtual bool operator!=(const PvTimeStamp& t) const;
    virtual void setSecondsPastEpoch(long long secondsPastEpoch);
    virtual long long getSecondsPastEpoch() const;
    virtual void setNanoseconds(int nanoseconds);
    virtual int getNanoseconds() const;
    virtual void setUserTag(int userTag);
    virtual int getUserTag() const;
};


#endif
