// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "boost/python/self.hpp"
#include "boost/python/operators.hpp"
#include "PvObject.h"
#include "PvObjectPickleSuite.h"

using namespace boost::python;

//
// PvObject class
//
void wrapPvObject()
{

class_<PvObject>("PvObject", 
    "PvObject represents a generic PV structure.\n\n"
    "**PvObject(structureDict [,valueDict][,typeId])**\n\n"
    "\t:Parameter: *structureDict* (dict) - dictionary of key:value pairs describing the underlying PV structure in terms of field names and their types\n\n"
    "\tThe dictionary key is a string (PV field name), and value is one of:\n\n"
    "\t- PVTYPE: scalar type, can be BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING\n"
    "\t- [PVTYPE]: single element list representing scalar array\n"
    "\t- {key:value, ...}: structure\n"
    "\t- [{key:value, ...}]: single element list representing structure array\n"
    "\t- (): variant union\n"
    "\t- [()]: single element list representing variant union array\n"
    "\t- ({key:value, ...},): restricted union\n"
    "\t- [({key:value, ...},)]: single element list representing restricted union array\n\n"
    "\t:Parameter: *valueDict* (dict) - (optional) dictionary of key:value pairs to be used to set field values in the underlying PV structure\n\n"
    "\t:Raises: *InvalidArgument* - in case structure dictionary cannot be parsed\n\n"
    "\t:Parameter: *typeId* (str) - (optional) The type ID string of the PV structure\n\n"
    "\tExamples of PvObject initialization: ::\n\n"
    "\t\tpv1 = PvObject({'anInt' : INT})\n\n"
    "\t\tpv2 = PvObject({'aShort' : SHORT, 'anUInt' : UINT, 'aString' : STRING})\n\n"
    "\t\tpv3 = PvObject({'aStringArray' : [STRING], 'aStruct' : {'aString2' : STRING, 'aBoolArray' : [BOOLEAN], 'aStruct2' : {'aFloat' : FLOAT, 'aString3' : [STRING]}}})\n\n"
    "\t\tpv4 = PvObject({'aStructArray' : [{'anInt' : INT, 'anInt2' : INT, 'aDouble' : DOUBLE}]})\n\n" 
    "\t\tpv5 = PvObject({'anUnion' : ({'anInt' : INT, 'aDouble' : DOUBLE},)})\n\n" 
    "\t\tpv6 = PvObject({'aVariant' : ()})\n\n" 
    "\t\tpv7 = PvObject({'value' : DOUBLE}, 'epics:nt/NTScalar:1.0')\n\n"
    "\tIn addition to various set/get methods described below,\n"
    "\tPvObject elements can be accessed and manipulated similar to dictionaries: ::\n\n"
    "\t\t>>> pv = PvObject({'a' : {'b' : STRING, 'c' : FLOAT}}, {'a' : {'b' : 'my string', 'c' : 10.1}})\n"
    "\t\t>>> print pv\n"
    "\t\tstructure\n"
    "\t\t    structure a\n"
    "\t\t        float c 10.1\n"
    "\t\t        string b my string\n"
    "\t\t>>> pv['a.b']\n"
    "\t\t'my string'\n"
    "\t\t>>> pv['a.c']\n"
    "\t\t10.1\n"
    "\t\t \n"
    "\t\t>>> print 'a.b' in pv\n"
    "\t\tTrue\n"
    "\t\t>>> print 'a.d' in pv\n"
    "\t\tFalse\n"
    "\t\t \n"
    "\t\t>>> pv['a.b'] = 'updated string'\n"
    "\t\t>>> pv['a.c'] = 20.2\n"
    "\t\t>>> print pv\n"
    "\t\tstructure\n"
    "\t\t    structure a\n"
    "\t\t        float c 20.2\n"
    "\t\t        string b updated string\n\n"
    "\tNote that compiling pvaPy with Boost.NumPy allows one to retrieve\n"
    "\tnumeric scalar arrays as read-only NumPy arrays: ::\n\n"
    "\t\t>>> pv = PvObject({'a' : {'b' : STRING, 'c' : [INT]}}, {'a' : {'b' : 'my string', 'c' : [1,2,3,4,5]}})\n"
    "\t\t>>> print pv\n"
    "\t\tstructure\n"
    "\t\t    structure a\n"
    "\t\t        int[] c [1,2,3,4,5]\n"
    "\t\t        string b my string\n"
    "\t\t>>> print pv.useNumPyArrays\n"
    "\t\tTrue\n"
    "\t\t>>> c = pv['a.c']\n"
    "\t\t>>> c\n"
    "\t\tarray([1, 2, 3, 4, 5], dtype=int32)\n"
    "\t\t>>> type(c)\n"
    "\t\t<type 'numpy.ndarray'>\n"
    "\t\t \n"
    "\t\t>>> pv.useNumPyArrays = False\n"
    "\t\t>>> c2 = pv['a.c']\n"
    "\t\t>>> c2\n"
    "\t\t>>> [1, 2, 3, 4, 5]\n"
    "\t\t>>> type(c2)\n"
    "\t\t<type 'list'>\n"
    "\n\n", 
    init<boost::python::dict>(args("structureDict")))

    .def(init<boost::python::dict,const std::string &>(args("structureDict","typeId")))

    .def(init<boost::python::dict,boost::python::dict>(args("structureDict","valueDict")))


    .def(init<boost::python::dict,boost::python::dict,const std::string>(args("structureDict","valueDict","typeId")))

    .def_pickle(PvObjectPickleSuite())

    .def(str(self))

    .def("set", 
        static_cast<void(PvObject::*)(const boost::python::dict&)>(&PvObject::set),
        args("valueDict"),
        "Populates PV structure fields from python dictionary.\n\n"
        ":Parameter: *valueDict* (dict) - dictionary of key:value pairs that correspond to PV structure field names and their values\n\n"
        ":Raises: *FieldNotFound* - in case PV structure does not have one of the dictionary keys\n"
        ":Raises: *InvalidDataType* - in case PV structure field type does not match type of the corresponding dictionary value\n\n"
        "::\n\n"
        "    pv = PvObject({'anUInt' : UINT, 'aString' : STRING})\n\n"
        "    pv.set({'anUInt' : 1, 'aString' : 'my string example'})\n\n")

    .def("get", 
        static_cast<boost::python::dict(PvObject::*)()const>(&PvObject::get), 
        "Retrieves PV structure as python dictionary.\n\n"
        ":Returns: python key:value dictionary representing current PV structure in terms of field names and their values\n\n"
        "::\n\n"
        "    pv = PvObject({'anInt' : INT, 'aString' : STRING})\n\n"
        "    pv.set({'anInt' : 1})\n\n"
        "    valueDict = pv.get()\n\n")

    .def("setPyObject", 
        static_cast<void(PvObject::*)(const boost::python::object&)>(&PvObject::setPyObject),
        args("value"),
        "Sets value for a single-field structure, or for a structure that has field named 'value'.\n\n"
        ":Parameter: *value* (object) - value object\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no field or multiple-field structure has no 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aString' : STRING})\n\n"
        "    pv.setPyObject('string value')\n\n")

    .def("hasField", 
        static_cast<bool(PvObject::*)(const std::string&)const>(&PvObject::hasField),
        args("fieldPath"),
        "Checks if the PV object has field specified by the given path, using '.' as the field name separator.\n\n"
        ":Parameter: *fieldPath* (str) - field path\n\n"
        ":Returns: true if path exists, false otherwise\n\n"
        "::\n\n"
        "    pv = PvObject({'aString' : STRING, 'aStruct' : {'anInt' : INT, 'aString2' : STRING}})\n\n"
        "    hasField = pv.hasField('aString')\n\n"
        "    hasField2 = pv.hasField('aString.anInt')\n\n")

    .def("__contains__", 
        static_cast<bool(PvObject::*)(const std::string&)const>(&PvObject::hasField),
        args("fieldPath"),
        "Checks if the PV object has field specified by the given path, using '.' as the field name separator.\n\n"
        ":Parameter: *fieldPath* (str) - field path\n\n"
        ":Returns: true if path exists, false otherwise\n\n"
        "::\n\n"
        "    pv = PvObject({'aString' : STRING, 'aStruct' : {'anInt' : INT, 'aString2' : STRING}})\n\n"
        "    hasField = 'aString' in pv\n\n"
        "    hasField2 = 'aString.anInt' in pv\n\n")

    .def("setPyObject", 
        static_cast<void(PvObject::*)(const std::string&,const boost::python::object&)>(&PvObject::setPyObject),
        args("fieldPath", "value"),
        "Sets value for the PV field specified by the given field path, using '.' as the field name separator.\n\n"
        ":Parameter: *fieldPath* (str) - field path\n\n"
        ":Parameter: *value* (object) - value object\n\n"
        ":Raises: *FieldNotFound* - when a part of the specified field path is not found\n\n"
        ":Raises: *InvalidRequest* - when specified field does not match provided object type\n\n"
        "::\n\n"
        "    pv = PvObject({'aString' : STRING, 'aStruct' : {'anInt' : INT, 'aString2' : STRING}})\n\n"
        "    pv.setPyObject('aString', 'string value')\n\n"
        "    pv.setPyObject('aString.aString2', 'string value2')\n\n")

    .def("__setitem__", 
        static_cast<void(PvObject::*)(const std::string&,const boost::python::object&)>(&PvObject::setPyObject),
        args("fieldPath", "value"),
        "Sets value for the PV field specified by the given field path, using '.' as the field name separator.\n\n"
        ":Parameter: *fieldPath* (str) - field path\n\n"
        ":Parameter: *value* (object) - value object\n\n"
        ":Raises: *FieldNotFound* - when a part of the specified field path is not found\n\n"
        ":Raises: *InvalidRequest* - when specified field does not match provided object type\n\n"
        "::\n\n"
        "    pv = PvObject({'aString' : STRING, 'aStruct' : {'anInt' : INT, 'aString2' : STRING}})\n\n"
        "    pv['aString'] = 'string value'\n\n"
        "    pv['aString.aString2'] = 'string value2'\n\n")

    .def("getPyObject", 
        static_cast<boost::python::object(PvObject::*)()const>(&PvObject::getPyObject),
        "Retrieves value object from a single-field structure, or from a structure that has field named 'value'.\n\n"
        ":Returns: value object\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no field or multiple-field structure has no 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aString' : STRING})\n\n"
        "    value = pv.getPyObject()\n\n")
 
    .def("getPyObject", 
        static_cast<boost::python::object(PvObject::*)(const std::string&)const>(&PvObject::getPyObject),
        args("fieldPath"), 
        "Retrieves value object assigned to the given PV field path, which uses '.' as the field name separator.\n\n"
        ":Parameter: *fieldPath* (str) - field path\n\n"
        ":Returns: value object\n\n"
        ":Raises: *FieldNotFound* - when a part of the specified field path is not found\n\n"
        "::\n\n"
        "    pv = PvObject({'aString' : STRING, 'aStruct' : {'anInt' : INT, 'aString2' : STRING}})\n\n"
        "    value = pv.getPyObject('aString')\n\n"
        "    value2 = pv.getPyObject('aStruct.aString2')\n\n")

    .def("__getitem__", 
        static_cast<boost::python::object(PvObject::*)(const std::string&)const>(&PvObject::getPyObject),
        args("fieldPath"), 
        "Retrieves value object assigned to the given PV field path, which uses '.' as the field name separator.\n\n"
        ":Parameter: *fieldPath* (str) - field path\n\n"
        ":Returns: value object\n\n"
        ":Raises: *FieldNotFound* - when a part of the specified field path is not found\n\n"
        "::\n\n"
        "    pv = PvObject({'aString' : STRING, 'aStruct' : {'anInt' : INT, 'aString2' : STRING}})\n\n"
        "    value = pv['aString']\n\n"
        "    value2 = pv['aStruct.aString2']\n\n")

    .def("setBoolean", 
        static_cast<void(PvObject::*)(bool)>(&PvObject::setBoolean),
        args("value"),
        "Sets boolean value for a single-field structure, or for a structure that has boolean field named 'value'.\n\n"
        ":Parameter: *value* (bool) - boolean value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no boolean field or multiple-field structure has no boolean 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aBoolean' : BOOLEAN})\n\n"
        "    pv.setBoolean(True)\n\n")

    .def("setBoolean", 
        static_cast<void(PvObject::*)(const std::string&,bool)>(&PvObject::setBoolean),
        args("fieldName", "value"),
        "Sets boolean value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *value* (bool) - boolean value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a boolean\n\n"
        "::\n\n"
        "    pv = PvObject({'aBoolean' : BOOLEAN, 'aString' : STRING})\n\n"
        "    pv.setBoolean('aBoolean', True)\n\n")

    .def("getBoolean", 
        static_cast<bool(PvObject::*)()const>(&PvObject::getBoolean),
        "Retrieves boolean value from a single-field structure, or from a structure that has boolean field named 'value'.\n\n"
        ":Returns: boolean value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no boolean field or multiple-field structure has no boolean 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aBoolean' : BOOLEAN})\n\n"
        "    value = pv.getBoolean()\n\n")
 
    .def("getBoolean", 
        static_cast<bool(PvObject::*)(const std::string&)const>(&PvObject::getBoolean),
        args("fieldName"), 
        "Retrieves boolean value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: boolean value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a boolean\n\n"
        "::\n\n"
        "    pv = PvObject({'aBoolean' : BOOLEAN, 'aString' : STRING})\n\n"
        "    value = pv.getBoolean('aBoolean')\n\n")

    .def("setByte", 
        static_cast<void(PvObject::*)(char)>(&PvObject::setByte),
        args("value"),
        "Sets byte (character) value for a single-field structure, or for a structure that has byte field named 'value'.\n\n"
        ":Parameter: *value* (str) - byte value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no byte field or multiple-field structure has no byte 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aByte' : BYTE})\n\n"
        "    pv.setByte('a')\n\n")

    .def("setByte", static_cast<void(PvObject::*)(const std::string&,char)>(&PvObject::setByte),
        args("fieldName", "value"),
        "Sets byte (character) value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *value* (str) - byte value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a byte\n\n"
        "::\n\n"
        "    pv = PvObject({'aByte' : BYTE, 'aString' : STRING})\n\n"
        "    pv.setByte('aByte', 'a')\n\n")

    .def("getByte", 
        static_cast<char(PvObject::*)()const>(&PvObject::getByte), 
        "Retrieves byte (character) value from a single-field structure, or from a structure that has byte field named 'value'.\n\n"
        ":Returns: byte value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no byte field or multiple-field structure has no byte 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aByte' : BYTE})\n\n"
        "    value = pv.getByte()\n\n")
        
    .def("getByte", 
        static_cast<char(PvObject::*)(const std::string&)const>(&PvObject::getByte), 
        args("fieldName"), 
        "Retrieves byte (character) value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: byte value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a byte\n\n"
        "::\n\n"
        "    pv = PvObject({'aByte' : BYTE, 'aString' : STRING})\n\n"
        "    value = pv.getByte('aByte')\n\n")

    .def("setUByte", 
        static_cast<void(PvObject::*)(unsigned char)>(&PvObject::setUByte),
        args("value"),
        "Sets unsigned byte (character) value for a single-field structure, or for a structure that has unsigned byte field named 'value'.\n\n"
        ":Parameter: *value* (str) - unsigned byte value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no unsigned byte field or multiple-field structure has no unsigned byte 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUByte' : UBYTE})\n\n"
        "    pv.setUByte('a')\n\n")

    .def("setUByte", 
        static_cast<void(PvObject::*)(const std::string&,unsigned char)>(&PvObject::setUByte),
        args("fieldName", "value"),
        "Sets unsigned byte (character) value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *value* (str) - unsigned byte value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an unsigned byte\n\n"
        "::\n\n"
        "    pv = PvObject({'anUByte' : UBYTE, 'aString' : STRING})\n\n"
        "    pv.setUByte('anUByte', 'a')\n\n")

    .def("getUByte", static_cast<unsigned char(PvObject::*)()const>(&PvObject::getUByte), 
        "Retrieves unsigned byte (character) value from a single-field structure, or from a structure that has unsigned byte field named 'value'.\n\n"
        ":Returns: unsigned byte value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no unsigned byte field or multiple-field structure has no unsigned byte 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUByte' : UBYTE})\n\n"
        "    value = pv.getUByte()\n\n")

    .def("getUByte", static_cast<unsigned char(PvObject::*)(const std::string&)const>(&PvObject::getUByte), 
        args("fieldName"), 
        "Retrieves unsigned byte (character) value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: unsigned byte value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an unsigned byte\n\n"
        "::\n\n"
        "    pv = PvObject({'anUByte' : UBYTE, 'aString' : STRING})\n\n"
        "    value = pv.getUByte('anUByte')\n\n")

    .def("setShort", 
        static_cast<void(PvObject::*)(short)>(&PvObject::setShort),
        args("value"),
        "Sets short value for a single-field structure, or for a structure that has short field named 'value'.\n\n"
        ":Parameter: *value* (int) - short value'\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no short field or multiple-field structure has no short 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aShort' : SHORT})\n\n"
        "    pv.setShort(10)\n\n")

    .def("setShort", 
        static_cast<void(PvObject::*)(const std::string&,short)>(&PvObject::setShort),
        args("fieldName", "value"),
        "Sets short value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *value* (int) - short value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a short\n\n"
        "::\n\n"
        "    pv = PvObject({'aShort' : SHORT, 'aString' : STRING})\n\n"
        "    pv.setShort('aShort', 10)\n\n")

    .def("getShort", static_cast<short(PvObject::*)()const>(&PvObject::getShort), 
        "Retrieves short value from a single-field structure, or from a structure that has short field named 'value'.\n\n"
        ":Returns: short value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no short field or multiple-field structure has no short 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aShort' : SHORT})\n\n"
        "    value = pv.getShort()\n\n")

    .def("getShort", static_cast<short(PvObject::*)(const std::string&)const>(&PvObject::getShort), 
        args("fieldName"), 
        "Retrieves short value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: short value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a short\n\n"
        "::\n\n"
        "    pv = PvObject({'aShort' : SHORT, 'aString' : STRING})\n\n"
        "    value = pv.getShort('aShort')\n\n")

    .def("setUShort", 
        static_cast<void(PvObject::*)(unsigned short)>(&PvObject::setUShort),
        args("value"),
        "Sets unsigned short value for a single-field structure, or for a structure that has unsigned short field named 'value'.\n\n"
        ":Parameter: *value* (int) - unsigned short value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no unsigned short field or multiple-field structure has no unsigned short 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUShort' : USHORT})\n\n"
        "    pv.setUShort(10)\n\n")

    .def("setUShort", 
        static_cast<void(PvObject::*)(const std::string&,unsigned short)>(&PvObject::setUShort),
        args("fieldName", "value"),
        "Sets unsigned short value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *value* (int) - unsigned short value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an unsigned short\n\n"
        "::\n\n"
        "    pv = PvObject({'anUShort' : USHORT, 'aString' : STRING})\n\n"
        "    pv.setUShort('anUShort', 10)\n\n")

    .def("getUShort", static_cast<unsigned short(PvObject::*)()const>(&PvObject::getUShort),
        "Retrieves unsigned short value from a single-field structure, or from a structure that has unsigned short field named 'value'.\n\n"
        ":Returns: unsigned short value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no unsigned short field or multiple-field structure has no unsigned short 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUShort' : USHORT})\n\n"
        "    value = pv.getUShort()\n\n")

    .def("getUShort", static_cast<unsigned short(PvObject::*)(const std::string&)const>(&PvObject::getUShort),
        args("fieldName"), 
        "Retrieves unsigned short value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: unsigned short value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an unsigned short\n\n"
        "::\n\n"
        "    pv = PvObject({'anUShort' : USHORT, 'aString' : STRING})\n\n"
        "    value = pv.getUShort('anUShort')\n\n")

    .def("setInt", 
        static_cast<void(PvObject::*)(int)>(&PvObject::setInt),
        args("value"),
        "Sets int value for a single-field structure, or for a structure that has int field named 'value'.\n\n"
        ":Parameter: *value* (int) - integer value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no int field or multiple-field structure has no int 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anInt' : INT})\n\n"
        "    pv.setInt(10)\n\n")

    .def("setInt", 
        static_cast<void(PvObject::*)(const std::string&,int)>(&PvObject::setInt),
        args("fieldName", "value"),
        "Sets int value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *value* (int) - integer value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an int\n\n"
        "::\n\n"
        "    pv = PvObject({'anInt' : INT, 'aString' : STRING})\n\n"
        "    pv.setInt('anInt', 10)\n\n")

    .def("getInt", 
        static_cast<int(PvObject::*)()const>(&PvObject::getInt), 
        "Retrieves int value from a single-field structure, or from a structure that has int field named 'value'.\n\n"
        ":Returns: int value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no int field or multiple-field structure has no int 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anInt' : INT})\n\n"
        "    value = pv.getInt()\n\n")

    .def("getInt", 
        static_cast<int(PvObject::*)(const std::string&)const>(&PvObject::getInt), 
        args("fieldName"), 
        "Retrieves int value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: int value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an int\n\n"
        "::\n\n"
        "    pv = PvObject({'anInt' : INT, 'aString' : STRING})\n\n"
        "    value = pv.getInt('anInt')\n\n")

    .def("setUInt", 
        static_cast<void(PvObject::*)(unsigned int)>(&PvObject::setUInt),
        args("value"),
        "Sets unsigned int value for a single-field structure, or for a structure that has unsigned int field named 'value'.\n\n"
        ":Parameter: *value* (int) - unsigned integer value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no unsigned int field or multiple-field structure has no unsigned int 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUInt' : UINT})\n\n"
        "    pv.setUInt(10)\n\n")

    .def("setUInt", 
        static_cast<void(PvObject::*)(const std::string&,unsigned int)>(&PvObject::setUInt),
        args("fieldName", "value"),
        "Sets unsigned int value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *value* (int) - unsigned integer value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an unsigned int\n\n"
        "::\n\n"
        "    pv = PvObject({'anUInt' : UINT, 'aString' : STRING})\n\n"
        "    pv.setUInt('anInt', 10)\n\n")

    .def("getUInt", 
        static_cast<unsigned int(PvObject::*)()const>(&PvObject::getUInt),
        args("fieldName"), 
        "Retrieves unsigned int value from a single-field structure, or from a structure that has unsigned int field named 'value'.\n\n"
        ":Returns: unsigned integer value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no unsigned int field or multiple-field structure has no unsigned int 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUInt' : UINT})\n\n"
        "    value = pv.getUInt()\n\n")

    .def("getUInt", 
        static_cast<unsigned int(PvObject::*)(const std::string&)const>(&PvObject::getUInt),
        args("fieldName"), 
        "Retrieves unsigned int value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: unsigned integer value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an unsigned int\n\n"
        "::\n\n"
        "    pv = PvObject({'anUInt' : UINT, 'aString' : STRING})\n\n"
        "    value = pv.getUInt('anUInt')\n\n")

    .def("setLong", 
        static_cast<void(PvObject::*)(long long)>(&PvObject::setLong),
        args("value"),
        "Sets long value for a single-field structure, or for a structure that has long field named 'value'.\n\n"
        ":Parameter: *value* (long) - long value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no long field or multiple-field structure has no long 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aLong' : LONG})\n\n"
        "    pv.setLong(10L)\n\n")

    .def("setLong", 
        static_cast<void(PvObject::*)(const std::string&,long long)>(&PvObject::setLong),
        args("fieldName", "value"),
        "Sets long value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *value* (long) - long value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a long\n\n"
        "::\n\n"
        "    pv = PvObject({'aLong' : LONG, 'aString' : STRING})\n\n"
        "    pv.setLong('aLong', 10L)\n\n")

    .def("getLong", 
        static_cast<long long(PvObject::*)()const>(&PvObject::getLong),
        "Retrieves long value from a single-field structure, or from a structure that has long field named 'value'.\n\n"
        ":Returns: long value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no long field or multiple-field structure has no long 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aLong' : LONG})\n\n"
        "    value = pv.getLong()\n\n")

    .def("getLong", 
        static_cast<long long(PvObject::*)(const std::string&)const>(&PvObject::getLong),
        args("fieldName"), 
        "Retrieves short value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: long value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a long\n\n"
        "::\n\n"
        "    pv = PvObject({'aLong' : LONG, 'aString' : STRING})\n\n"
        "    value = pv.getLong('aLong')\n\n")

    .def("setULong", 
        static_cast<void(PvObject::*)(unsigned long long)>(&PvObject::setULong),
        args("value"),
        "Sets unsigned long value for a single-field structure, or for a structure that has unsigned long field named 'value'.\n\n"
        ":Parameter: *value* (long) - unsigned long value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no unsigned long field or multiple-field structure has no unsigned long 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anULong' : ULONG})\n\n"
        "    pv.setULong(10L)\n\n")

    .def("setULong", 
        static_cast<void(PvObject::*)(const std::string&,unsigned long long)>(&PvObject::setULong),
        args("fieldName", "value"),
        "Sets unsigned long value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *value* (long) - unsigned long value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an unsigned long\n\n"
        "::\n\n"
        "    pv = PvObject({'anULong' : ULONG, 'aString' : STRING})\n\n"
        "    pv.setULong('anULong', 10L)\n\n")

    .def("getULong", 
        static_cast<unsigned long long(PvObject::*)()const>(&PvObject::getULong), 
        "Retrieves unsigned long value from a single-field structure, or from a structure that has unsigned long field named 'value'.\n\n"
        ":Returns: unsigned long value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no unsigned long field or multiple-field structure has no unsigned long 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anULong' : ULONG})\n\n"
        "    value = pv.getULong()\n\n")

    .def("getULong", 
        static_cast<unsigned long long(PvObject::*)(const std::string&)const>(&PvObject::getULong), 
        args("fieldName"), 
        "Retrieves unsigned long value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: unsigned long value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an unsigned long\n\n"
        "::\n\n"
        "    pv = PvObject({'anULong' : ULONG, 'aString' : STRING})\n\n"
        "    value = pv.getULong('anULong')\n\n")

    .def("setFloat", 
        static_cast<void(PvObject::*)(float)>(&PvObject::setFloat),
        args("value"),
        "Sets float value for a single-field structure, or for a structure that has float field named 'value'.\n\n"
        ":Parameter: *value* (float) - float value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no float field or multiple-field structure has no float 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aFloat' : FLOAT})\n\n"
        "    pv.setFloat(10.0)\n\n")

    .def("setFloat", 
        static_cast<void(PvObject::*)(const std::string&,float)>(&PvObject::setFloat),
        args("fieldName", "value"),
        "Sets short value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *value* (float) - float value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a short\n\n"
        "::\n\n"
        "    pv = PvObject({'aFloat' : FLOAT, 'aString' : STRING})\n\n"
        "    pv.setFloat('aFloat', 10.0)\n\n")

    .def("getFloat", 
        static_cast<float(PvObject::*)()const>(&PvObject::getFloat), 
        "Retrieves float value from a single-field structure, or from a structure that has float field named 'value'.\n\n"
        ":Returns: float value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no float field or multiple-field structure has no float 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aFloat' : FLOAT})\n\n"
        "    value = pv.getFloat()\n\n")

    .def("getFloat", 
        static_cast<float(PvObject::*)(const std::string&)const>(&PvObject::getFloat), 
        args("fieldName"), 
        "Retrieves float value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: float value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a float\n\n"
        "::\n\n"
        "    pv = PvObject({'aFloat' : FLOAT, 'aString' : STRING})\n\n"
        "    value = pv.getFloat('aFloat')\n\n")

    .def("setDouble", static_cast<void(PvObject::*)(double)>(&PvObject::setDouble),
        args("value"),
        "Sets double value for a single-field structure, or for a structure that has double field named 'value'.\n\n"
        ":Parameter: *value* (float) - double value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no double field or multiple-field structure has no double 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aDouble' : DOUBLE})\n\n"
        "    pv.setDouble(10.0)\n\n")

    .def("setDouble", static_cast<void(PvObject::*)(const std::string&,double)>(&PvObject::setDouble),
        args("fieldName", "value"),
        "Sets short value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *value* (float) - double value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a double\n\n"
        "::\n\n"
        "    pv = PvObject({'aDouble' : DOUBLE, 'aString' : STRING})\n\n"
        "    pv.setDouble('aDouble', 10.0)\n\n")

    .def("getDouble", 
        static_cast<double(PvObject::*)()const>(&PvObject::getDouble), 
        "Retrieves double value from a single-field structure, or from a structure that has double field named 'value'.\n\n"
        ":Returns: double value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no double field or multiple-field structure has no double 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aDouble' : DOUBLE})\n\n"
        "    value = pv.getDouble()\n\n")

    .def("getDouble", 
        static_cast<double(PvObject::*)(const std::string&)const>(&PvObject::getDouble), 
        args("fieldName"), 
        "Retrieves double value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: double value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a double\n\n"
        "::\n\n"
        "    pv = PvObject({'aDouble' : DOUBLE, 'aString' : STRING})\n\n"
        "    value = pv.getDouble('aDouble')\n\n")

    .def("setString", 
        static_cast<void(PvObject::*)(const std::string&)>(&PvObject::setString),
        args("value"),
        "Sets string value for a single-field structure, or for a structure that has string field named 'value'.\n\n"
        ":Parameter: *value* (str) - string value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no string field or multiple-field structure has no string 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aString' : STRING})\n\n"
        "    pv.setString('string value')\n\n")

    .def("setString", 
        static_cast<void(PvObject::*)(const std::string&,const std::string&)>(&PvObject::setString),
        args("fieldName", "value"),
        "Sets string value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *value* (str) - string value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a string\n\n"
        "::\n\n"
        "    pv = PvObject({'aString' : STRING, 'anInt' : INT})\n\n"
        "    pv.setString('aString', 'string value')\n\n")

    .def("getString", static_cast<std::string(PvObject::*)()const>(&PvObject::getString),
        "Retrieves string value from a single-field structure, or from a structure that has string field named 'value'.\n\n"
        ":Returns: string value\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no string field or multiple-field structure has no string 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aString' : STRING})\n\n"
        "    value = pv.getString()\n\n")

    .def("getString", static_cast<std::string(PvObject::*)(const std::string&)const>(&PvObject::getString),
        args("fieldName"), 
        "Retrieves string value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: string value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a string\n\n"
        "::\n\n"
        "    pv = PvObject({'aString' : STRING, 'anInt' : INT})\n\n"
        "    value = pv.getString('aString')\n\n")

    .def("setScalarArray", 
        static_cast<void(PvObject::*)(const boost::python::list&)>(&PvObject::setScalarArray),
        args("valueList"),
        "Sets scalar array value for a single-field structure, or for a structure that has scalar array field named 'value'.\n\n"
        ":Parameter: *valueList* (list) - list of scalar values\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no scalar array field or multiple-field structure has no scalar array 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aScalarArray' : [INT]})\n\n"
        "    pv.setScalarArray([0,1,2,3,4])\n\n")

    .def("setScalarArray", 
        static_cast<void(PvObject::*)(const std::string&,const boost::python::list&)>(&PvObject::setScalarArray),
        args("fieldName", "valueList"),
        "Sets scalar array value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *valueList* (list) - list of scalar values\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a scalar array\n\n"
        "::\n\n"
        "    pv = PvObject({'aScalarArray' : [INT], 'aString' : STRING})\n\n"
        "    pv.setScalarArray('aScalarArray', [0,1,2,3,4])\n\n")

    .def("getScalarArray", 
        static_cast<boost::python::list(PvObject::*)()const>(&PvObject::getScalarArray), 
        "Retrieves scalar array value from a single-field structure, or from a structure that has scalar array field named 'value'.\n\n"
        ":Returns: list of scalar values\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no scalar array field or multiple-field structure has no scalar array 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aScalarArray' : [INT]})\n\n"
        "    valueList = pv.getScalarArray()\n\n")

    .def("getScalarArray", 
        static_cast<boost::python::list(PvObject::*)(const std::string&)const>(&PvObject::getScalarArray), 
        args("fieldName"), 
        "Retrieves scalar array value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: list of scalar values\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a scalar array\n\n"
        "Retrieves scalar array assigned to the given PV field.\n\n"
        "::\n\n"
        "    pv = PvObject({'aScalarArray' : [INT]})\n\n"
        "    valueList = pv.getScalarArray('aScalarArray', 'aString' : STRING)\n\n")

    .def("setStructure", 
        static_cast<void(PvObject::*)(const boost::python::dict&)>(&PvObject::setStructure),
        args("valueDict"),
        "Sets structure value for a single-field structure, or for a structure that has structure field named 'value'.\n\n"
        ":Parameter: *valueDict* (dict) - dictionary of structure key:value pairs\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no structure field or multiple-field structure has no structure 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aStruct' : {'anInt':INT, 'aDouble':DOUBLE}})\n\n"
        "    pv.setStructure({'anInt' : 1, 'aDouble' : 1.1})\n\n")

    .def("setStructure", 
        static_cast<void(PvObject::*)(const std::string&,const boost::python::dict&)>(&PvObject::setStructure),
        args("fieldName", "valueDict"),
        "Sets structure value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *valueDict* (dict) - dictionary of structure key:value pairs\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a structure\n\n"
        "::\n\n"
        "    pv = PvObject({'aStruct' : {'anInt':INT, 'aDouble':DOUBLE}, 'aString' : STRING})\n\n"
        "    pv.setStructure('aStruct', {'anInt' : 1, 'aDouble' : 1.1})\n\n")

    .def("getStructure", 
        static_cast<boost::python::dict(PvObject::*)()const>(&PvObject::getStructure), 
        "Retrieves structure value from a single-field structure, or from a structure that has structure field named 'value'.\n\n"
        ":Returns: dictionary of structure key:value pairs\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no structure field or multiple-field structure has no structure 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aStruct' : {'anInt':INT, 'aDouble':DOUBLE}})\n\n"
        "    valueDict = pv.getStructure()\n\n")

    .def("getStructure", 
        static_cast<boost::python::dict(PvObject::*)(const std::string&)const>(&PvObject::getStructure), 
        args("fieldName"), 
        "Retrieves structure value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: dictionary of structure key:value pairs\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a structure\n\n"
        "::\n\n"
        "    pv = PvObject({'aStruct' : {'anInt':INT, 'aDouble':DOUBLE}, 'aString' : STRING})\n\n"
        "    valueDict = pv.getStructure('aStruct')\n\n")

    .def("setStructureArray", 
        static_cast<void(PvObject::*)(const boost::python::list&)>(&PvObject::setStructureArray),
        args("dictList"),
        "Sets structure array value for a single-field structure, or for a structure that has structure array field named 'value'.\n\n"
        ":Parameter: *dictList* (list) - list of dictionaries\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no structure array field or multiple-field structure has no structure array 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aStructArray' : [{'anInt' : INT, 'aFloat' : FLOAT}]})\n\n"
        "    pv.setStructureArray([{'anInt' : 1, 'aFloat' : 1.1},{'anInt' : 2, 'aFloat' : 2.2},{'anInt' : 3, 'aFloat' : 3.3}])\n\n")

    .def("setStructureArray", 
        static_cast<void(PvObject::*)(const std::string&,const boost::python::list&)>(&PvObject::setStructureArray),
        args("fieldName", "dictList"),
        "Sets structure array value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *dictList* (list) - list of dictionaries\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a structure array\n\n"
        "::\n\n"
        "    pv = PvObject({'aStructArray' : [{'anInt' : INT, 'aFloat' : FLOAT}], 'aString' : STRING})\n\n"
        "    pv.setStructureArray('aStructArray', [{'anInt' : 1, 'aFloat' : 1.1},{'anInt' : 2, 'aFloat' : 2.2},{'anInt' : 3, 'aFloat' : 3.3}])\n\n")

    .def("getStructureArray", 
        static_cast<boost::python::list(PvObject::*)()const>(&PvObject::getStructureArray), 
        "Retrieves structure array value from a single-field structure, or from a structure that has structure array field named 'value'.\n\n"
        ":Returns: list of dictionaries\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no structure array field or multiple-field structure has no structure array 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'aStructArray' : [{'anInt' : INT, 'aFloat' : FLOAT}]})\n\n"
        "    dictList = pv.getStructureArray()\n\n")

    .def("getStructureArray", 
        static_cast<boost::python::list(PvObject::*)(const std::string&)const>(&PvObject::getStructureArray), 
        args("fieldName"), 
        "Retrieves structure array value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: list of dictionaries\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a structure array\n\n"
        "::\n\n"
        "    pv = PvObject({'aStructArray' : [{'anInt' : INT, 'aFloat' : FLOAT}], 'aString' : STRING})\n\n"
        "    dictList = pv.getStructureArray('aStructArray')\n\n")

    .def("setUnion", static_cast<void(PvObject::*)(const PvObject&)>(&PvObject::setUnion),
        args("valueObject"),
        "Sets union value for a single-field structure, or for a structure that has union field named 'value'.\n\n"
        ":Parameter: *valueObject* (PvObject) - union value\n\n"
        ":Raises: *InvalidArgument* - when object's field name/type do not match any of the union's fields\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union field or multiple-field structure has no union 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},)})\n\n"
        "    value = PvObject({'anInt' : INT})\n\n"
        "    value.setInt(10)\n\n"
        "    pv.setUnion(value)\n\n")

    .def("setUnion", static_cast<void(PvObject::*)(const std::string&,const PvObject&)>(&PvObject::setUnion),
        args("fieldName", "valueObject"),
        "Sets union value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *valueObject* (PvObject) - union value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidArgument* - when object's field name/type do not match any of the union's fields\n\n"
        ":Raises: *InvalidRequest* - when specified field is not a union\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},), 'aString' : STRING})\n\n"
        "    value = PvObject({'anInt' : INT})\n\n"
        "    value.setInt(10)\n\n"
        "    pv.setUnion('anUnion', value)\n\n")

    .def("setUnion", static_cast<void(PvObject::*)(const boost::python::dict&)>(&PvObject::setUnion),
        args("valueDict"),
        "Sets union value for a single-field structure, or for a structure that has union field named 'value'.\n\n"
        ":Parameter: *valueDict* (dict) - union value\n\n"
        ":Raises: *InvalidArgument* - when dictionary's field name/type do not match any of the union's fields\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union field or multiple-field structure has no union 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},)})\n\n"
        "    pv.setUnion({'anInt' : 10})\n\n")

    .def("setUnion", static_cast<void(PvObject::*)(const std::string&,const boost::python::dict&)>(&PvObject::setUnion),
        args("fieldName", "valueDict"),
        "Sets union for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *valueDict* (dict) - union value\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidArgument* - when dictionary's field name/type do not match any of the union's fields\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union field or multiple-field structure has no union 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},), 'aString' : STRING})\n\n"
        "    pv.setUnion('anUnion', {'anInt' : 10})\n\n")

    .def("setUnion", static_cast<void(PvObject::*)(const boost::python::tuple&)>(&PvObject::setUnion),
        args("valueDict"),
        "Sets union value for a single-field structure, or for a structure that has union field named 'value'.\n\n"
        ":Parameter: *valueTuple* (tuple) - union value, must contain dictionary as its only element\n\n"
        ":Raises: *InvalidArgument* - when dictionary's field name/type do not match any of the union's fields\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union field or multiple-field structure has no union 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},)})\n\n"
        "    pv.setUnion(({'anInt' : 10},))\n\n")

    .def("setUnion", static_cast<void(PvObject::*)(const std::string&,const boost::python::tuple&)>(&PvObject::setUnion),
        args("fieldName", "valueTuple"),
        "Sets union for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *valueTuple* (tuple) - union value, must contain dictionary as its only element\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidArgument* - when dictionary's field name/type do not match any of the union's fields\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union field or multiple-field structure has no union 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},), 'aString' : STRING})\n\n"
        "    pv.setUnion('anUnion', ({'anInt' : 10},)\n\n")

    .def("getUnion",
        static_cast<PvObject(PvObject::*)()const>(&PvObject::getUnion),
        "Retrieves union value from a single-field structure, or from a structure that has union field named 'value'.\n\n"
        ":Returns: union PV object\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union field or multiple-field structure has no union 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},)})\n\n"
        "    value = pv.getUnion()\n\n")

    .def("getUnion",
        static_cast<PvObject(PvObject::*)(const std::string&)const>(&PvObject::getUnion),
        args("fieldName"),
        "Retrieves union assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: union PV object\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an union\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},), 'aString' : STRING})\n\n"
        "    value = pv.getUnion('anUnion')\n\n")

    .def("getUnionFieldNames",
        static_cast<boost::python::list(PvObject::*)(const std::string&)const>(&PvObject::getUnionFieldNames),
        args("fieldName"),
        "Retrieves list of field names for a union.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: list of union field names\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an union\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},), 'aString' : STRING})\n\n"
        "    fieldNameList = pv.getUnionFieldNames('anUnion')\n\n")

    .def("getUnionFieldNames",
        static_cast<boost::python::list(PvObject::*)()const>(&PvObject::getUnionFieldNames),
        "Retrieves list of union field names from a single-field structure, or from a structure that has union field named 'value'.\n\n"
        ":Returns: list of union field names\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union field or multiple-field structure has no union 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},)})\n\n"
        "    fieldNameList = pv.getUnionFieldNames()\n\n")

    .def("getSelectedUnionFieldName",
        static_cast<std::string(PvObject::*)(const std::string&)const>(&PvObject::getSelectedUnionFieldName),
        args("fieldName"),
        "Retrieves selected field name for an union.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: selected union field name\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an union\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},), 'aString' : STRING})\n\n"
        "    fieldName = pv.getSelectedUnionFieldName('anUnion')\n\n")

    .def("getSelectedUnionFieldName",
        static_cast<std::string(PvObject::*)()const>(&PvObject::getSelectedUnionFieldName),
        "Retrieves selected field name for an union from a single-field structure, or from a structure that has union field named 'value'.\n\n"
        ":Returns: selected union field name\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union field or multiple-field structure has no union 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},)})\n\n"
        "    fieldName = pv.getSelectedUnionFieldNames()\n\n")

    .def("selectUnionField",
        static_cast<PvObject(PvObject::*)(const std::string&, const std::string&)const>(&PvObject::selectUnionField),
        args("fieldName", "unionFieldName"),
        "Selects field for an union assigned to a given field name.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *unionFieldName* (str) - union field name to be selected\n\n"
        ":Returns: PV object for the selected union field\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an union\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},), 'aString' : STRING})\n\n"
        "    selectedPv = pv.selectUnionField('anUnion', 'anInt')\n\n")

    .def("selectUnionField",
        static_cast<PvObject(PvObject::*)(const std::string&)const>(&PvObject::selectUnionField),
        args("unionFieldName"),
        "Selects field for an union from a single-field structure, or from a structure that has union field named 'value'.\n\n"
        ":Parameter: *unionFieldName* (str) - union field name to be selected\n\n"
        ":Returns: PV object for the selected union field\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union field or multiple-field structure has no union 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},)})\n\n"
        "    selectedPv = pv.selectUnionField('anInt')\n\n")

    .def("isUnionVariant",
        static_cast<bool(PvObject::*)(const std::string&)const>(&PvObject::isUnionVariant),
        args("fieldName"),
        "Checks if an union assigned to a given field name is variant.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: true if union is variant, false otherwise\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an union\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},), 'aString' : STRING})\n\n"
        "    isVariant = pv.isUnionVariant('anUnion')\n\n")

    .def("isUnionVariant",
        static_cast<bool(PvObject::*)()const>(&PvObject::isUnionVariant),
        "Checks if an union from a single-field structure, or from a structure that has union field named 'value', is variant.\n\n"
        ":Returns: true if union is variant, false otherwise\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union field or multiple-field structure has no union 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},)})\n\n"
        "    isVariant = pv.isUnionVariant()\n\n")

    .def("createUnionField",
        static_cast<PvObject(PvObject::*)(const std::string&, const std::string&)const>(&PvObject::createUnionField),
        args("fieldName", "unionFieldName"),
        "Creates union field object for an union assigned to a given field name.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *unionFieldName* (str) - union field name to be created\n\n"
        ":Returns: PV object for new union field\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an union\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},), 'aString' : STRING})\n\n"
        "    createdPv = pv.createUnionField('anUnion', 'anInt')\n\n")

    .def("createUnionField",
        static_cast<PvObject(PvObject::*)(const std::string&)const>(&PvObject::createUnionField),
        args("unionFieldName"),
        "Creates union field object for an union from a single-field structure, or from a structure that has union field named 'value'.\n\n"
        ":Parameter: *unionFieldName* (str) - union field name to be created\n\n"
        ":Returns: PV object for new union field\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union field or multiple-field structure has no union 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},)})\n\n"
        "    createdPv = pv.createUnionField('anInt')\n\n")

    .def("setUnionArray", 
        static_cast<void(PvObject::*)(const boost::python::list&)>(&PvObject::setUnionArray),
        args("objectList"),
        "Sets union array value for a single-field structure, or for a structure that has union array field named 'value'.\n\n"
        ":Parameter: *objectList* (list) - list of PV objects, dictionaries, or tuples representing unions\n\n"
        ":Raises: *InvalidArgument* - when dictionary's field name/type do not match any of the union's fields\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union array field or multiple-field structure has no union array 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnionArray' : [({'anInt' : INT, 'aFloat' : FLOAT},)]})\n\n"
        "    pv.setUnionArray([{'anInt' : 10}, {'aFloat' : 11.1}])\n\n")

    .def("setUnionArray", 
        static_cast<void(PvObject::*)(const std::string&,const boost::python::list&)>(&PvObject::setUnionArray),
        args("fieldName", "objectList"),
        "Sets union array value for the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *objectList* (list) - list of PV objects, dictionaries, or tuples representing unions\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidArgument* - when dictionary's field name/type do not match any of the union's fields\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an union array\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnionArray' : [({'anInt' : INT, 'aFloat' : FLOAT},)], 'aString' : STRING})\n\n"
        "    pv.setUnionArray('anUnionArray', [{'anInt' : 10}, {'aFloat' : 11.1}])\n\n")

    .def("getUnionArray", 
        static_cast<boost::python::list(PvObject::*)()const>(&PvObject::getUnionArray), 
        "Retrieves union array value from a single-field structure, or from a structure that has union array field named 'value'.\n\n"
        ":Returns: list of union PV objects\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union array field or multiple-field structure has no union array 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnionArray' : [({'anInt' : INT, 'aFloat' : FLOAT},)]})\n\n"
        "    unionPvList = pv.getUnionArray()\n\n")

    .def("getUnionArray", 
        static_cast<boost::python::list(PvObject::*)(const std::string&)const>(&PvObject::getUnionArray), 
        args("fieldName"), 
        "Retrieves union array value assigned to the given PV field.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: list of union PV objects\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an union array\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnionArray' : [({'anInt' : INT, 'aFloat' : FLOAT},)], 'aString' : STRING})\n\n"
        "    unionPvList = pv.getUnionArray('anUnionArray')\n\n")

    .def("getUnionArrayFieldNames",
        static_cast<boost::python::list(PvObject::*)(const std::string&)const>(&PvObject::getUnionArrayFieldNames),
        args("fieldName"),
        "Retrieves list of field names for an union array.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: list of union array field names\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an union array\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnionArray' : [({'anInt' : INT, 'aFloat' : FLOAT},)], 'aString' : STRING})\n\n"
        "    fieldNameList = pv.getUnionArrayFieldNames('anUnionArray')\n\n")

    .def("getUnionArrayFieldNames",
        static_cast<boost::python::list(PvObject::*)()const>(&PvObject::getUnionArrayFieldNames),
        "Retrieves list of union array field names from a single-field structure, or from a structure that has union array field named 'value'.\n\n"
        ":Returns: list of union array field names\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union array field or multiple-field structure has no union array 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnionArray' : [({'anInt' : INT, 'aFloat' : FLOAT},)]})\n\n"
        "    fieldNameList = pv.getUnionArrayFieldNames()\n\n")

    .def("isUnionArrayVariant",
        static_cast<bool(PvObject::*)(const std::string&)const>(&PvObject::isUnionArrayVariant),
        args("fieldName"),
        "Checks if an union array assigned to a given field name is variant.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Returns: true if union array is variant, false otherwise\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an union array\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnionArray' : [({'anInt' : INT, 'aFloat' : FLOAT},)], 'aString' : STRING})\n\n"
        "    isVariant = pv.isUnionArrayVariant('anUnionArray')\n\n")

    .def("isUnionArrayVariant",
        static_cast<bool(PvObject::*)()const>(&PvObject::isUnionArrayVariant),
        "Checks if an union array from a single-field structure, or from a structure that has union array field named 'value', is variant.\n\n"
        ":Returns: true if union array is variant, false otherwise\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union array field or multiple-field structure has no union array 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnionArray' : [({'anInt' : INT, 'aFloat' : FLOAT},)]})\n\n"
        "    isVariant = pv.isUnionArrayVariant()\n\n")

    .def("createUnionArrayElementField",
        static_cast<PvObject(PvObject::*)(const std::string&, const std::string&)const>(&PvObject::createUnionArrayElementField),
        args("fieldName", "unionFieldName"),
        "Creates union field object for an union array assigned to a given field name.\n\n"
        ":Parameter: *fieldName* (str) - field name\n\n"
        ":Parameter: *unionFieldName* (str) - union field name to be created\n\n"
        ":Returns: PV object for new union field\n\n"
        ":Raises: *FieldNotFound* - when PV structure does not have specified field\n\n"
        ":Raises: *InvalidRequest* - when specified field is not an union array\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnionArray' : [({'anInt' : INT, 'aFloat' : FLOAT},)], 'aString' : STRING})\n\n"
        "    unionPv = pv.createUnionArrayElementField('anUnionArray', 'anInt')\n\n")

    .def("createUnionArrayElementField",
        static_cast<PvObject(PvObject::*)(const std::string&)const>(&PvObject::createUnionArrayElementField),
        args("unionFieldName"),
        "Creates union field object for an union array from a single-field structure, or from a structure that has union array field named 'value'.\n\n"
        ":Parameter: *unionFieldName* (str) - union field name to be created\n\n"
        ":Returns: PV object for new union field\n\n"
        ":Raises: *InvalidRequest* - when single-field structure has no union array field or multiple-field structure has no union array 'value' field\n\n"
        "::\n\n"
        "    pv = PvObject({'anUnionArray' : [({'anInt' : INT, 'aFloat' : FLOAT},)]})\n\n"
        "    unionPv = pv.createUnionArrayElementField('anInt')\n\n")

    .def("toDict", 
        &PvObject::toDict,
        "Converts PV structure to python dictionary.\n\n:Returns: python key:value dictionary representing current PV structure in terms of field names and their values\n\n::\n\n    valueDict = pv.toDict()\n\n")

    .def("getStructureDict", 
        &PvObject::getStructureDict,
        "Retrieves PV structure definition as python dictionary.\n\n:Returns: python key:value dictionary representing PV structure definition in terms of field names and their types\n\n::\n\n    structureDict = pv.getStructureDict()\n\n")

#if defined HAVE_BOOST_NUM_PY && HAVE_BOOST_NUM_PY == 1
    .add_property("useNumPyArrays", &PvObject::getUseNumPyArraysFlag, &PvObject::setUseNumPyArraysFlag)
#endif // if defined HAVE_BOOST_NUM_PY && HAVE_BOOST_NUM_PY == 1

;

} // wrapPvObject()

