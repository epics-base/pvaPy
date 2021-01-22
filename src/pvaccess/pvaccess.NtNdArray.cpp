// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "NtNdArray.h"

using namespace boost::python;

// 
// NT ND Array class
// 
void wrapNtNdArray()
{

class_<NtNdArray, bases<NtType> >("NtNdArray", 
    "NtNdArray represents NT table structure.\n\n"
    "**NtNdArray()**\n\n"
    "\t::\n\n"
    "\t\ta1 = NtNdArray()\n\n"
    "\t**NtNdArray(extraFieldsDict)**\n\n"
    "\t:Parameter: *extraFieldsDict* (dict) - dictionary of key:value pairs describing the additional PV structure fields in terms of field names and their types\n\n"
    "\t::\n\n"
    "\t\ta2 = NtNdArray({'detectorName' : STRING, 'imageCrc' : STRING})\n\n"
    "\t**NtNdArray(pvObject)**\n\n"
    "\t:Parameter: *pvObject* (PvObject) - PV object that has a structure containing the required NT ND Array elements\n\n"
    "\t::\n\n"
    "\t\ta3 = NtNdArray(pvObject)\n\n", 
    init<>())

    .def(init<const dict&>())

    .def(init<const PvObject&>())

    .def("getValue", 
        &NtNdArray::getValue, 
        "Retrieves array value.\n\n"
        ":Returns: array value\n\n"
        "::\n\n"
        "    value = a.getValue()\n\n")

    .def("setValue", static_cast<void(NtNdArray::*)(const boost::python::dict&)>(&NtNdArray::setValue),
        args("valueDict"),
        "Sets array value.\n\n"
        ":Parameter: *valueDict* (dict) - array value dictionary (must contain array value with one of the allowed field names: booleanValue, byteValue, ubyteValue, shortValue, uShortValue, intValue, uintValue, longValue, ulongValue, floatValue, doubleValue)\n\n"
        ":Raises: *InvalidDataType* - when object's field name/type do not match allowed fields\n\n"
        "::\n\n"
        "    array.setValue({'byteValue' : [34, 56, 77, ... ]})\n\n")

    .def("setValue", static_cast<void(NtNdArray::*)(const PvObject&)>(&NtNdArray::setValue),
        args("valueObject"),
        "Sets array value.\n\n"
        ":Parameter: *valueObject* (PvObject) - array value object (must contain array value with one of the allowed field names: booleanValue, byteValue, ubyteValue, shortValue, uShortValue, intValue, uintValue, longValue, ulongValue, floatValue, doubleValue)\n\n"
        ":Raises: *InvalidDataType* - when object's field name/type do not match allowed fields\n\n"
        "::\n\n"
        "    array.setValue(PvObject({'byteValue' : [BYTE]}, {'byteValue' : [34, 56, 77, ... ]}))\n\n")

    .def("getCodec", 
        &NtNdArray::getCodec, 
        "Retrieves array codec.\n\n"
        ":Returns: array codec object\n\n"
        "::\n\n"
        "    codec = a.getCodec()\n\n")

    .def("setCodec", 
        &NtNdArray::setCodec, 
        args("codec"), 
        "Sets array codec.\n\n"
        ":Parameter: *codec* (PvCodec) - array codec object\n\n"
        "::\n\n"
        "    codec = PvCodec('pvapyc', PvInt(14))\n\n"
        "    a.setCodec(codec)\n\n")

    .def("getCompressedSize", 
        &NtNdArray::getCompressedSize, 
        "Retrieves array compressed size.\n\n"
        ":Returns: array compressed size\n\n"
        "::\n\n"
        "    cSize = a.getCompressedSize()\n\n")

    .def("setCompressedSize", 
        &NtNdArray::setCompressedSize, 
        args("value"), 
        "Sets array compressedSize.\n\n"
        ":Parameter: *value* (int) - array compressed size\n\n"
        "::\n\n"
        "    a.setCompressedSize(123456)\n\n")

    .def("getUncompressedSize", 
        &NtNdArray::getUncompressedSize, 
        "Retrieves array uncompressed size.\n\n"
        ":Returns: array uncompressed size\n\n"
        "::\n\n"
        "    ucSize = a.getUncompressedSize()\n\n")

    .def("setUncompressedSize", 
        &NtNdArray::setUncompressedSize, 
        args("value"), 
        "Sets array uncompressedSize.\n\n"
        ":Parameter: *value* (int) - array uncompressed size\n\n"
        "::\n\n"
        "    a.setUncompressedSize(123456)\n\n")

    .def("setDimension",
        static_cast<void(NtNdArray::*)(const boost::python::list&)>(&NtNdArray::setDimension),
        args("dimList"),
        "Sets array dimension list.\n\n"
        ":Parameter: *dimList* (list) - list of PvDimension objects\n\n"
        ":Raises: *InvalidDataType* - when list objects do not match required type\n\n"
        "::\n\n"
        "    array.setDimension([PvDimension(1024, 0, 1024, 1, False), PvDimension(1024, 0, 1024, 1, False)])\n\n")

    .def("getDimension",
        static_cast<boost::python::list(NtNdArray::*)()const>(&NtNdArray::getDimension),
        "Retrieves array dimension list.\n\n"
        ":Returns: list of array dimension objects\n\n"
        "::\n\n"
        "    dims = array.getDimension()\n\n")

    .def("getUniqueId", 
        &NtNdArray::getUniqueId, 
        "Retrieves array id.\n\n"
        ":Returns: array id\n\n"
        "::\n\n"
        "    id = a.getUniqueId()\n\n")

    .def("setUniqueId", 
        &NtNdArray::setUniqueId, 
        args("id"), 
        "Sets array id.\n\n"
        ":Parameter: *id* (int) - array id\n\n"
        "::\n\n"
        "    a.setUniqueId(123456)\n\n")

    .def("getDataTimeStamp", 
        &NtNdArray::getDataTimeStamp, 
        "Retrieves array data time stamp.\n\n"
        ":Returns: array data time stamp object\n\n"
        "::\n\n"
        "    dataTimeStamp = a.getDataTimeStamp()\n\n")

    .def("setDataTimeStamp", 
        &NtNdArray::setDataTimeStamp, 
        args("timeStamp"), 
        "Sets array data time stamp.\n\n"
        ":Parameter: *timeStamp* (PvTimeStamp) - array time stamp object\n\n"
        "::\n\n"
        "    dataTimeStamp = PvTimeStamp(1234567890, 10000, 1)\n\n"
        "    a.setDataTimeStamp(dataTimeStamp)\n\n")

    .def("setAttribute",
        static_cast<void(NtNdArray::*)(const boost::python::list&)>(&NtNdArray::setAttribute),
        args("attrList"),
        "Sets array attribute list.\n\n"
        ":Parameter: *attrList* (list) - list of NtAttribute objects\n\n"
        ":Raises: *InvalidDataType* - when list objects do not match required type\n\n"
        "::\n\n"
        "    array.setAttribute([NtAttribute('ColorMode', PvInt(0))])\n\n")

    .def("getAttribute",
        static_cast<boost::python::list(NtNdArray::*)()const>(&NtNdArray::getAttribute),
        "Retrieves array attribute list.\n\n"
        ":Returns: list of array attribute objects\n\n"
        "::\n\n"
        "    attrs = array.getAttribute()\n\n")

    .def("getDescriptor", 
        &NtNdArray::getDescriptor, 
        "Retrieves array descriptor.\n\n"
        ":Returns: array descriptor\n\n"
        "::\n\n"
        "    descriptor = a.getDescriptor()\n\n")

    .def("setDescriptor", 
        &NtNdArray::setDescriptor, 
        args("descriptor"), 
        "Sets array descriptor.\n\n"
        ":Parameter: *descriptor* (str) - array descriptor\n\n"
        "::\n\n"
        "    a.setDescriptor('Test Array')\n\n")

    .def("getAlarm", 
        &NtNdArray::getAlarm, 
        "Retrieves array alarm.\n\n"
        ":Returns: array alarm object\n\n"
        "::\n\n"
        "    alarm = a.getAlarm()\n\n")

    .def("setAlarm", 
        &NtNdArray::setAlarm, 
        args("alarm"), 
        "Sets array alarm.\n\n"
        ":Parameter: *alarm* (PvAlarm) - array alarm object\n\n"
        "::\n\n"
        "    alarm = PvAlarm(11, 126, 'Server SegFault')\n\n"
        "    a.setAlarm(alarm)\n\n")

    .def("getTimeStamp", 
        &NtNdArray::getTimeStamp, 
        "Retrieves array time stamp.\n\n"
        ":Returns: array time stamp object\n\n"
        "::\n\n"
        "    timeStamp = a.getTimeStamp()\n\n")

    .def("setTimeStamp", 
        &NtNdArray::setTimeStamp, 
        args("timeStamp"), 
        "Sets array time stamp.\n\n"
        ":Parameter: *timeStamp* (PvTimeStamp) - array time stamp object\n\n"
        "::\n\n"
        "    timeStamp = PvTimeStamp(1234567890, 10000, 1)\n\n"
        "    a.setTimeStamp(timeStamp)\n\n")

    .def("getDisplay", 
        &NtNdArray::getDisplay, 
        "Retrieves array display.\n\n"
        ":Returns: array display object\n\n"
        "::\n\n"
        "    display = a.getDisplay()\n\n")

    .def("setDisplay", 
        &NtNdArray::setDisplay, 
        args("display"), 
        "Sets array display.\n\n"
        ":Parameter: *display* (PvDisplay) - array display object\n\n"
        "::\n\n"
        "    display = PvDisplay(10, 100, 'Test Display', 'Test Format', 'Seconds')\n\n"
        "    a.setDisplay(display)\n\n")

;

} // wrapNtNdArray()

