// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvTimeStamp.h"

using namespace boost::python;

//
// PV TimeStamp class
//
void wrapPvTimeStamp()
{

class_<PvTimeStamp, bases<PvObject> >("PvTimeStamp", 
    "PvTimeStamp represents PV time stamp structure.\n\n"
    "**PvTimeStamp()**\n\n"
    "\t::\n\n"
    "\t\ttimestamp1 = PvTimeStamp()\n\n"
    "**PvTimeStamp(secondsPastEpoch, nanoseconds [, userTag=-1])**\n\n"
    "\t:Parameter: *secondsPastEpoch* (long) - seconds past epoch\n\n"
    "\t:Parameter: *nanoseconds* (int) - nanoseconds\n\n"
    "\t:Parameter: *userTag* (int) - user tag\n\n"
    "\t::\n\n"
    "\t\ttimeStamp2 = PvTimeStamp(1234567890, 10000)\n\n"
    "\t\ttimeStamp3 = PvTimeStamp(1234567890, 10000, 1)\n\n", 
    init<>())

    .def(init<long long, int>())

    .def(init<long long, int, int>())

    .def("getSecondsPastEpoch", 
        &PvTimeStamp::getSecondsPastEpoch, 
        "Retrieves time stamp value for seconds past epoch.\n\n"
        ":Returns: seconds past epoch\n\n"
        "::\n\n"
        "    secondsPastEpoch = timeStamp.getSecondsPastEpoch()\n\n")

    .def("setSecondsPastEpoch", 
        &PvTimeStamp::setSecondsPastEpoch, 
        args("secondsPastEpoch"), 
        "Sets time stamp value for seconds past epoch.\n\n"
        ":Parameter: *secondsPastEpoch* (long) - seconds past epoch\n\n"
        "::\n\n"
        "    timeStamp.setSecondsPastEpoch(1234567890)\n\n")

    .def("getNanoseconds", 
        &PvTimeStamp::getNanoseconds, 
        "Retrieves time stamp value for nanoseconds.\n\n"
        ":Returns: nanoseconds\n\n"
        "::\n\n"
        "    nanoseconds = timeStamp.getNanoseconds()\n\n")

    .def("setNanoseconds", 
        &PvTimeStamp::setNanoseconds, 
        args("nanoseconds"), 
        "Sets time stamp value for nanoseconds.\n\n"
        ":Parameter: *nanoseconds* (int) - nanoseconds\n\n"
        "::\n\n"
        "    timeStamp.setNanoseconds(10000)\n\n")

    .def("getUserTag", 
        &PvTimeStamp::getUserTag, 
        "Retrieves user tag.\n\n"
        ":Returns: user tag\n\n"
        "::\n\n"
        "    userTag = timeStamp.getUserTag()\n\n")

    .def("setUserTag", 
        &PvTimeStamp::setUserTag, 
        args("userTag"), 
        "Sets user tag.\n\n"
        ":Parameter: *userTag* (int) - user tag\n\n"
        "::\n\n"
        "    timeStamp.setUserTag(1)\n\n")
;

} // wrapPvTimeStamp()

