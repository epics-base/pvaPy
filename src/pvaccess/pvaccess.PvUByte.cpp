// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvUByte.h"

using namespace boost::python;

//
// PV UByte class
//
void wrapPvUByte() 
{

class_<PvUByte, bases<PvScalar> >("PvUByte", 
    "PvUByte represents PV unsigned byte type.\n\n"
    "**PvUByte([value=0])**\n\n"
    "\t:Parameter: *value* (int) - unsigned byte value\n\n"
    "\t::\n\n\t\tpv = PvUByte(10)\n\n", 
    init<>())

    .def(init<unsigned char>())

    .def("get", 
        &PvUByte::get, 
        "Retrieves unsigned byte PV value.\n\n"
        ":Returns: unsigned byte value\n\n"
        "::\n\n"
        "    value = pv.get()\n\n")

    .def("set", 
        &PvUByte::set, 
        args("value"), 
        "Sets unsigned byte PV value.\n\n"
        ":Parameter: *value* (int) - unsigned byte value\n\n"
        "::\n\n"
        "    pv.set(10)\n\n")
;

} // wrapPvUByte()


