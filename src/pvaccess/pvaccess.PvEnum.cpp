// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include <boost/python/class.hpp>
#include "PvEnum.h"

using namespace boost::python;

//
// PV Enum class
//
void wrapPvEnum()
{

class_<PvEnum, bases<PvObject> >("PvEnum", 
    "PvEnum represents PV Enum structure.\n\n"
    "**PvEnum()**\n\n"
    "\t::\n\n"
    "\t\tenum1 = PvEnum()\n\n"
    "**PvEnum(choices [, index=0])**\n\n"
    "\t:Parameter: *choices* ([str]) - list of choices\n\n"
    "\t:Parameter: *index* (int) - current choice\n\n"
    "\t::\n\n"
    "\t\tenum2 = PvEnum(['a','b','c'], 1)\n\n",
    init<>())

    .def(init<const list&>())

    .def(init<const list&, int>())

    .def(init<const PvObject&>())

    .def("getIndex", 
        &PvEnum::getIndex, 
        "Retrieves current enum choice index.\n\n"
        ":Returns: index value\n\n"
        "::\n\n"
        "    index = enum1.getIndex()\n\n")

    .def("setIndex", 
        &PvEnum::setIndex, 
        args("index"), 
        "Sets index value.\n\n"
        ":Parameter: *index* (int) - index value\n\n"
        "::\n\n"
        "    enum1.setIndex(1)\n\n")

    .def("getChoices", 
        &PvEnum::getChoices, 
        "Retrieves list of choices.\n\n"
        ":Returns: list of choices\n\n"
        "::\n\n"
        "    choices = enum1.getChoices()\n\n")

    .def("setChoices", 
        &PvEnum::setChoices, 
        args("choices"), 
        "Sets list of choices.\n\n"
        ":Parameter: *choices* ([str]) - list of choices\n\n"
        "::\n\n"
        "    enum.setChoices(['a','b','c'])\n\n")

    .def("getCurrentChoice", 
        &PvEnum::getCurrentChoice, 
        "Retrieves current choice.\n\n"
        ":Returns: current choice label\n\n"
        "::\n\n"
        "    choice = enum1.getCurrentChoice()\n\n")

;

} // wrapPvEnum()

