// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#if PVA_API_VERSION <= 440
#include "ChannelMonitorRequesterImpl.440.cpp"
#elif PVA_API_VERSION >= 450
#include "ChannelMonitorRequesterImpl.450.cpp"
#endif // if PVA_API_VERSION <= 440

