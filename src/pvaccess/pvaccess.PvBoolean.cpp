// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvBoolean.h"

using namespace boost::python;

//
// PV Boolean class
//
void wrapPvBoolean()
{

class_<PvBoolean, bases<PvScalar> >("PvBoolean", 
    "PvBoolean represents PV boolean type.\n\n"
    "**PvBoolean([value=False])**\n\n"
    "\t:Parameter: *value* (bool) - boolean value\n\n"
    "\t::\n\n"
    "\t\tpv = PvBoolean(True)\n\n", 
    init<>())

    .def(init<bool>())

    .def("get", 
        &PvBoolean::get, 
        "Retrieves boolean PV value.\n\n"
        ":Returns: boolean value\n\n"
        "::\n\n"
        "    value = pv.get()\n\n")

    .def("set", 
        &PvBoolean::set, args("value"), 
        "Sets boolean PV value.\n\n"
        ":Parameter: *value* (bool) - boolean value\n\n"
        "::\n\n"
        "    pv.set(False)\n\n")
;

} // wrapPvBoolean()



