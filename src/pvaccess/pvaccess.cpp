#include "boost/python/class.hpp"
#include "boost/python/module.hpp"
#include "boost/python/def.hpp"
#include "boost/python/self.hpp"
#include "boost/python/other.hpp"
#include "boost/python/enum.hpp"
#include "boost/python/dict.hpp"
#include "boost/python/operators.hpp"
#include "boost/python/manage_new_object.hpp"
#include "boost/python/overloads.hpp"
#include "boost/python/exception_translator.hpp"
#include "boost/python/object.hpp"
#include "boost/python/docstring_options.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/operators.hpp"

#include "PvObject.h"
#include "PvProvider.h"
#include "PvScalar.h"
#include "PvType.h"
#include "PvBoolean.h"
#include "PvByte.h"
#include "PvUByte.h"
#include "PvShort.h"
#include "PvUShort.h"
#include "PvInt.h"
#include "PvUInt.h"
#include "PvLong.h"
#include "PvULong.h"
#include "PvFloat.h"
#include "PvDouble.h"
#include "PvString.h"
#include "PvScalarArray.h"

#include "PvTimeStamp.h"

#include "NtType.h"
#include "NtTable.h"

#include "Channel.h"
#include "RpcClient.h"
#include "RpcServer.h"
#include "RpcServiceImpl.h"
#include "PvaException.h"
#include "PvaExceptionTranslator.h"

// Overload macros
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetBoolean, PvObject::getBoolean, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetByte, PvObject::getByte, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetUByte, PvObject::getUByte, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetShort, PvObject::getShort, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetUShort, PvObject::getUShort, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetInt, PvObject::getInt, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetUInt, PvObject::getUInt, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetLong, PvObject::getLong, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetULong, PvObject::getULong, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetFloat, PvObject::getFloat, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetDouble, PvObject::getDouble, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetString, PvObject::getString, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetScalarArray, PvObject::getScalarArray, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetStructure, PvObject::getStructure, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetStructureArray, PvObject::getStructureArray, 0, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ChannelGet, Channel::get, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ChannelPut, Channel::put, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ChannelStartMonitor, Channel::startMonitor, 0, 1)
//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RpcClientRequest, RpcClient::request, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RpcServerListen, RpcServer::listen, 0, 1)

