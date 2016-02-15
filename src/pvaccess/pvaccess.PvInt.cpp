// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvInt.h"

using namespace boost::python;

//
// PV Int class
//
void wrapPvInt() 
{

class_<PvInt, bases<PvScalar> >("PvInt", 
    "PvInt represents PV integer type.\n\n"
    "**PvInt([value=0])**\n\n"
    "\t:Parameter: *value* (int) - integer value\n\n"
    "\t::\n\n"
    "\t\tpv = PvInt(-1000)\n\n", 
    init<>())

    .def(init<int>())

    .def("get", 
        &PvInt::get, 
        "Retrieves integer PV value.\n\n"
        ":Returns: integer value\n\n"
        "::\n\n"
        "    value = pv.get()\n\n")

    .def("set", 
        &PvInt::set, 
        args("value"), 
        "Sets integer PV value.\n\n"
        ":Parameter: *value* (int) - integer value\n\n"
        "::\n\n"
        "    pv.set(-1000)\n\n")
;

} // wrapPvInt()

