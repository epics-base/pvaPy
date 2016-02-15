// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvDouble.h"

using namespace boost::python;

//
// PV Double class
//
void wrapPvDouble()
{

class_<PvDouble, bases<PvScalar> >("PvDouble", 
    "PvDouble represents PV double type.\n\n"
    "**PvDouble([value=0])**\n\n"
    "\t:Parameter: *value* (float) - double value\n\n"
    "\t::\n\n"
    "\t\tpv = PvDouble(1.1)\n\n", 
    init<>())

    .def(init<double>())

    .def("get", 
        &PvDouble::get, 
        "Retrieves double PV value.\n\n"
        ":Returns: double value\n\n"
        "::\n\n"
        "    value = pv.get()\n\n")

    .def("set", 
        &PvDouble::set, 
        args("value"), 
        "Sets double PV value.\n\n"
        ":Parameter: *value* (float) - double value\n\n"
        "::\n\n"
        "    pv.set(1.1)\n\n")
;

} // wrapPvDouble()

