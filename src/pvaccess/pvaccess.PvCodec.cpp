// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvCodec.h"

using namespace boost::python;

//
// PV Codec class
//
void wrapPvCodec() 
{

class_<PvCodec, bases<PvObject> >("PvCodec", 
    "PvCodec represents PV codec structure.\n\n"
    "**PvCodec()**\n\n"
    "\t::\n\n"
    "\t\tcodec1 = PvCodec()\n\n"
    "**PvCodec(name, parameters)**\n\n"
    "\t:Parameter: *name* (str) - codec name\n\n"
    "\t:Parameter: *parameters* (PvObject) - codec parameters\n\n"
    "\t::\n\n"
    "\t\tcodec2 = PvCodec('jpeg', PvObject({'value':{'compressor':STRING,'compressionFactor':FLOAT,'quality': INT}},{'value':{'compressor':'BloscLZ','compressionFactor':1.0,'quality':75}}))\n\n", 
    init<>())

    .def(init<const std::string&, const PvObject&>())

    .def(init<const PvObject&>())

    .def("getName", 
        &PvCodec::getName, 
        "Retrieves codec name.\n\n"
        ":Returns: codec name\n\n"
        "::\n\n"
        "    name = codec.getName()\n\n")

    .def("setName", 
        &PvCodec::setName, 
        args("name"), 
        "Sets codec name.\n\n"
        ":Parameter: *name* (str) - codec name\n\n"
        "::\n\n"
        "    codec.setName('codec name')\n\n")

    .def("getParameters", 
        &PvCodec::getParameters, 
        "Retrieves codec parameters.\n\n"
        ":Returns: codec parameters\n\n"
        "::\n\n"
        "    p = codec.getParameters()\n\n")

    .def("setParameters", static_cast<void(PvObject::*)(const PvObject&)>(&PvCodec::setParameters),
        args("valueObject"),
        "Sets parameters field.\n\n"
        ":Parameter: *valueObject* (PvObject) - object describing codec parameters\n\n"
        "::\n\n"
        "    p = PvObject({'value':{'compressor':STRING,'compressionFactor':FLOAT,'quality': INT}},{'value':{'compressor':'BloscLZ','compressionFactor':1.0,'quality':75}}))\n\n"
        "    codec.setParameters(p)\n\n")

;

} // wrapPvCodec()


