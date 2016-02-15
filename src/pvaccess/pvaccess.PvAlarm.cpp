// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvAlarm.h"

using namespace boost::python;

//
// PV Alarm class
//
void wrapPvAlarm() 
{

class_<PvAlarm, bases<PvObject> >("PvAlarm", 
    "PvAlarm represents PV alarm structure.\n\n"
    "**PvAlarm()**\n\n"
    "\t::\n\n"
    "\t\talarm1 = PvAlarm()\n\n"
    "**PvAlarm(severity, status, message)**\n\n"
    "\t:Parameter: *severity* (int) - alarm severity\n\n"
    "\t:Parameter: *status* (int) - status code\n\n"
    "\t:Parameter: *message* (str) - alarm message\n\n"
    "\t::\n\n"
    "\t\talarm2 = PvAlarm(5, 1, 'alarm message')\n\n", 
    init<>())

    .def(init<int, int, const std::string&>())

    .def("getSeverity", 
        &PvAlarm::getSeverity, 
        "Retrieves alarm severity.\n\n"
        ":Returns: alarm severity\n\n"
        "::\n\n"
        "    severity = alarm.getSeverity()\n\n")

    .def("setSeverity", 
        &PvAlarm::setSeverity, 
        args("severity"), 
        "Sets alarm severity.\n\n"
        ":Parameter: *severity* (int) - alarm severity\n\n"
        "::\n\n"
        "    alarm.setSeverity(1)\n\n")

    .def("getStatus", 
        &PvAlarm::getStatus, 
        "Retrieves status code.\n\n"
        ":Returns: status code\n\n"
        "::\n\n"
        "    status = alarm.getStatusCode()\n\n")

    .def("setStatus", 
        &PvAlarm::setStatus, 
        args("status"), 
        "Sets status code.\n\n"
        ":Parameter: *status* (int) - status code\n\n"
        "::\n\n"
        "    alarm.setStatus(1)\n\n")

    .def("getMessage", 
        &PvAlarm::getMessage, 
        "Retrieves alarm message.\n\n"
        ":Returns: alarm message\n\n"
        "::\n\n"
        "    message = alarm.getMessage()\n\n")

    .def("setMessage", 
        &PvAlarm::setMessage, 
        args("message"), 
        "Sets alarm message.\n\n"
        ":Parameter: *message* (str) - alarm message\n\n"
        "::\n\n"
        "    alarm.setmessage('alarm message')\n\n")
;

} // wrapPvAlarm()


