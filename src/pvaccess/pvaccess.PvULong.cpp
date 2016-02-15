// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvULong.h"

using namespace boost::python;

//
// PV ULong class
//
void wrapPvULong() 
{

class_<PvULong, bases<PvScalar> >("PvULong", 
    "PvULong represents PV unsigned long type.\n\n"
    "**PvULong([value=0])**\n\n"
    "\t:Parameter: *value* (long) - unsigned long value\n\n"
    "\t::\n\n"
    "\t\tpv = PvULong(100000L)\n\n", 
    init<>())

    .def(init<unsigned long long>())

    .def("get", 
        &PvULong::get, 
        "Retrieves unsigned long PV value.\n\n"
        ":Returns: unsigned long value\n\n"
        "::\n\n"
        "    value = pv.get()\n\n")

    .def("set", 
        &PvULong::set, 
        args("value"), 
        "Sets unsigned long PV value.\n\n"
        ":Parameter: *value* (long) - unsigned long value\n\n"
        "::\n\n"
        "    pv.set(100000L)\n\n")
;

} // wrapPvULong()
