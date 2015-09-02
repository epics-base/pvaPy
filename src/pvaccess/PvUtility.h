// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PV_UTILITY_H
#define PV_UTILITY_H

#include "pv/pvData.h"

namespace PvUtility 
{

size_t fromString(const epics::pvData::PVStructurePtr& pvStructure, const epics::pvData::StringArray& from, size_t fromStartIndex = 0);

size_t fromString(const epics::pvData::PVScalarArrayPtr& pv, const epics::pvData::StringArray& from, size_t fromStartIndex = 0);

size_t fromString(const epics::pvData::PVStructureArrayPtr& pv, const epics::pvData::StringArray& from, size_t fromStartIndex = 0);

}

#endif
