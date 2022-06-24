// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "NtEnum.h"

using namespace boost::python;

// 
// NT Enum class
// 
void wrapNtEnum()
{

class_<NtEnum, bases<NtType> >("NtEnum", 
    "NtEnum represents NT enum structure.\n\n"
    "**NtEnum()**\n\n"
    "\t::\n\n"
    "\t\tenum1 = NtEnum()\n\n"
    "**NtEnum(choices [, index=0])**\n\n"
    "\t:Parameter: *choices* ([str]) - list of choices\n\n"
    "\t:Parameter: *index* (int) - current choice\n\n"
    "\t::\n\n"
    "\t\tenum2 = NtEnum(['a','b','c'], 1)\n\n",
    init<>())

    .def(init<const list&>())

    .def(init<const list&, int>())

    .def(init<const NtEnum&>())

    .def(init<const PvObject&>())

    .def("getValue", 
        &NtEnum::getValue,
        "Get enum value object.\n\n"
        ":Returns: enum object\n\n"
        "::\n\n"
        "    pvEnum = enum.getValue()\n\n")

    .def("setValue", 
        static_cast<void(NtEnum::*)(int)>(&NtEnum::setValue),
        args("index"), 
        "Sets enum object index value.\n\n"
        ":Parameter: *index* (int) - current choice index\n\n"
        "::\n\n"
        "    enum.setValue(2)\n\n")

    .def("setValue", 
        static_cast<void(NtEnum::*)(const boost::python::dict&)>(&NtEnum::setValue),
        args("value"), 
        "Sets enum object value.\n\n"
        ":Parameter: *value* (dict) - dictionary containing enum fields\n\n"
        "::\n\n"
        "    enum.setValue({'choices' : ['a','b','c'], 'index' : 1})\n\n")

    .def("setValue", 
        static_cast<void(NtEnum::*)(const PvEnum&)>(&NtEnum::setValue),
        args("value"), 
        "Sets enum object value.\n\n"
        ":Parameter: *value* (enum_t) - enum object value\n\n"
        "::\n\n"
        "    enum.setValue(PvEnum(['a','b','c'],1))\n\n")

    .def("getDescriptor", 
        &NtEnum::getDescriptor, 
        "Retrieves enum descriptor.\n\n"
        ":Returns: enum descriptor\n\n"
        "::\n\n"
        "    descriptor = enum.getDescriptor()\n\n")

    .def("setDescriptor", 
        &NtEnum::setDescriptor, 
        args("descriptor"), 
        "Sets enum descriptor.\n\n"
        ":Parameter: *descriptor* (str) - enum object descriptor\n\n"
        "::\n\n"
        "    enum.setDescriptor('myEnum')\n\n")

    .def("getTimeStamp", 
        &NtEnum::getTimeStamp, 
        "Retrieves enum object time stamp.\n\n"
        ":Returns: enum object time stamp\n\n"
        "::\n\n"
        "    timeStamp = enum.getTimeStamp()\n\n")

    .def("setTimeStamp", 
        &NtEnum::setTimeStamp, 
        args("timeStamp"), 
        "Sets enum object time stamp.\n\n"
        ":Parameter: *timeStamp* (PvTimeStamp) - enum object time stamp\n\n"
        "::\n\n"
        "    timeStamp = PvTimeStamp(1234567890, 10000, 1)\n\n"
        "    enum.setTimeStamp(timeStamp)\n\n")

    .def("getAlarm", 
        &NtEnum::getAlarm, 
        "Retrieves enum object alarm.\n\n"
        ":Returns: enum object alarm\n\n"
        "::\n\n"
        "    alarm = enum.getAlarm()\n\n")

    .def("setAlarm", 
        &NtEnum::setAlarm, 
        args("alarm"), 
        "Sets enum object alarm.\n\n"
        ":Parameter: *alarm* (PvAlarm) - enum object alarm\n\n"
        "::\n\n"
        "    alarm = PvAlarm(11, 126, 'Server SegFault')\n\n"
        "    enum.setAlarm(alarm)\n\n")
;

} // wrapNtEnum()

