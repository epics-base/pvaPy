// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvValueAlarm.h"

using namespace boost::python;

//
// PV ValueAlarm class
//
void wrapPvValueAlarm() 
{

class_<PvValueAlarm, bases<PvObject> >("PvValueAlarm",
    "PvValueAlarm represents PV value alarm structure.\n\n"
    "**PvValueAlarm(scalarType)**\n\n"
    "\t:Parameter: *scalarType* (PVTYPE) - scalar type of array elements\n\n"
    "\t- PVTYPE: scalar type, can be BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING\n\n"
    "\t::\n\n"
    "\t\talarm = PvValueAlarm(INT)\n\n",
    init<PvType::ScalarType>())

    .def(init<const PvObject&>())

    .def("getActive",
        &PvValueAlarm::getActive,
        "Retrieves active value.\n\n"
        ":Returns: active value\n\n"
        "::\n\n"
        "    value = alarm.getActive()\n\n")

    .def("setActive",
        &PvValueAlarm::setActive,
        args("value"),
        "Sets active value.\n\n"
        ":Parameter: *value* (boolean) - active value\n\n"
        "::\n\n"
        "    alarm.setActive(True)\n\n")

    .def("getLowAlarmLimit",
        &PvValueAlarm::getLowAlarmLimit,
        "Retrieves low alarm limit value.\n\n"
        ":Returns: low alarm limit value\n\n"
        "::\n\n"
        "    value = alarm.getLowAlarmLimit()\n\n")

    .def("setLowAlarmLimit",
        &PvValueAlarm::setLowAlarmLimit,
        args("value"),
        "Sets low alarm limit value.\n\n"
        ":Parameter: *value* (scalar_t) - low alarm limit value\n\n"
        "::\n\n"
        "    alarm.setLowAlarmLimit(10)\n\n")

    .def("getLowWarningLimit",
        &PvValueAlarm::getLowWarningLimit,
        "Retrieves low warning limit value.\n\n"
        ":Returns: low warning limit value\n\n"
        "::\n\n"
        "    value = alarm.getLowWarningLimit()\n\n")

    .def("setLowWarningLimit",
        &PvValueAlarm::setLowWarningLimit,
        args("value"),
        "Sets low warning limit value.\n\n"
        ":Parameter: *value* (scalar_t) - low warning limit value\n\n"
        "::\n\n"
        "    alarm.setLowWarningLimit(10)\n\n")

    .def("getHighWarningLimit",
        &PvValueAlarm::getHighWarningLimit,
        "Retrieves high warning limit value.\n\n"
        ":Returns: high warning limit value\n\n"
        "::\n\n"
        "    value = alarm.getHighWarningLimit()\n\n")

    .def("setHighWarningLimit",
        &PvValueAlarm::setHighWarningLimit,
        args("value"),
        "Sets high warning limit value.\n\n"
        ":Parameter: *value* (scalar_t) - high warning limit value\n\n"
        "::\n\n"
        "    alarm.setHighWarningLimit(10)\n\n")

    .def("getHighAlarmLimit",
        &PvValueAlarm::getHighAlarmLimit,
        "Retrieves high alarm limit value.\n\n"
        ":Returns: high alarm limit value\n\n"
        "::\n\n"
        "    value = alarm.getHighAlarmLimit()\n\n")

    .def("setHighAlarmLimit",
        &PvValueAlarm::setHighAlarmLimit,
        args("value"),
        "Sets high alarm limit value.\n\n"
        ":Parameter: *value* (scalar_t) - high alarm limit value\n\n"
        "::\n\n"
        "    alarm.setHighAlarmLimit(10)\n\n")

    .def("getLowAlarmSeverity",
        &PvValueAlarm::getLowAlarmSeverity,
        "Retrieves low alarm severity value.\n\n"
        ":Returns: low alarm severity value\n\n"
        "::\n\n"
        "    value = alarm.getLowAlarmSeverity()\n\n")

    .def("setLowAlarmSeverity",
        &PvValueAlarm::setLowAlarmSeverity,
        args("value"),
        "Sets low alarm severity value.\n\n"
        ":Parameter: *value* (int) - low alarm severity value\n\n"
        "::\n\n"
        "    alarm.setLowAlarmSeverity(10)\n\n")

    .def("getLowWarningSeverity",
        &PvValueAlarm::getLowWarningSeverity,
        "Retrieves low warning severity value.\n\n"
        ":Returns: low warning severity value\n\n"
        "::\n\n"
        "    value = alarm.getLowWarningSeverity()\n\n")

    .def("setLowWarningSeverity",
        &PvValueAlarm::setLowWarningSeverity,
        args("value"),
        "Sets low warning severity value.\n\n"
        ":Parameter: *value* (int) - low warning severity value\n\n"
        "::\n\n"
        "    alarm.setLowWarningSeverity(10)\n\n")

    .def("getHighWarningSeverity",
        &PvValueAlarm::getHighWarningSeverity,
        "Retrieves high warning severity value.\n\n"
        ":Returns: high warning severity value\n\n"
        "::\n\n"
        "    value = alarm.getHighWarningSeverity()\n\n")

    .def("setHighWarningSeverity",
        &PvValueAlarm::setHighWarningSeverity,
        args("value"),
        "Sets high warning severity value.\n\n"
        ":Parameter: *value* (int) - high warning severity value\n\n"
        "::\n\n"
        "    alarm.setHighWarningSeverity(10)\n\n")

    .def("getHighAlarmSeverity",
        &PvValueAlarm::getHighAlarmSeverity,
        "Retrieves high alarm severity value.\n\n"
        ":Returns: high alarm severity value\n\n"
        "::\n\n"
        "    value = alarm.getHighAlarmSeverity()\n\n")

    .def("setHighAlarmSeverity",
        &PvValueAlarm::setHighAlarmSeverity,
        args("value"),
        "Sets high alarm severity value.\n\n"
        ":Parameter: *value* (int) - high alarm severity value\n\n"
        "::\n\n"
        "    alarm.setHighAlarmSeverity(10)\n\n")

    .def("getHysteresis",
        &PvValueAlarm::getHysteresis,
        "Retrieves hysteresis value.\n\n"
        ":Returns: hysteresis value\n\n"
        "::\n\n"
        "    value = alarm.getHysteresis()\n\n")

    .def("setHysteresis",
        &PvValueAlarm::setHysteresis,
        args("value"),
        "Sets hysteresis value.\n\n"
        ":Parameter: *value* (chr) - hysteresis value\n\n"
        "::\n\n"
        "    alarm.setHysteresis(True)\n\n")

;

} // wrapPvValueAlarm()


