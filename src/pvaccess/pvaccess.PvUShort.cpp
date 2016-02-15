// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvUShort.h"

using namespace boost::python;

//
// PV UShort class
//
void wrapPvUShort() 
{

class_<PvUShort, bases<PvScalar> >("PvUShort", 
    "PvUShort represents PV unsigned short type.\n\n"
    "**PvUShort([value=0])**\n\n"
    "\t:Parameter: *value* (int) - unsigned short value\n\n"
    "\t::\n\n"
    "\t\tpv = PvUShort(10)\n\n", 
    init<>())

    .def(init<unsigned short>())

    .def("get", 
        &PvUShort::get, 
        "Retrieves unsigned short PV value.\n\n"
        ":Returns: unsigned short value\n\n"
        "::\n\n"
        "    value = pv.get()\n\n")

    .def("set", 
        &PvUShort::set, 
        args("value"), 
        "Sets unsigned short PV value.\n\n"
        ":Parameter: *value* (int) - unsigned short value\n\n"
        "::\n\n"
        "    pv.set(10)\n\n")
;

} // wrapPvUShort()

