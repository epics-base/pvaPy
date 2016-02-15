// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvString.h"

using namespace boost::python;

//
// PV String class
//
void wrapPvString()
{

class_<PvString, bases<PvScalar> >("PvString", 
    "PvString represents PV string type.\n\n"
    "**PvString([value=''])**\n\n"
    "\t:Parameter: *value* (str) - string value\n\n"
    "\t::\n\n"
    "\t\tpv = PvString('stringValue')\n\n", 
    init<>())

    .def(init<std::string>())

    .def("get", 
        &PvString::get, 
        "Retrieves string PV value.\n\n"
        ":Returns: string value\n\n"
        "::\n\n"
        "    value = pv.get()\n\n")

    .def("set", 
        &PvString::set, 
        args("value"), 
        "Sets string PV value.\n\n"
        ":Parameter: *value* (str) - string value\n\n"
        "::\n\n"
        "    pv.set('stringValue')\n\n")
;

} // wrapPvString()


