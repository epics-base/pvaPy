#include "PvTimeStamp.h"
#include "PvType.h"

const char* PvTimeStamp::StructureId("time_t");

const char* PvTimeStamp::SecondsPastEpochFieldKey("secondsPastEpoch");
const char* PvTimeStamp::NanoSecondsFieldKey("nanoSeconds");
const char* PvTimeStamp::UserTagFieldKey("userTag");

const int PvTimeStamp::UnknownUserTag(-1);

boost::python::dict PvTimeStamp::createStructureDict()
{
    // StandardField implementation is using signed types for some reason.
    boost::python::dict pyDict;
    pyDict[SecondsPastEpochFieldKey] = PvType::Long;
    pyDict[NanoSecondsFieldKey] = PvType::Int;
    pyDict[UserTagFieldKey] = PvType::Int;
    return pyDict;
}

PvTimeStamp::PvTimeStamp()
    : PvObject(createStructureDict(), StructureId)
{
    setSecondsPastEpoch(0);
    setNanoSeconds(0);
    setUserTag(UnknownUserTag);
}

PvTimeStamp::PvTimeStamp(long long secondsPastEpoch, int nanoSeconds)
    : PvObject(createStructureDict(), StructureId)
{
    setSecondsPastEpoch(secondsPastEpoch);
    setNanoSeconds(nanoSeconds);
    setUserTag(UnknownUserTag);
}

PvTimeStamp::PvTimeStamp(long long secondsPastEpoch, int nanoSeconds, int userTag)
    : PvObject(createStructureDict(), StructureId)
{
    setSecondsPastEpoch(secondsPastEpoch);
    setNanoSeconds(nanoSeconds);
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

void PvTimeStamp::setNanoSeconds(int nanoSeconds)
{
    pvStructurePtr->getIntField(NanoSecondsFieldKey)->put(nanoSeconds);
}

int PvTimeStamp::getNanoSeconds() const
{
    return pvStructurePtr->getIntField(NanoSecondsFieldKey)->get();
}

void PvTimeStamp::setUserTag(int userTag)
{
    pvStructurePtr->getIntField(UserTagFieldKey)->put(userTag);
}

int PvTimeStamp::getUserTag() const
{
    return pvStructurePtr->getIntField(UserTagFieldKey)->get();
}

