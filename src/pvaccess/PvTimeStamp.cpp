#include "PvTimeStamp.h"
#include "PvType.h"

const char* PvTimeStamp::StructureId("time_t");

const char* PvTimeStamp::SecondsPastEpochFieldKey("secondsPastEpoch");
const char* PvTimeStamp::NanosecondsFieldKey("nanoseconds");
const char* PvTimeStamp::UserTagFieldKey("userTag");

const int PvTimeStamp::UnknownUserTag(-1);

boost::python::dict PvTimeStamp::createStructureDict()
{
    // StandardField implementation is using signed types for some reason.
    boost::python::dict pyDict;
    pyDict[SecondsPastEpochFieldKey] = PvType::Long;
    pyDict[NanosecondsFieldKey] = PvType::Int;
    pyDict[UserTagFieldKey] = PvType::Int;
    return pyDict;
}

PvTimeStamp::PvTimeStamp()
    : PvObject(createStructureDict(), StructureId)
{
    setSecondsPastEpoch(0);
    setNanoseconds(0);
    setUserTag(UnknownUserTag);
}

PvTimeStamp::PvTimeStamp(long long secondsPastEpoch, int nanoseconds)
    : PvObject(createStructureDict(), StructureId)
{
    setSecondsPastEpoch(secondsPastEpoch);
    setNanoseconds(nanoseconds);
    setUserTag(UnknownUserTag);
}

PvTimeStamp::PvTimeStamp(long long secondsPastEpoch, int nanoseconds, int userTag)
    : PvObject(createStructureDict(), StructureId)
{
    setSecondsPastEpoch(secondsPastEpoch);
    setNanoseconds(nanoseconds);
    setUserTag(userTag);
}

PvTimeStamp::PvTimeStamp(const boost::python::dict& pyDict, const std::string& structureId)
    : PvObject(pyDict, structureId)
{
}

PvTimeStamp::PvTimeStamp(const PvTimeStamp& pvTimeStamp)
    : PvObject(pvTimeStamp.pvStructurePtr)
{
}

PvTimeStamp::PvTimeStamp(const epics::pvData::PVStructurePtr& pvStructurePtr)
    : PvObject(pvStructurePtr)
{
}

PvTimeStamp::~PvTimeStamp()
{
}

void PvTimeStamp::setSecondsPastEpoch(long long secondsPastEpoch)
{
    pvStructurePtr->getLongField(SecondsPastEpochFieldKey)->put(secondsPastEpoch);
}

long long PvTimeStamp::getSecondsPastEpoch() const
{
    return pvStructurePtr->getLongField(SecondsPastEpochFieldKey)->get();
}

void PvTimeStamp::setNanoseconds(int nanoseconds)
{
    pvStructurePtr->getIntField(NanosecondsFieldKey)->put(nanoseconds);
}

int PvTimeStamp::getNanoseconds() const
{
    return pvStructurePtr->getIntField(NanosecondsFieldKey)->get();
}

void PvTimeStamp::setUserTag(int userTag)
{
    pvStructurePtr->getIntField(UserTagFieldKey)->put(userTag);
}

int PvTimeStamp::getUserTag() const
{
    return pvStructurePtr->getIntField(UserTagFieldKey)->get();
}

