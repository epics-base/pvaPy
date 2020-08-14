// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "NtAttribute.h"

using namespace boost::python;

//
// NT Attribute class
//
void wrapNtAttribute() 
{

class_<NtAttribute, bases<PvObject> >("NtAttribute", 
    "NtAttribute class represents NTAttribute normative type.\n\n"
    "**NtAttribute()**\n\n"
    "\t::\n\n"
    "\t\tattr1 = NtAttribute()\n\n"
    "**NtAttribute(name, parameters)**\n\n"
    "\t:Parameter: *name* (str) - attr name\n\n"
    "\t:Parameter: *value* (PvObject) - attr value\n\n"
    "\t::\n\n"
    "\t\tattr2 = NtAttribute('fileAttr', PvObject({'value':{'size':INT,'crc':STRING}},{'value':{'size':123456,'crc':'ab34cf6123'}}))\n\n", 
    init<>())

    .def(init<const std::string&, const PvObject&>())

    .def(init<const PvObject&>())

    .def("getName", 
        &NtAttribute::getName, 
        "Retrieves attribute name.\n\n"
        ":Returns: attribute name\n\n"
        "::\n\n"
        "    name = attr.getName()\n\n")

    .def("setName", 
        &NtAttribute::setName, 
        args("name"), 
        "Sets attribute name.\n\n"
        ":Parameter: *name* (str) - attribute name\n\n"
        "::\n\n"
        "    attr.setName('attr name')\n\n")

    .def("getValue", 
        &NtAttribute::getValue, 
        "Retrieves attribute parameters.\n\n"
        ":Returns: attribute parameters\n\n"
        "::\n\n"
        "    p = attr.getValue()\n\n")

    .def("setValue", static_cast<void(PvObject::*)(const PvObject&)>(&NtAttribute::setValue),
        args("valueObject"),
        "Sets value field.\n\n"
        ":Parameter: *valueObject* (PvObject) - attribute value object\n\n"
        "::\n\n"
        "    p = PvObject({'value':{'size':INT,'crc':STRING}},{'value':{'size':123456,'crc':'ab34cf6123'}})\n\n"
        "    attr.setValue(p)\n\n")

    .def("getTags",
        &NtAttribute::getTags,
        "Retrieves list of tags.\n\n"
        ":Returns: list of tags\n\n"
        "::\n\n"
        "    tagList = attr.getTags()\n\n")

    .def("setTags",
        &NtAttribute::setTags,
        args("tagList"),
        "Sets attribute tags.\n\n"
        ":Parameter: *tagList* ([str]) - list of strings attribute tags\n\n"
        "::\n\n"
        "    attr.setTags(['DatasetA', 'DetectorB'])\n\n")

    .def("getDescriptor",
        &NtAttribute::getDescriptor,
        "Retrieves attribute descriptor.\n\n"
        ":Returns: attribute descriptor\n\n"
        "::\n\n"
        "    descriptor = attr.getDescriptor()\n\n")

    .def("setDescriptor",
        &NtAttribute::setDescriptor,
        args("descriptor"),
        "Sets attribute descriptor.\n\n"
        ":Parameter: *descriptor* (str) - attribute descriptor\n\n"
        "::\n\n"
        "    attr.setDescriptor('myAttr')\n\n")

    .def("getTimeStamp",
        &NtAttribute::getTimeStamp,
        "Retrieves attribute time stamp.\n\n"
        ":Returns: attribute time stamp object\n\n"
        "::\n\n"
        "    timeStamp = attr.getTimeStamp()\n\n")

    .def("setTimeStamp",
        &NtAttribute::setTimeStamp,
        args("timeStamp"),
        "Sets attribute time stamp.\n\n"
        ":Parameter: *timeStamp* (PvTimeStamp) - attribute time stamp object\n\n"
        "::\n\n"
        "    timeStamp = PvTimeStamp(1234567890, 10000, 1)\n\n"
        "    attr.setTimeStamp(timeStamp)\n\n")

    .def("getAlarm",
        &NtAttribute::getAlarm,
        "Retrieves attribute alarm.\n\n"
        ":Returns: attribute alarm object\n\n"
        "::\n\n"
        "    alarm = attr.getAlarm()\n\n")

    .def("setAlarm",
        &NtAttribute::setAlarm,
        args("alarm"),
        "Sets attribute alarm.\n\n"
        ":Parameter: *alarm* (PvAlarm) - attribute alarm object\n\n"
        "::\n\n"
        "    alarm = PvAlarm(11, 126, 'Server SegFault')\n\n"
        "    attr.setAlarm(alarm)\n\n")

    .def("getSourceType", 
        &NtAttribute::getSourceType, 
        "Retrieves attribute source type.\n\n"
        ":Returns: attribute source type\n\n"
        "::\n\n"
        "    sourceType = attr.getSourceType()\n\n")

    .def("setSourceType", 
        &NtAttribute::setSourceType, 
        args("sourceType"), 
        "Sets attribute source type. The following values should be used: 0==NDAttrSourceDriver, 1==NDAttrSourceDriver, 2==NDAttrSourceEPICSPV, 3==NDAttrSourceFunct.\n\n"
        ":Parameter: *sourceType* (int) - attribute source type\n\n"
        "::\n\n"
        "    attr.setSourceType(2)\n\n")

    .def("getSource", 
        &NtAttribute::getSource, 
        "Retrieves attribute source.\n\n"
        ":Returns: attribute source\n\n"
        "::\n\n"
        "    source = attr.getSource()\n\n")

    .def("setSource", 
        &NtAttribute::setSource, 
        args("source"), 
        "Sets attribute source.\n\n"
        ":Parameter: *source* (str) - attribute source\n\n"
        "::\n\n"
        "    attr.setSource('Detector')\n\n")

;

} // wrapNtAttribute()


