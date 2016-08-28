// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef CHANNEL_MONITOR_DATA_PROCESSOR_H
#define CHANNEL_MONITOR_DATA_PROCESSOR_H

#include "pv/pvData.h"

// Monitor processing interface
class ChannelMonitorDataProcessor 
{
public:
    virtual void processMonitorData(epics::pvData::PVStructurePtr pvStructurePtr)=0;
};

#endif // CHANNEL_MONITOR_DATA_PROCESSOR_H
