// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvUInt.h"

using namespace boost::python;

//
// PV UInt class
//
void wrapPvUInt() 
{

class_<PvUInt, bases<PvScalar> >("PvUInt", 
    "PvUInt represents PV unsigned int type.\n\n"
    "**PvUInt([value=0])**\n\n"
    "\t:Parameter: *value* (int) - unsigned integer value\n\n"
    "\t::\n\n"
    "\t\tpv = PvUInt(1000)\n\n", 
    init<>())

    .def(init<unsigned int>())

    .def("get", 
        &PvUInt::get, 
        "Retrieves unsigned integer PV value.\n\n"
        ":Returns: unsigned integer value\n\n"
        "::\n\n"
        "    value = pv.get()\n\n")

    .def("set", 
        &PvUInt::set, 
        args("value"), 
        "Sets unsigned integer PV value.\n\n"
        ":Parameter: *value* (int) - unsigned integer value\n\n"
        "::\n\n"
        "    pv.set(1000)\n\n")
;

} // wrapPvUInt()
