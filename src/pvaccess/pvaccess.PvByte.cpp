// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvByte.h"

using namespace boost::python;

//
// PV Byte class
//
void wrapPvByte()
{

class_<PvByte, bases<PvScalar> >("PvByte", 
    "PvByte represents PV byte type.\n\n"
    "**PvByte([value=''])**\n\n"
    "\t:Parameter: *value* (str) - byte value\n\n"
    "\t::\n\n"
    "\t\tpv = PvByte('a')\n\n", 
    init<>())

    .def(init<char>())
    
    .def("get", 
        &PvByte::get, 
        "Retrieves byte PV value.\n\n"
        ":Returns: byte value\n\n::\n\n"
        "    value = pv.get()\n\n")

    .def("set", 
        &PvByte::set, 
        args("value"), 
        "Sets byte PV value.\n\n"
        ":Parameter: *value* (str) - byte value\n\n"
        "::\n\n"
        "    pv.set('a')\n\n")
;

} // wrapPvByte()

