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