BOOST_PYTHON_MODULE(pvaccess)
{
    using namespace boost::python;
    scope().attr("__doc__") = "pvaccess module is a python wrapper for pvAccess and other `EPICS V4 <http://epics-pvdata.sourceforge.net>`_ C++ libraries."; 
    docstring_options local_docstring_options(true, true, false);

    //
    // Exceptions
    //
    register_exception_translator<PvaException>(PvaExceptionTranslator::translator)
        ;

    //
    // PvType
    //
    enum_<PvType::ScalarType>("PvType")
        .value("BOOLEAN", PvType::Boolean)
        .value("BYTE", PvType::Byte)
        .value("UBYTE", PvType::UByte)
        .value("SHORT", PvType::Short)
        .value("USHORT", PvType::UShort)
        .value("INT", PvType::Int)
        .value("UINT", PvType::UInt)
        .value("LONG", PvType::Long)
        .value("ULONG", PvType::ULong)
        .value("FLOAT", PvType::Float)
        .value("DOUBLE", PvType::Double)
        .value("STRING", PvType::String)
        .export_values()
        ;

    //
    // PvProvider
    //
    enum_<PvProvider::ProviderType>("ProviderType")
        .value("PVA", PvProvider::PvaProviderType)
        .value("CA", PvProvider::CaProviderType)
        .export_values()
        ;

    //
    // PvObject
    //
    class_<PvObject>("PvObject", 
            "PvObject represents a generic PV structure.\n\n:Parameter: structureDict (dict) - dictionary of key:value pairs describing the underlying PV structure in terms of field names and their types\n\nThe dictionary key is a string (PV field name), and value is one of:\n\n- PVTYPE: scalar type, can be BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING\n- [PVTYPE]: single element list representing scalar array\n- {key:value,…}: structure\n- [{key:value,…}]: single element list representing structure array\n\n**Examples:**\n::\n\n\tpv1 = PvObject({'anInt' : INT})\n\n\tpv2 = PvObject({'aShort' : SHORT, 'anUInt' : UINT, 'aString' : STRING})\n\n\tpv3 = PvObject({'aStringArray' : [STRING], 'aStruct' : {'aString2' : STRING, 'aBoolArray' : [BOOLEAN], 'aStruct2' : {'aFloat' : FLOAT, 'aString3' : [STRING]}}})\n\n\tpv4 = PvObject({'aStructArray' : [{'anInt' : INT, 'anInt2' : INT, 'aDouble' : DOUBLE}]})\n\n", 
            init<boost::python::dict>(arg("structureDict")))

        .def(str(self))

        .def("set", 
            static_cast<void(PvObject::*)(const boost::python::dict&)>(&PvObject::set),
            arg("valueDict"),
            "Populates PV structure fields from python dictionary.\n\n:Parameter: *valueDict* (dict) - dictionary of key:value pairs that correspond to PV structure field names and their values\n\n**Example:**\n::\n\n    pv.set({'anUInt' : 1, 'aString' : 'my string example'})\n\n")

        .def("get", 
            static_cast<boost::python::dict(PvObject::*)()const>(&PvObject::get), 
            "Retrieves PV structure as python dictionary.\n\n:Returns: python key:value dictionary representing current PV structure in terms of field names and their values\n\n**Example:**\n::\n\n    valueDict = pv.get()\n\n")

        .def("setBoolean", 
            static_cast<void(PvObject::*)(bool)>(&PvObject::setBoolean),
            arg("value"),
            "Sets boolean value for the PV field named 'value'.\n\n:Parameter: *value* (bool) - boolean value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setBoolean(True)\n\n")

        .def("setBoolean", 
            static_cast<void(PvObject::*)(const std::string&,bool)>(&PvObject::setBoolean),
            args("name", "value"),
            "Sets boolean value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (bool) - boolean value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setBoolean('aBoolean', True)\n\n")

        .def("getBoolean", 
            static_cast<bool(PvObject::*)(const std::string&)const>(&PvObject::getBoolean),
            PvObjectGetBoolean(args("name"), "Retrieves boolean value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name (default: 'value')\n\n:Returns: boolean stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getBoolean('aBoolean')\n\n"))
 
        .def("setByte", 
            static_cast<void(PvObject::*)(char)>(&PvObject::setByte),
            args("value"),
            "Sets byte (character) value for the PV field named 'value'.\n\n:Parameter: *value* (str) - byte value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setByte('a')\n\n")

        .def("setByte", static_cast<void(PvObject::*)(const std::string&,char)>(&PvObject::setByte),
            args("name", "value"),
            "Sets byte (character) value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (str) - byte value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setByte('aByte', 'a')\n\n")

        .def("getByte", 
            static_cast<char(PvObject::*)(const std::string&)const>(&PvObject::getByte), 
            PvObjectGetByte(args("name"), "Retrieves byte (character) value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name (default: 'value')\n\n:Returns: byte stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getByte('aByte')\n\n"))

        .def("setUByte", 
            static_cast<void(PvObject::*)(unsigned char)>(&PvObject::setUByte),
            args("value"),
            "Sets unsigned byte (character) value for the PV field named 'value'.\n\n:Parameter: *value* (str) - unsigned byte that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setUByte('a')\n\n")

        .def("setUByte", 
            static_cast<void(PvObject::*)(const std::string&,unsigned char)>(&PvObject::setUByte),
            args("name", "value"),
            "Sets unsigned byte (character) value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (str) - unsigned byte value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setByte('anUByte', 'a')\n\n")

        .def("getUByte", static_cast<unsigned char(PvObject::*)(const std::string&)const>(&PvObject::getUByte), 
            PvObjectGetUByte(args("name"), "Retrieves unsigned byte (character) value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name (default: 'value')\n\n:Returns: unsigned byte stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getUByte('anUByte')\n\n"))

        .def("setShort", 
            static_cast<void(PvObject::*)(short)>(&PvObject::setShort),
            args("value"),
            "Sets short value for the PV field named 'value'.\n\n:Parameter: *value* (int) - short value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setShort(10)\n\n")

        .def("setShort", 
            static_cast<void(PvObject::*)(const std::string&,short)>(&PvObject::setShort),
            args("name", "value"),
            "Sets short value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (int) - short value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setShort('aShort', 10)\n\n")

        .def("getShort", static_cast<short(PvObject::*)(const std::string&)const>(&PvObject::getShort), 
            PvObjectGetShort(args("name"), "Retrieves short value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name (default: 'value')\n\n:Returns: short stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getShort('aShort')\n\n"))

        .def("setUShort", 
            static_cast<void(PvObject::*)(unsigned short)>(&PvObject::setUShort),
            args("value"),
            "Sets unsigned short value for the PV field named 'value'.\n\n:Parameter: *value* (int) - unsigned short value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setUShort(10)\n\n")

        .def("setUShort", 
            static_cast<void(PvObject::*)(const std::string&,unsigned short)>(&PvObject::setUShort),
            args("name", "value"),
            "Sets unsigned short value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (int) - unsigned short value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setUShort('anUShort', 10)\n\n")

        .def("getUShort", static_cast<unsigned short(PvObject::*)(const std::string&)const>(&PvObject::getUShort),
            PvObjectGetUShort(args("name"), "Retrieves unsigned short value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name (default: 'value')\n\n:Returns: unsigned short stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getUShort('anUShort')\n\n"))

        .def("setInt", 
            static_cast<void(PvObject::*)(int)>(&PvObject::setInt),
            args("value"),
            "Sets integer value for the PV field named 'value'.\n\n:Parameter: *value* (int) - integer value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setInt(10)\n\n")

        .def("setInt", 
            static_cast<void(PvObject::*)(const std::string&,int)>(&PvObject::setInt),
            args("name", "value"),
            "Sets integer value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (int) - integer value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setInt('anInt', 10)\n\n")

        .def("getInt", 
            static_cast<int(PvObject::*)(const std::string&)const>(&PvObject::getInt), 
            PvObjectGetInt(args("name"), "Retrieves integer value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name (default: 'value')\n\n:Returns: integer stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getInt('anInt')\n\n"))

        .def("setUInt", 
            static_cast<void(PvObject::*)(unsigned int)>(&PvObject::setUInt),
            args("value"),
            "Sets unsigned integer value for the PV field named 'value'.\n\n:Parameter: *value* (int) - unsigned integer value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setUInt(10)\n\n")

        .def("setUInt", 
            static_cast<void(PvObject::*)(const std::string&,unsigned int)>(&PvObject::setUInt),
            args("name", "value"),
            "Sets unsigned integer value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (int) - unsigned integer value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setUInt('anInt', 10)\n\n")

        .def("getUInt", 
            static_cast<unsigned int(PvObject::*)(const std::string&)const>(&PvObject::getUInt),
            PvObjectGetUInt(args("name"), "Retrieves unsigned integer value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name (default: 'value')\n\n:Returns: unsigned integer stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getUInt('anUInt')\n\n"))

        .def("setLong", 
            static_cast<void(PvObject::*)(long long)>(&PvObject::setLong),
            args("value"),
            "Sets long value for the PV field named 'value'.\n\n:Parameter: *value* (long) - long value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setLong(10L)\n\n")

        .def("setLong", 
            static_cast<void(PvObject::*)(const std::string&,long long)>(&PvObject::setLong),
            args("name", "value"),
            "Sets long value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (long) - long value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setLong('aLong', 10L)\n\n")

        .def("getLong", 
            static_cast<long long(PvObject::*)(const std::string&)const>(&PvObject::getLong),
            PvObjectGetLong(args("name"), "Retrieves long value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name (default: 'value')\n\n:Returns: long stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getLong('aLong')\n\n"))

        .def("setULong", 
            static_cast<void(PvObject::*)(unsigned long long)>(&PvObject::setULong),
            args("value"),
            "Sets unsigned long value for the PV field named 'value'.\n\n:Parameter: *value* (long) - unsigned long value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setULong(10L)\n\n")

        .def("setULong", 
            static_cast<void(PvObject::*)(const std::string&,unsigned long long)>(&PvObject::setULong),
            args("name", "value"),
            "Sets unsigned long value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (long) - unsigned long value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setULong('aLong', 10L)\n\n")

        .def("getULong", 
            static_cast<unsigned long long(PvObject::*)(const std::string&)const>(&PvObject::getULong), 
            PvObjectGetULong(args("name"), "Retrieves unsigned long value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name (default: 'value')\n\n:Returns: unsigned long stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getULong('anULong')\n\n"))

        .def("setFloat", 
            static_cast<void(PvObject::*)(float)>(&PvObject::setFloat),
            args("value"),
            "Sets float value for the PV field named 'value'.\n\n:Parameter: *value* (float) - float value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setFloat(10.0)\n\n")

        .def("setFloat", 
            static_cast<void(PvObject::*)(const std::string&,float)>(&PvObject::setFloat),
            args("name", "value"),
            "Sets float value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (float) - float value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setFloat('aFloat', 10.0)\n\n")

        .def("getFloat", 
            static_cast<float(PvObject::*)(const std::string&)const>(&PvObject::getFloat), 
            PvObjectGetFloat(args("name"), "Retrieves float value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name (default: 'value')\n\n:Returns: float stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getFloat('aFloat')\n\n"))

        .def("setDouble", static_cast<void(PvObject::*)(double)>(&PvObject::setDouble),
            args("value"),
            "Sets double value for the PV field named 'value'.\n\n:Parameter: *value* (float) - double value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setDouble(10.0)\n\n")

        .def("setDouble", static_cast<void(PvObject::*)(const std::string&,double)>(&PvObject::setDouble),
            args("name", "value"),
            "Sets double value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (float) - double value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setDouble('aDouble', 10.0)\n\n")

        .def("getDouble", 
            static_cast<double(PvObject::*)(const std::string&)const>(&PvObject::getDouble), 
            PvObjectGetDouble(args("name"), "Retrieves double value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name (default: 'value')\n\n:Returns: double stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getDouble('aDouble')\n\n"))

        .def("setString", 
            static_cast<void(PvObject::*)(const std::string&)>(&PvObject::setString),
            args("value"),
            "Sets string value for the PV field named 'value'.\n\n:Parameter: *value* (str) - string value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setString('string value')\n\n")

        .def("setString", 
            static_cast<void(PvObject::*)(const std::string&,const std::string&)>(&PvObject::setString),
            args("name", "value"),
            "Sets string value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (str) - string value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setString('aString', 'string value')\n\n")
        
        .def("getString", static_cast<std::string(PvObject::*)(const std::string&)const>(&PvObject::getString),
            PvObjectGetString(args("name"), "Retrieves string value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name (default: 'value')\n\n:Returns: string stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getString('aString')\n\n"))

        .def("setScalarArray", 
            static_cast<void(PvObject::*)(const boost::python::list&)>(&PvObject::setScalarArray),
            args("valueList"),
            "Sets scalar array for the PV field named 'value'.\n\n:Parameter: *valueList* (list) - list of scalar values that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setScalarArray([0,1,2,3,4,5])\n\n")

        .def("setScalarArray", 
            static_cast<void(PvObject::*)(const std::string&,const boost::python::list&)>(&PvObject::setScalarArray),
            args("name", "valueList"),
            "Sets scalar array for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *valueList* (list) - list of scalar values that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setScalarArray('aScalarArray', [0,1,2,3,4,5])\n\n")

        .def("getScalarArray", 
            static_cast<boost::python::list(PvObject::*)(const std::string&)const>(&PvObject::getScalarArray), 
            PvObjectGetScalarArray(args("name"), "Retrieves scalar array assigned to the given PV field.\n\n:Parameter: *name* (str) - field name (default: 'value')\n\n:Returns: list of scalar values stored in the given PV field\n\n**Example:**\n::\n\n    valueList = pv.getScalarArray('aScalarArray')\n\n"))

        .def("setStructure", 
            static_cast<void(PvObject::*)(const boost::python::dict&)>(&PvObject::setStructure),
            args("valueDict"),
            "Sets structure for the PV field named 'value'.\n\n:Parameter: *valueDict* (dict) - dictionary of key:value pairs for PV structure that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setStructure({'aString' : 'string value', 'anInt' : 1, 'aFloat' : 1.1})\n\n")

        .def("setStructure", 
            static_cast<void(PvObject::*)(const std::string&,const boost::python::dict&)>(&PvObject::setStructure),
            args("name", "valueDict"),
            "Sets structure for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *valueDict* (dict) - dictionary of key:value pairs for PV structure that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setStructure('aStruct', {'aString' : 'string value', 'anInt' : 1, 'aFloat' : 1.1})\n\n")

        .def("getStructure", 
            static_cast<boost::python::dict(PvObject::*)(const std::string&)const>(&PvObject::getStructure), 
            PvObjectGetStructure(args("name"), "Retrieves structure assigned to the given PV field.\n\n:Parameter: *name* (str) - field name (default: 'value')\n\n:Returns: dictionary of key:value pairs for PV structure stored in the given PV field\n\n**Example:**\n::\n\n    valueDict = pv.getStructure('aStruct')\n\n"))

        .def("setStructureArray", 
            static_cast<void(PvObject::*)(const boost::python::list&)>(&PvObject::setStructureArray),
            args("dictList"),
            "Sets structure array for the PV field named 'value'.\n\n:Parameter: *dictList* (list) - list of dictionaries (describing PV structures) that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setStructureArray([{'anInt' : 1, 'aFloat' : 1.1},{'anInt' : 2, 'aFloat' : 2.2},{'anInt' : 3, 'aFloat' : 3.3}])\n\n")

        .def("setStructureArray", 
            static_cast<void(PvObject::*)(const std::string&,const boost::python::list&)>(&PvObject::setStructureArray),
            args("name", "dictList"),
            "Sets structure array for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *dictList* (list) - list of dictionaries (describing PV structures) that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setStructureArray('aStructArray', [{'anInt' : 1, 'aFloat' : 1.1},{'anInt' : 2, 'aFloat' : 2.2},{'anInt' : 3, 'aFloat' : 3.3}])\n\n")

        .def("getStructureArray", 
            static_cast<boost::python::list(PvObject::*)(const std::string&)const>(&PvObject::getStructureArray), 
            PvObjectGetStructureArray(args("name"), "Retrieves structure array assigned to the given PV field.\n\n:Parameter: *name* (str) - field name (default: 'value')\n\n:Returns: list of dictionaries (describing PV structures) stored in the given PV field\n\n**Example:**\n::\n\n    dictList = pv.getStructureArray('aStructArray')\n\n"))

        .def("toDict", 
            &PvObject::toDict,
            "Converts PV structure to python dictionary.\n\n:Returns: python key:value dictionary representing current PV structure in terms of field names and their values\n\n**Example:**\n::\n\n    valueDict = pv.toDict()\n\n")

        .def("getStructureDict", 
            &PvObject::getStructureDict,
            "Retrieves PV structure definition as python dictionary.\n\n:Returns: python key:value dictionary representing PV structure definition in terms of field names and their types\n\n**Example:**\n::\n\n    structureDict = pv.getStructureDict()\n\n")
        ;

    //
    // PV Scalar
    //
    class_<PvScalar, bases<PvObject> >("PvScalar", no_init)
        // use __int__ instead of .def(int_(self)) which requires operator::long()
        // use __float__ instead of .def(float_(self)) 
        .def("__int__", &PvScalar::toInt)   
        .def("__long__", &PvScalar::toLongLong)
        .def("__float__", &PvScalar::toDouble)   
        .def("__str__", &PvScalar::toString)   
        ;

    //
    // PV Boolean
    //
    class_<PvBoolean, bases<PvScalar> >("PvBoolean", init<>())
        .def(init<bool>())
        .def("get", &PvBoolean::get)
        .def("set", &PvBoolean::set)
        ;

    //
    // PV Byte
    //
    class_<PvByte, bases<PvScalar> >("PvByte", init<>())
        .def(init<char>())
        .def("get", &PvByte::get)
        .def("set", &PvByte::set)
        ;

    //
    // PV UByte
    //
    class_<PvUByte, bases<PvScalar> >("PvUByte", init<>())
        .def(init<unsigned char>())
        .def("get", &PvUByte::get)
        .def("set", &PvUByte::set)
        ;

    //
    // PV Short
    //
    class_<PvShort, bases<PvScalar> >("PvShort", init<>())
        .def(init<short>())
        .def("get", &PvShort::get)
        .def("set", &PvShort::set)
        ;

    //
    // PV UShort
    //
    class_<PvUShort, bases<PvScalar> >("PvUShort", init<>())
        .def(init<unsigned short>())
        .def("get", &PvUShort::get)
        .def("set", &PvUShort::set)
        ;

    //
    // PV Int
    //
    class_<PvInt, bases<PvScalar> >("PvInt", init<>())
        .def(init<int>())
        .def("get", &PvInt::get)
        .def("set", &PvInt::set)
        ;

    //
    // PV UInt
    //
    class_<PvUInt, bases<PvScalar> >("PvUInt", init<>())
        .def(init<unsigned int>())
        .def("get", &PvUInt::get)
        .def("set", &PvUInt::set)
        ;

    //
    // PV Long
    //
    class_<PvLong, bases<PvScalar> >("PvLong", init<>())
        .def(init<long long>())
        .def("get", &PvLong::get)
        .def("set", &PvLong::set)
        ;

    //
    // PV ULong
    //
    class_<PvULong, bases<PvScalar> >("PvULong", init<>())
        .def(init<unsigned long long>())
        .def("get", &PvULong::get)
        .def("set", &PvULong::set)
        ;

    //
    // PV Float
    //
    class_<PvFloat, bases<PvScalar> >("PvFloat", init<>())
        .def(init<float>())
        .def("get", &PvFloat::get)
        .def("set", &PvFloat::set)
        ;

    //
    // PV Double
    //
    class_<PvDouble, bases<PvScalar> >("PvDouble", init<>())
        .def(init<double>())
        .def("get", &PvDouble::get)
        .def("set", &PvDouble::set)
        ;

    //
    // PV String
    //
    class_<PvString, bases<PvScalar> >("PvString", init<>())
        .def(init<std::string>())
        .def("get", &PvString::get)
        .def("set", &PvString::set)
        ;

    //
    // PV Scalar Array
    //
    class_<PvScalarArray, bases<PvObject> >("PvScalarArray", init<PvType::ScalarType>())
        .def("get", &PvScalarArray::get)
        .def("set", &PvScalarArray::set)
        .def("toList", &PvScalarArray::toList)
        ;

    //
    // PV TimeStamp
    //
    class_<PvTimeStamp, bases<PvObject> >("PvTimeStamp", init<>())
        .def(init<long long, int>())
        .def(init<long long, int, int>())
        .def("getSecondsPastEpoch", &PvTimeStamp::getSecondsPastEpoch)
        .def("setSecondsPastEpoch", &PvTimeStamp::setSecondsPastEpoch)
        .def("getNanoSeconds", &PvTimeStamp::getNanoSeconds)
        .def("setNanoSeconds", &PvTimeStamp::setNanoSeconds)
        .def("getUserTag", &PvTimeStamp::getUserTag)
        .def("setUserTag", &PvTimeStamp::setUserTag)
        ;

    //
    // PV Alarm
    //
    class_<PvAlarm, bases<PvObject> >("PvAlarm", init<>())
        .def(init<int, int, const std::string&>())
        .def("getSeverity", &PvAlarm::getSeverity)
        .def("setSeverity", &PvAlarm::setSeverity)
        .def("getStatus", &PvAlarm::getStatus)
        .def("setStatus", &PvAlarm::setStatus)
        .def("getMessage", &PvAlarm::getMessage)
        .def("setMessage", &PvAlarm::setMessage)
        ;

    //
    // NT Type
    //
    class_<NtType, bases<PvObject> >("NtType", no_init)
        ;

    //
    // NT Table 
    //
    class_<NtTable, bases<NtType> >("NtTable", init<int, PvType::ScalarType>())
        .def(init<const boost::python::list&>())
        .def(init<const PvObject&>())
        .def("getNColumns", &NtTable::getNColumns)
        .def("getLabels", &NtTable::getLabels)
        .def("setLabels", &NtTable::setLabels)
        .def("getColumn", &NtTable::getColumn)
        .def("setColumn", &NtTable::setColumn)
        .def("getDescriptor", &NtTable::getDescriptor)
        .def("setDescriptor", &NtTable::setDescriptor)
        .def("getTimeStamp", &NtTable::getTimeStamp)
        .def("setTimeStamp", &NtTable::setTimeStamp)
        .def("getAlarm", &NtTable::getAlarm)
        .def("setAlarm", &NtTable::setAlarm)
        ;

    // Channel
    class_<Channel>("Channel", init<std::string>())
        .def(init<std::string, PvProvider::ProviderType>())
        .def("get", static_cast<PvObject*(Channel::*)(const std::string&)>(&Channel::get), 
            return_value_policy<manage_new_object>(), ChannelGet())
        .def("get", static_cast<PvObject*(Channel::*)()>(&Channel::get), 
            return_value_policy<manage_new_object>(), ChannelGet())
        .def("put", static_cast<void(Channel::*)(const PvObject&, const std::string&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(const PvObject&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(const std::string&, const std::string&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(const std::string&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(const boost::python::list&, const std::string&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(const boost::python::list&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(bool, const std::string&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(bool)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(char, const std::string&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(char)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(unsigned char, const std::string&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(unsigned char)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(short, const std::string&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(short)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(unsigned short, const std::string&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(unsigned short)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(int, const std::string&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(int)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(unsigned int, const std::string&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(unsigned int)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(long long, const std::string&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(long long)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(unsigned long long, const std::string&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(unsigned long long)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(float, const std::string&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(float)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(double, const std::string&)>(&Channel::put))
        .def("put", static_cast<void(Channel::*)(double)>(&Channel::put))
        .def("subscribe", &Channel::subscribe)
        .def("unsubscribe", &Channel::unsubscribe)
        .def("startMonitor", static_cast<void(Channel::*)(const std::string&)>(&Channel::startMonitor))
        .def("startMonitor", static_cast<void(Channel::*)()>(&Channel::startMonitor))
        .def("stopMonitor", &Channel::stopMonitor)
        .def("getTimeout", &Channel::getTimeout)
        .def("setTimeout", &Channel::setTimeout)
        ;

    // RPC Client
    class_<RpcClient>("RpcClient", init<std::string>())
        .def("invoke", &RpcClient::invoke,
            return_value_policy<manage_new_object>())
        ;
    
    // RPC Service Impl
    class_<RpcServiceImpl, boost::shared_ptr<RpcServiceImpl> >("RpcServiceImpl", init<boost::python::object>())
        ;

    // RPC Server
    class_<RpcServer>("RpcServer", init<>())
        .def("registerService", &RpcServer::registerService)
        .def("unregisterService", &RpcServer::unregisterService)
        .def("startListener", &RpcServer::startListener)
        .def("stopListener", &RpcServer::stopListener)
        .def("start", &RpcServer::start)
        .def("stop", &RpcServer::start)
        .def("listen", static_cast<void(RpcServer::*)(int)>(&RpcServer::listen), RpcServerListen())
        .def("shutdown", &RpcServer::shutdown)
        ;
    
}
