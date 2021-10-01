// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#if PVA_API_VERSION <= 440
#include "Channel.440.cpp"
#elif PVA_API_VERSION <= 450
#include "Channel.450.cpp"
#elif PVA_API_VERSION <= 460
#include "Channel.460.cpp"
#elif PVA_API_VERSION <= 470
#include "Channel.470.cpp"
#elif PVA_API_VERSION <= 481
#include "Channel.480.cpp"
#elif PVA_API_VERSION <= 483
#include "Channel.482.cpp"
#endif // if PVA_API_VERSION <= 440

