#ifndef PV_UTILITY_H
#define PV_UTILITY_H

#include "pv/pvData.h"

namespace PvUtility 
{

size_t fromString(const epics::pvData::PVStructurePtr& pvStructure, const epics::pvData::StringArray& from, size_t fromStartIndex = 0);

#if defined PVA_API_VERSION && PVA_API_VERSION == 430

void fromString(const epics::pvData::PVScalarPtr& pvScalar, const epics::pvData::String& from);

size_t fromStringArray(const epics::pvData::PVScalarArrayPtr& pv, size_t offset, size_t length, const epics::pvData::StringArray& from, size_t fromOffset);

size_t convertFromStringArray(epics::pvData::PVScalarArray* pv, size_t offset, size_t len, const epics::pvData::StringArray& from, size_t fromOffset);

#else

size_t fromString(const epics::pvData::PVScalarArrayPtr& pv, const epics::pvData::StringArray& from, size_t fromStartIndex = 0);

size_t fromString(const epics::pvData::PVStructureArrayPtr& pv, const epics::pvData::StringArray& from, size_t fromStartIndex = 0);

#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430

}

#endif
