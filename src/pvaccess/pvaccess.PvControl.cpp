// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvControl.h"

using namespace boost::python;

//
// PV Control class
//
void wrapPvControl() 
{

class_<PvControl, bases<PvObject> >("PvControl", 
    "PvControl represents PV control structure.\n\n"
    "**PvControl()**\n\n"
    "\t::\n\n"
    "\t\tcontrol1 = PvControl()\n\n"
    "**PvControl(limitLow, limitHigh, description, format, units)**\n\n"
    "\t:Parameter: *limitLow* (float) - limit low value\n\n"
    "\t:Parameter: *limitHigh* (float) - limit high value\n\n"
    "\t:Parameter: *minStep* (float) - min step value\n\n"
    "\t::\n\n"
    "\t\tcontrol2 = PvControl(-10.0, 10.0, 1.0))\n\n", 
    init<>())

    .def(init<double, double, double>())

    .def(init<const PvObject&>())

    .def("getLimitLow", 
        &PvControl::getLimitLow, 
        "Retrieves limit low value.\n\n"
        ":Returns: limit low value\n\n"
        "::\n\n"
        "    limitLow = control.getLimitLow()\n\n")

    .def("setLimitLow", 
        &PvControl::setLimitLow, 
        args("limitLow"), 
        "Sets limit low value.\n\n"
        ":Parameter: *limitLow* (float) - limit low value\n\n"
        "::\n\n"
        "    control.setLimitLow(10.0)\n\n")

    .def("getLimitHigh", 
        &PvControl::getLimitHigh, 
        "Retrieves limit high value.\n\n"
        ":Returns: limit high value\n\n"
        "::\n\n"
        "    limitHigh = control.getLimitHigh()\n\n")

    .def("setLimitHigh", 
        &PvControl::setLimitHigh, 
        args("limitHigh"), 
        "Sets limit high value.\n\n"
        ":Parameter: *limitHigh* (float) - limit high value\n\n"
        "::\n\n"
        "    control.setLimitHigh(10.0)\n\n")

    .def("getMinStep", 
        &PvControl::getMinStep, 
        "Retrieves min step.\n\n"
        ":Returns: min step value\n\n"
        "::\n\n"
        "    minStep = control.getMinStep()\n\n")

    .def("setMinStep", 
        &PvControl::setMinStep, 
        args("minStep"), 
        "Sets min step value.\n\n"
        ":Parameter: *minStep* (float) - min step value\n\n"
        "::\n\n"
        "    control.setMinStep(1.0)\n\n")

;

} // wrapPvControl()


