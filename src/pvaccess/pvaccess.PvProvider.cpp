// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/enum.hpp"
#include "PvProvider.h"

using namespace boost::python;

//
// PvProvider enum 
// 
void wrapPvProvider()
{

enum_<PvProvider::ProviderType>("ProviderType")
    .value("PVA", PvProvider::PvaProviderType)
    .value("CA", PvProvider::CaProviderType)
    .export_values()
;
                                        
} // wrapPvProvider()

