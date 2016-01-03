// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "NtType.h"

using namespace boost::python;

// 
// NT Type class
// 
void wrapNtType()
{

class_<NtType, bases<PvObject> >("NtType", 
    "NtType is a base class for all NT structures. It cannot be instantiated directly from python.\n\n", 
    no_init)
;

} // wrapNtType()

