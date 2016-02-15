// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvLong.h"

using namespace boost::python;

//
// PV Long class
//
void wrapPvLong() 
{

class_<PvLong, bases<PvScalar> >("PvLong", 
    "PvLong represents PV long type.\n\n"
    "**PvLong([value=0])**\n\n"
    "\t:Parameter: *value* (long) - long value\n\n"
    "\t::\n\n"
    "\t\tpv = PvLong(-100000L)\n\n", 
    init<>())

    .def(init<long long>())

    .def("get", 
        &PvLong::get, 
        "Retrieves long PV value.\n\n"
        ":Returns: long value\n\n"
        "::\n\n"
        "    value = pv.get()\n\n")

    .def("set", 
        &PvLong::set, 
        "Sets long PV value.\n\n"
        ":Parameter: *value* (long) - long value\n\n"
        "::\n\n"
        "    pv.set(-100000L)\n\n")
;

} // wrapPvLong()

