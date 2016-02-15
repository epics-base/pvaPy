// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvShort.h"

using namespace boost::python;

//
// PV Short
//
void wrapPvShort() 
{

class_<PvShort, bases<PvScalar> >("PvShort", 
    "PvShort represents PV short type.\n\n"
    "**PvShort([value=0])**\n\n"
    "\t:Parameter: *value* (int) - short value\n\n"
    "\t::\n\n"
    "\t\tpv = PvShort(-10)\n\n", 
    init<>())

    .def(init<short>())

    .def("get", 
        &PvShort::get, 
        "Retrieves short PV value.\n\n"
        ":Returns: short value\n\n"
        "::\n\n"
        "    value = pv.get()\n\n")

    .def("set", 
        &PvShort::set, 
        args("value"), 
        "Sets short PV value.\n\n"
        ":Parameter: *value* (int) - short value\n\n"
        "::\n\n"
        "    pv.set(-10)\n\n")
;

} // wrapPvShort()

