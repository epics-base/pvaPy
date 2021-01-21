// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvDisplay.h"

using namespace boost::python;

//
// PV Display class
//
void wrapPvDisplay() 
{

class_<PvDisplay, bases<PvObject> >("PvDisplay", 
    "PvDisplay represents PV display structure.\n\n"
    "**PvDisplay()**\n\n"
    "\t::\n\n"
    "\t\tdisplay1 = PvDisplay()\n\n"
    "**PvDisplay(limitLow, limitHigh, description, format, units)**\n\n"
    "\t:Parameter: *limitLow* (float) - limit low value\n\n"
    "\t:Parameter: *limitHigh* (float) - limit high value\n\n"
    "\t:Parameter: *description* (str) - description string\n\n"
    "\t:Parameter: *format* (str) - format string\n\n"
    "\t:Parameter: *units* (str) - units string\n\n"
    "\t::\n\n"
    "\t\tdisplay2 = PvDisplay(-10.0, 10.0, 'Test Display', 'Test Format', 'amps'))\n\n", 
    init<>())

    .def(init<double, double, const std::string&, const std::string&, const std::string&>())

    .def(init<const PvObject&>())

    .def("getLimitLow", 
        &PvDisplay::getLimitLow, 
        "Retrieves limit low value.\n\n"
        ":Returns: limit low value\n\n"
        "::\n\n"
        "    limitLow = display.getLimitLow()\n\n")

    .def("setLimitLow", 
        &PvDisplay::setLimitLow, 
        args("limitLow"), 
        "Sets limit low value.\n\n"
        ":Parameter: *limitLow* (float) - limit low value\n\n"
        "::\n\n"
        "    display.setLimitLow(10.0)\n\n")

    .def("getLimitHigh", 
        &PvDisplay::getLimitHigh, 
        "Retrieves limit high value.\n\n"
        ":Returns: limit high value\n\n"
        "::\n\n"
        "    limitHigh = display.getLimitHigh()\n\n")

    .def("setLimitHigh", 
        &PvDisplay::setLimitHigh, 
        args("limitHigh"), 
        "Sets limit high value.\n\n"
        ":Parameter: *limitHigh* (float) - limit high value\n\n"
        "::\n\n"
        "    display.setLimitHigh(10.0)\n\n")

    .def("getDescription", 
        &PvDisplay::getDescription, 
        "Retrieves description.\n\n"
        ":Returns: description string\n\n"
        "::\n\n"
        "    description = display.getDescription()\n\n")

    .def("setDescription", 
        &PvDisplay::setDescription, 
        args("description"), 
        "Sets description string.\n\n"
        ":Parameter: *description* (str) - description string\n\n"
        "::\n\n"
        "    display.setDescription('Test display')\n\n")

    .def("getFormat", 
        &PvDisplay::getFormat, 
        "Retrieves format.\n\n"
        ":Returns: format string\n\n"
        "::\n\n"
        "    format = display.getFormat()\n\n")

    .def("setFormat", 
        &PvDisplay::setFormat, 
        args("format"), 
        "Sets format string.\n\n"
        ":Parameter: *format* (str) - format string\n\n"
        "::\n\n"
        "    display.setFormat('Test format')\n\n")

    .def("getUnits", 
        &PvDisplay::getUnits, 
        "Retrieves units.\n\n"
        ":Returns: units string\n\n"
        "::\n\n"
        "    units = display.getUnits()\n\n")

    .def("setUnits", 
        &PvDisplay::setUnits, 
        args("units"), 
        "Sets units string.\n\n"
        ":Parameter: *units* (str) - units string\n\n"
        "::\n\n"
        "    display.setUnits('seconds')\n\n")

;

} // wrapPvDisplay()


