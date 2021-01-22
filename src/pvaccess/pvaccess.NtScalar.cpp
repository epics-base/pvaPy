// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "NtScalar.h"

using namespace boost::python;

// 
// NT Scalar class
// 
void wrapNtScalar()
{

class_<NtScalar, bases<NtType> >("NtScalar", 
    "NtScalar represents NT scalar structure.\n\n"
    "**NtScalar(scalarType)**\n\n"
    "\t:Parameter: *scalarType* (PVTYPE) - scalar type (BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING)\n\n"
    "\t::\n\n"
    "\t\ts1 = NtScalar(DOUBLE)\n\n"
    "**NtScalar(scalarType, value)**\n\n"
    "\t:Parameter: *scalarType* (PVTYPE) - scalar type (BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING)\n\n"
    "\t:Parameter: *value* (scalar_t) - scalar value\n\n"
    "\t::\n\n"
    "\t\ts2 = NtScalar(DOUBLE, 10.0)\n\n"
    "**NtScalar(pvObject)**\n\n"
    "\t:Parameter: *pvObject* (PvObject) - PV object that has a structure containing required NT scalar elements:\n\n"
    "\t::\n\n"
    "\t\tpvObject = PvInt(3)\n\n"
    "\t\ts3 = NtScalar(pvObject)", 
    init<PvType::ScalarType>())

    .def(init<PvType::ScalarType, const boost::python::object&>())

    .def(init<const PvObject&>())

    .def("getValue", 
        &NtScalar::getValue,
        "Get scalar value.\n\n"
        ":Returns: scalar value\n\n"
        "::\n\n"
        "    value = scalar.getValue()\n\n")

    .def("setValue", 
        &NtScalar::setValue, 
        args("value"), 
        "Sets scalar value.\n\n"
        ":Parameter: *value* (scalar_t) - scalar value\n\n"
        "::\n\n"
        "    scalar.setValue(1)\n\n")

    .def("getDescriptor", 
        &NtScalar::getDescriptor, 
        "Retrieves scalar descriptor.\n\n"
        ":Returns: scalar descriptor\n\n"
        "::\n\n"
        "    descriptor = scalar.getDescriptor()\n\n")

    .def("setDescriptor", 
        &NtScalar::setDescriptor, 
        args("descriptor"), 
        "Sets scalar descriptor.\n\n"
        ":Parameter: *descriptor* (str) - scalar descriptor\n\n"
        "::\n\n"
        "    scalar.setDescriptor('myTable')\n\n")

    .def("getTimeStamp", 
        &NtScalar::getTimeStamp, 
        "Retrieves scalar time stamp.\n\n"
        ":Returns: scalar time stamp object\n\n"
        "::\n\n"
        "    timeStamp = scalar.getTimeStamp()\n\n")

    .def("setTimeStamp", 
        &NtScalar::setTimeStamp, 
        args("timeStamp"), 
        "Sets scalar time stamp.\n\n"
        ":Parameter: *timeStamp* (PvTimeStamp) - scalar time stamp object\n\n"
        "::\n\n"
        "    timeStamp = PvTimeStamp(1234567890, 10000, 1)\n\n"
        "    scalar.setTimeStamp(timeStamp)\n\n")

    .def("getAlarm", 
        &NtScalar::getAlarm, 
        "Retrieves scalar alarm.\n\n"
        ":Returns: scalar alarm object\n\n"
        "::\n\n"
        "    alarm = scalar.getAlarm()\n\n")

    .def("setAlarm", 
        &NtScalar::setAlarm, 
        args("alarm"), 
        "Sets scalar alarm.\n\n"
        ":Parameter: *alarm* (PvAlarm) - scalar alarm object\n\n"
        "::\n\n"
        "    alarm = PvAlarm(11, 126, 'Server SegFault')\n\n"
        "    scalar.setAlarm(alarm)\n\n")

    .def("getDisplay", 
        &NtScalar::getDisplay, 
        "Retrieves scalar display.\n\n"
        ":Returns: scalar display object\n\n"
        "::\n\n"
        "    display = scalar.getDisplay()\n\n")

    .def("setDisplay", 
        &NtScalar::setDisplay, 
        args("display"), 
        "Sets scalar display.\n\n"
        ":Parameter: *display* (PvDisplay) - scalar display object\n\n"
        "::\n\n"
        "    display = PvDisplay()\n\n"
        "    scalar.setDisplay(display)\n\n")

    .def("getControl", 
        &NtScalar::getControl, 
        "Retrieves scalar control.\n\n"
        ":Returns: scalar control object\n\n"
        "::\n\n"
        "    control = scalar.getControl()\n\n")

    .def("setControl", 
        &NtScalar::setControl, 
        args("control"), 
        "Sets scalar control.\n\n"
        ":Parameter: *control* (PvControl) - scalar control object\n\n"
        "::\n\n"
        "    control = PvControl()\n\n"
        "    scalar.setControl(control)\n\n")
;

} // wrapNtScalar()

