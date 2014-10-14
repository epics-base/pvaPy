#ifndef PV_TIME_STAMP_H
#define PV_TIME_STAMP_H

#include "boost/python/dict.hpp"
#include "PvObject.h"

class PvTimeStamp : public PvObject
{
public:
    // Constants
    static const char* StructureId;

    static const char* SecondsPastEpochFieldKey;
    static const char* NanosecondsFieldKey;
    static const char* UserTagFieldKey;

    static const int UnknownUserTag;

    // Static methods
    static boost::python::dict createStructureDict();

    // Instance methods
    PvTimeStamp();
    PvTimeStamp(long long secondsPastEpoch, int nanoseconds);
    PvTimeStamp(long long secondsPastEpoch, int nanoseconds, int userTag);
    PvTimeStamp(const boost::python::dict& pyDict, const std::string& structureId=StructureId);
    PvTimeStamp(const epics::pvData::PVStructurePtr& pvStructurePtr);
    PvTimeStamp(const PvTimeStamp& pvTimeStamp); 
    virtual ~PvTimeStamp();

    virtual void setSecondsPastEpoch(long long secondsPastEpoch);
    virtual long long getSecondsPastEpoch() const;
    virtual void setNanoseconds(int nanoseconds);
    virtual int getNanoseconds() const;
    virtual void setUserTag(int userTag);
    virtual int getUserTag() const;

};


#endif
