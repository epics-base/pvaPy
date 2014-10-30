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
            "PvObject represents a generic PV structure.\n\n**PvObject(structureDict)**\n\n\t:Parameter: structureDict (dict) - dictionary of key:value pairs describing the underlying PV structure in terms of field names and their types\n\n\tThe dictionary key is a string (PV field name), and value is one of:\n\n\t- PVTYPE: scalar type, can be BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING\n\t- [PVTYPE]: single element list representing scalar array\n\t- {key:value,…}: structure\n\t- [{key:value,…}]: single element list representing structure array\n\n\t::\n\n\t\tpv1 = PvObject({'anInt' : INT})\n\n\t\tpv2 = PvObject({'aShort' : SHORT, 'anUInt' : UINT, 'aString' : STRING})\n\n\t\tpv3 = PvObject({'aStringArray' : [STRING], 'aStruct' : {'aString2' : STRING, 'aBoolArray' : [BOOLEAN], 'aStruct2' : {'aFloat' : FLOAT, 'aString3' : [STRING]}}})\n\n\t\tpv4 = PvObject({'aStructArray' : [{'anInt' : INT, 'anInt2' : INT, 'aDouble' : DOUBLE}]})\n\n", 
            init<boost::python::dict>(args("structureDict")))

        .def(str(self))

        .def("set", 
            static_cast<void(PvObject::*)(const boost::python::dict&)>(&PvObject::set),
            args("valueDict"),
            "Populates PV structure fields from python dictionary.\n\n:Parameter: *valueDict* (dict) - dictionary of key:value pairs that correspond to PV structure field names and their values\n\n**Example:**\n::\n\n    pv.set({'anUInt' : 1, 'aString' : 'my string example'})\n\n")

        .def("get", 
            static_cast<boost::python::dict(PvObject::*)()const>(&PvObject::get), 
            "Retrieves PV structure as python dictionary.\n\n:Returns: python key:value dictionary representing current PV structure in terms of field names and their values\n\n**Example:**\n::\n\n    valueDict = pv.get()\n\n")

        .def("setBoolean", 
            static_cast<void(PvObject::*)(bool)>(&PvObject::setBoolean),
            args("value"),
            "Sets boolean value for the PV field named 'value'.\n\n:Parameter: *value* (bool) - boolean value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setBoolean(True)\n\n")

        .def("setBoolean", 
            static_cast<void(PvObject::*)(const std::string&,bool)>(&PvObject::setBoolean),
            args("name", "value"),
            "Sets boolean value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (bool) - boolean value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setBoolean('aBoolean', True)\n\n")

        .def("getBoolean", 
            static_cast<bool(PvObject::*)(const std::string&)const>(&PvObject::getBoolean),
            PvObjectGetBoolean(args("name='value'"), "Retrieves boolean value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Returns: boolean stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getBoolean('aBoolean')\n\n"))
 
        .def("setByte", 
            static_cast<void(PvObject::*)(char)>(&PvObject::setByte),
            args("value"),
            "Sets byte (character) value for the PV field named 'value'.\n\n:Parameter: *value* (str) - byte value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setByte('a')\n\n")

        .def("setByte", static_cast<void(PvObject::*)(const std::string&,char)>(&PvObject::setByte),
            args("name", "value"),
            "Sets byte (character) value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (str) - byte value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setByte('aByte', 'a')\n\n")

        .def("getByte", 
            static_cast<char(PvObject::*)(const std::string&)const>(&PvObject::getByte), 
            PvObjectGetByte(args("name='value'"), "Retrieves byte (character) value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Returns: byte stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getByte('aByte')\n\n"))

        .def("setUByte", 
            static_cast<void(PvObject::*)(unsigned char)>(&PvObject::setUByte),
            args("value"),
            "Sets unsigned byte (character) value for the PV field named 'value'.\n\n:Parameter: *value* (str) - unsigned byte that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setUByte('a')\n\n")

        .def("setUByte", 
            static_cast<void(PvObject::*)(const std::string&,unsigned char)>(&PvObject::setUByte),
            args("name", "value"),
            "Sets unsigned byte (character) value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (str) - unsigned byte value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setByte('anUByte', 'a')\n\n")

        .def("getUByte", static_cast<unsigned char(PvObject::*)(const std::string&)const>(&PvObject::getUByte), 
            PvObjectGetUByte(args("name='value'"), "Retrieves unsigned byte (character) value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Returns: unsigned byte stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getUByte('anUByte')\n\n"))

        .def("setShort", 
            static_cast<void(PvObject::*)(short)>(&PvObject::setShort),
            args("value"),
            "Sets short value for the PV field named 'value'.\n\n:Parameter: *value* (int) - short value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setShort(10)\n\n")

        .def("setShort", 
            static_cast<void(PvObject::*)(const std::string&,short)>(&PvObject::setShort),
            args("name", "value"),
            "Sets short value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (int) - short value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setShort('aShort', 10)\n\n")

        .def("getShort", static_cast<short(PvObject::*)(const std::string&)const>(&PvObject::getShort), 
            PvObjectGetShort(args("name='value'"), "Retrieves short value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Returns: short stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getShort('aShort')\n\n"))

        .def("setUShort", 
            static_cast<void(PvObject::*)(unsigned short)>(&PvObject::setUShort),
            args("value"),
            "Sets unsigned short value for the PV field named 'value'.\n\n:Parameter: *value* (int) - unsigned short value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setUShort(10)\n\n")

        .def("setUShort", 
            static_cast<void(PvObject::*)(const std::string&,unsigned short)>(&PvObject::setUShort),
            args("name", "value"),
            "Sets unsigned short value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (int) - unsigned short value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setUShort('anUShort', 10)\n\n")

        .def("getUShort", static_cast<unsigned short(PvObject::*)(const std::string&)const>(&PvObject::getUShort),
            PvObjectGetUShort(args("name='value'"), "Retrieves unsigned short value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Returns: unsigned short stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getUShort('anUShort')\n\n"))

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
            PvObjectGetInt(args("name='value'"), "Retrieves integer value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Returns: integer stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getInt('anInt')\n\n"))

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
            PvObjectGetUInt(args("name='value'"), "Retrieves unsigned integer value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Returns: unsigned integer stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getUInt('anUInt')\n\n"))

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
            PvObjectGetLong(args("name='value'"), "Retrieves long value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Returns: long stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getLong('aLong')\n\n"))

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
            PvObjectGetULong(args("name='value'"), "Retrieves unsigned long value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Returns: unsigned long stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getULong('anULong')\n\n"))

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
            PvObjectGetFloat(args("name='value'"), "Retrieves float value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Returns: float stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getFloat('aFloat')\n\n"))

        .def("setDouble", static_cast<void(PvObject::*)(double)>(&PvObject::setDouble),
            args("value"),
            "Sets double value for the PV field named 'value'.\n\n:Parameter: *value* (float) - double value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setDouble(10.0)\n\n")

        .def("setDouble", static_cast<void(PvObject::*)(const std::string&,double)>(&PvObject::setDouble),
            args("name", "value"),
            "Sets double value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (float) - double value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setDouble('aDouble', 10.0)\n\n")

        .def("getDouble", 
            static_cast<double(PvObject::*)(const std::string&)const>(&PvObject::getDouble), 
            PvObjectGetDouble(args("name='value'"), "Retrieves double value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Returns: double stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getDouble('aDouble')\n\n"))

        .def("setString", 
            static_cast<void(PvObject::*)(const std::string&)>(&PvObject::setString),
            args("value"),
            "Sets string value for the PV field named 'value'.\n\n:Parameter: *value* (str) - string value that will be assigned to the field named 'value'\n\n**Example:**\n::\n\n    pv.setString('string value')\n\n")

        .def("setString", 
            static_cast<void(PvObject::*)(const std::string&,const std::string&)>(&PvObject::setString),
            args("name", "value"),
            "Sets string value for the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Parameter: *value* (str) - string value that will be assigned to the given field\n\n**Example:**\n::\n\n    pv.setString('aString', 'string value')\n\n")
        
        .def("getString", static_cast<std::string(PvObject::*)(const std::string&)const>(&PvObject::getString),
            PvObjectGetString(args("name='value'"), "Retrieves string value assigned to the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Returns: string stored in the given PV field\n\n**Example:**\n::\n\n    value = pv.getString('aString')\n\n"))

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
            PvObjectGetScalarArray(args("name='value'"), "Retrieves scalar array assigned to the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Returns: list of scalar values stored in the given PV field\n\n**Example:**\n::\n\n    valueList = pv.getScalarArray('aScalarArray')\n\n"))

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
            PvObjectGetStructure(args("name='value'"), "Retrieves structure assigned to the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Returns: dictionary of key:value pairs for PV structure stored in the given PV field\n\n**Example:**\n::\n\n    valueDict = pv.getStructure('aStruct')\n\n"))

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
            PvObjectGetStructureArray(args("name='value'"), "Retrieves structure array assigned to the given PV field.\n\n:Parameter: *name* (str) - field name\n\n:Returns: list of dictionaries (describing PV structures) stored in the given PV field\n\n**Example:**\n::\n\n    dictList = pv.getStructureArray('aStructArray')\n\n"))

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
    class_<PvScalar, bases<PvObject> >("PvScalar", "PvScalar is a base class for all scalar PV types. It cannot be instantiated directly from python.\n\n", no_init)
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
    class_<PvBoolean, bases<PvScalar> >("PvBoolean", "PvBoolean represents PV boolean type.\n\n**PvBoolean([value=False])**\n\n\t:Parameter: value (bool) - boolean value\n\n\t::\n\n\t\tpv = PvBoolean(True)\n\n", init<>())
        .def(init<bool>())
        .def("get", &PvBoolean::get, "Retrieves boolean PV value.\n\n:Returns: boolean value\n\n::\n\n    value = pv.get()\n\n")
        .def("set", &PvBoolean::set, args("value"), "Sets boolean PV value.\n\n:Parameter: value (bool) - boolean value\n\n::\n\n    pv.set(False)\n\n")
        ;

    //
    // PV Byte
    //
    class_<PvByte, bases<PvScalar> >("PvByte", "PvByte represents PV byte type.\n\n**PvByte([value=''])**\n\n\t:Parameter: value (str) - byte value\n\n\t::\n\n\t\tpv = PvByte('a')\n\n", init<>())
        .def(init<char>())
        .def("get", &PvByte::get, "Retrieves byte PV value.\n\n:Returns: byte value\n\n::\n\n    value = pv.get()\n\n")
        .def("set", &PvByte::set, args("value"), "Sets byte PV value.\n\n:Parameter: value (str) - byte value\n\n::\n\n    pv.set('a')\n\n")
        ;

    //
    // PV UByte
    //
    class_<PvUByte, bases<PvScalar> >("PvUByte", "PvUByte represents PV unsigned byte type.\n\n**PvUByte([value=0])**\n\n\t:Parameter: value (int) - unsigned byte value\n\n\t::\n\n\t\tpv = PvUByte(10)\n\n", init<>())
        .def(init<unsigned char>())
        .def("get", &PvUByte::get, "Retrieves unsigned byte PV value.\n\n:Returns: unsigned byte value\n\n::\n\n    value = pv.get()\n\n")
        .def("set", &PvUByte::set, args("value"), "Sets unsigned byte PV value.\n\n:Parameter: value (int) - unsigned byte value\n\n::\n\n    pv.set(10)\n\n")
        ;

    //
    // PV Short
    //
    class_<PvShort, bases<PvScalar> >("PvShort", "PvShort represents PV short type.\n\n**PvShort([value=0])**\n\n\t:Parameter: value (int) - short value\n\n\t::\n\n\t\tpv = PvShort(-10)\n\n", init<>())
        .def(init<short>())
        .def("get", &PvShort::get, "Retrieves short PV value.\n\n:Returns: short value\n\n::\n\n    value = pv.get()\n\n")
        .def("set", &PvShort::set, args("value"), "Sets short PV value.\n\n:Parameter: value (int) - short value\n\n::\n\n    pv.set(-10)\n\n")
        ;

    //
    // PV UShort
    //
    class_<PvUShort, bases<PvScalar> >("PvUShort", "PvUShort represents PV unsigned short type.\n\n**PvUShort([value=0])**\n\n\t:Parameter: value (int) - unsigned short value\n\n\t::\n\n\t\tpv = PvUShort(10)\n\n", init<>())
        .def(init<unsigned short>())
        .def("get", &PvUShort::get, "Retrieves unsigned short PV value.\n\n:Returns: unsigned short value\n\n::\n\n    value = pv.get()\n\n")
        .def("set", &PvUShort::set, args("value"), "Sets unsigned short PV value.\n\n:Parameter: value (int) - unsigned short value\n\n::\n\n    pv.set(10)\n\n")
        ;

    //
    // PV Int
    //
    class_<PvInt, bases<PvScalar> >("PvInt", "PvInt represents PV integer type.\n\n**PvInt([value=0])**\n\n\t:Parameter: value (int) - integer value\n\n\t::\n\n\t\tpv = PvInt(-1000)\n\n", init<>())
        .def(init<int>())
        .def("get", &PvInt::get, "Retrieves integer PV value.\n\n:Returns: integer value\n\n::\n\n    value = pv.get()\n\n")
        .def("set", &PvInt::set, args("value"), "Sets integer PV value.\n\n:Parameter: value (int) - integer value\n\n::\n\n    pv.set(-1000)\n\n")
        ;

    //
    // PV UInt
    //
    class_<PvUInt, bases<PvScalar> >("PvUInt", "PvUInt represents PV unsigned int type.\n\n**PvUInt([value=0])**\n\n\t:Parameter: value (int) - unsigned integer value\n\n\t::\n\n\t\tpv = PvUInt(1000)\n\n", init<>())
        .def(init<unsigned int>())
        .def("get", &PvUInt::get,  "Retrieves unsigned integer PV value.\n\n:Returns: unsigned integer value\n\n::\n\n    value = pv.get()\n\n")
        .def("set", &PvUInt::set, args("value"), "Sets unsigned integer PV value.\n\n:Parameter: value (int) - unsigned integer value\n\n::\n\n    pv.set(1000)\n\n")
        ;

    //
    // PV Long
    //
    class_<PvLong, bases<PvScalar> >("PvLong", "PvLong represents PV long type.\n\n**PvLong([value=0])**\n\n\t:Parameter: value (long) - long value\n\n\t::\n\n\t\tpv = PvLong(-100000L)\n\n", init<>())
        .def(init<long long>())
        .def("get", &PvLong::get, "Retrieves long PV value.\n\n:Returns: long value\n\n::\n\n    value = pv.get()\n\n")
        .def("set", &PvLong::set, "Sets long PV value.\n\n:Parameter: value (long) - long value\n\n::\n\n    pv.set(-100000L)\n\n")
        ;

    //
    // PV ULong
    //
    class_<PvULong, bases<PvScalar> >("PvULong", "PvULong represents PV unsigned long type.\n\n**PvULong([value=0])**\n\n\t:Parameter: value (long) - unsigned long value\n\n\t::\n\n\t\tpv = PvULong(100000L)\n\n", init<>())
        .def(init<unsigned long long>())
        .def("get", &PvULong::get, "Retrieves unsigned long PV value.\n\n:Returns: unsigned long value\n\n::\n\n    value = pv.get()\n\n")
        .def("set", &PvULong::set, args("value"), "Sets unsigned long PV value.\n\n:Parameter: value (long) - unsigned long value\n\n::\n\n    pv.set(100000L)\n\n")
        ;

    //
    // PV Float
    //
    class_<PvFloat, bases<PvScalar> >("PvFloat", "PvFloat represents PV float type.\n\n**PvFloat([value=0])**\n\n\t:Parameter: value (float) - float value\n\n\t::\n\n\t\tpv = PvFloat(1.1)\n\n", init<>())
        .def(init<float>())
        .def("get", &PvFloat::get, "Retrieves float PV value.\n\n:Returns: float value\n\n::\n\n    value = pv.get()\n\n")
        .def("set", &PvFloat::set, args("value"), "Sets float PV value.\n\n:Parameter: value (float) - float value\n\n::\n\n    pv.set(1.1)\n\n")
        ;

    //
    // PV Double
    //
    class_<PvDouble, bases<PvScalar> >("PvDouble", "PvDouble represents PV double type.\n\n**PvDouble([value=0])**\n\n\t:Parameter: value (float) - double value\n\n\t::\n\n\t\tpv = PvDouble(1.1)\n\n", init<>())
        .def(init<double>())
        .def("get", &PvDouble::get, "Retrieves double PV value.\n\n:Returns: double value\n\n::\n\n    value = pv.get()\n\n")
        .def("set", &PvDouble::set, args("value"), "Sets double PV value.\n\n:Parameter: value (float) - double value\n\n::\n\n    pv.set(1.1)\n\n")
        ;

    //
    // PV String
    //
    class_<PvString, bases<PvScalar> >("PvString", "PvString represents PV string type.\n\n**PvString([value=''])**\n\n\t:Parameter: value (str) - string value\n\n\t::\n\n\t\tpv = PvString('stringValue')\n\n", init<>())
        .def(init<std::string>())
        .def("get", &PvString::get, "Retrieves string PV value.\n\n:Returns: string value\n\n::\n\n    value = pv.get()\n\n")
        .def("set", &PvString::set, args("value"), "Sets string PV value.\n\n:Parameter: value (str) - string value\n\n::\n\n    pv.set('stringValue')\n\n")
        ;

    //
    // PV Scalar Array
    //
    class_<PvScalarArray, bases<PvObject> >("PvScalarArray", "PvScalarArray represents PV scalar array.\n\n**PvScalarArray(scalarType)**\n\n\t:Parameter: scalarType (PVTYPE) - scalar type of array elements\n\n\t- PVTYPE: scalar type, can be BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING\n\n\t::\n\n\t\tpv = PvScalarArray(INT)\n\n", init<PvType::ScalarType>())
        .def("get", &PvScalarArray::get, "Retrieves PV value list.\n\n:Returns: list of scalar values\n\n::\n\n    valueList = pv.get()\n\n")
        .def("set", &PvScalarArray::set, args("valueList"), "Sets PV value list.\n\n:Parameter: valueList (list) - list of scalar values\n\n::\n\n    pv.set([1,2,3,4,5])\n\n")
        .def("toList", &PvScalarArray::toList, "Converts PV to value list.\n\n:Returns: list of scalar values\n\n::\n\n    valueList = pv.toList()\n\n")
        ;

    //
    // PV TimeStamp
    //
    class_<PvTimeStamp, bases<PvObject> >("PvTimeStamp", "PvTimeStamp represents PV time stamp structure.\n\n**PvTimeStamp()**\n\n\t::\n\n\t\ttimestamp1 = PvTimeStamp()\n\n**PvTimeStamp(secondsPastEpoch, nanoseconds [, userTag=-1])**\n\n\t:Parameter: secondsPastEpoch (long) - seconds past epoch\n\n\t:Parameter: nanoseconds (int) - nanoseconds\n\n\t:Parameter: userTag (int) - user tag\n\n\t::\n\n\t\ttimeStamp2 = PvTimeStamp(1234567890, 10000)\n\n\t\ttimeStamp3 = PvTimeStamp(1234567890, 10000, 1)\n\n", init<>())
        .def(init<long long, int>())
        .def(init<long long, int, int>())
        .def("getSecondsPastEpoch", &PvTimeStamp::getSecondsPastEpoch, "Retrieves time stamp value for seconds past epoch.\n\n:Returns: seconds past epoch\n\n::\n\n    secondsPastEpoch = timeStamp.getSecondsPastEpoch()\n\n")
        .def("setSecondsPastEpoch", &PvTimeStamp::setSecondsPastEpoch, args("secondsPastEpoch"), "Sets time stamp value for seconds past epoch.\n\n:Parameter: secondsPastEpoch (long) - seconds past epoch\n\n::\n\n    timeStamp.setSecondsPastEpoch(1234567890)\n\n")
        .def("getNanoseconds", &PvTimeStamp::getNanoseconds, "Retrieves time stamp value for nanoseconds.\n\n:Returns: nanoseconds\n\n::\n\n    nanoseconds = timeStamp.getNanoseconds()\n\n")
        .def("setNanoseconds", &PvTimeStamp::setNanoseconds, args("nanoseconds"), "Sets time stamp value for nanoseconds.\n\n:Parameter: nanoseconds (int) - nanoseconds\n\n::\n\n    timeStamp.setNanoseconds(10000)\n\n")
        .def("getUserTag", &PvTimeStamp::getUserTag, "Retrieves user tag.\n\n:Returns: user tag\n\n::\n\n    userTag = timeStamp.getUserTag()\n\n")
        .def("setUserTag", &PvTimeStamp::setUserTag, args("userTag"), "Sets user tag.\n\n:Parameter: userTag (int) - user tag\n\n::\n\n    timeStamp.setUserTag(1)\n\n")
        ;

    //
    // PV Alarm
    //
    class_<PvAlarm, bases<PvObject> >("PvAlarm", "PvAlarm represents PV alarm structure.\n\n**PvAlarm()**\n\n\t::\n\n\t\talarm1 = PvAlarm()\n\n**PvAlarm(severity, status, message)**\n\n\t:Parameter: severity (int) - alarm severity\n\n\t:Parameter: status (int) - status code\n\n\t:Parameter: message (str) - alarm message\n\n\t::\n\n\t\talarm2 = PvAlarm(5, 1, 'alarm message')\n\n", init<>())
        .def(init<int, int, const std::string&>())
        .def("getSeverity", &PvAlarm::getSeverity, "Retrieves alarm severity.\n\n:Returns: alarm severity\n\n::\n\n    severity = alarm.getSeverity()\n\n")
        .def("setSeverity", &PvAlarm::setSeverity, args("severity"), "Sets alarm severity.\n\n:Parameter: severity (int) - alarm severity\n\n::\n\n    alarm.setSeverity(1)\n\n")
        .def("getStatus", &PvAlarm::getStatus, "Retrieves status code.\n\n:Returns: status code\n\n::\n\n    status = alarm.getStatusCode()\n\n")
        .def("setStatus", &PvAlarm::setStatus, args("status"), "Sets status code.\n\n:Parameter: status (int) - status code\n\n::\n\n    alarm.setStatus(1)\n\n")
        .def("getMessage", &PvAlarm::getMessage, "Retrieves alarm message.\n\n:Returns: alarm message\n\n::\n\n    message = alarm.getMessage()\n\n")
        .def("setMessage", &PvAlarm::setMessage, args("message"), "Sets alarm message.\n\n:Parameter: message (str) - alarm message\n\n::\n\n    alarm.setmessage('alarm message')\n\n")
        ;

    //
    // NT Type
    //
    class_<NtType, bases<PvObject> >("NtType", "NtType is a base class for all NT structures. It cannot be instantiated directly from python.\n\n", no_init)
        ;

    //
    // NT Table 
    //
    class_<NtTable, bases<NtType> >("NtTable", "NtTable represents NT table structure.\n\n**NtTable(nColumns, scalarType)**\n\n\t:Parameter: nColumns (int) - number of table columns\n\n\t:Parameter: scalarType (PVTYPE) - scalar type (BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING)\n\n\tThis example creates NT Table with 3 columns of DOUBLE values:\n\n\t::\n\n\t\ttable1 = NtTable(3, DOUBLE)\n\n\t**NtTable(scalarTypeList)**\n\n\t:Parameter: scalarTypeList ([PVTYPE]) - list of column scalar types (BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING)\n\n\tThis example creates NT Table with STRING, INT and DOUBLE columns:\n\n\t::\n\n\t\ttable2 = NtTable([STRING, INT, DOUBLE])\n\n**NtTable(pvObject)**\n\n\t:Parameter: pvObject (PvObject) - PV object that has a structure containing required NT Table elements:\n\n\t- labels ([STRING]) - list of column labels\n\n\t- value (dict) - dictionary of column<index>:[PVTYPE] pairs, where <index> is an integer in range [0,N-1], with N being NT Table dimension\n\n\tThe following example creates NT Table with 3 DOUBLE columns:\n\n\t::\n\n\t\tpvObject = PvObject({'labels' : [STRING], 'value' : {'column0' : [DOUBLE], 'column1' : [DOUBLE], 'column2' : [DOUBLE]}})\n\n\t\tpvObject.setScalarArray('labels', ['x', 'y', 'z'])\n\n\t\tpvObject.setStructure('value', {'column0' : [0.1, 0.2, 0.3], 'column1' : [1.1, 1.2, 1.3], 'column2' : [2.1, 2.2, 2.3]})\n\n\t\ttable3 = NtTable(pvObject)", init<int, PvType::ScalarType>())
        .def(init<const boost::python::list&>())
        .def(init<const PvObject&>())
        .def("getNColumns", &NtTable::getNColumns, "Retrieves number of columns.\n\n:Returns: number of table columns\n\n::\n\n    nColumns = table.getNColumns()\n\n")
        .def("getLabels", &NtTable::getLabels, "Retrieves list of column labels.\n\n:Returns: list of column labels\n\n::\n\n    labelList = table.getLabels()\n\n")
        .def("setLabels", &NtTable::setLabels, args("labelList"), "Sets column labels.\n\n:Parameter: labelList ([str]) - list of strings containing column labels (the list length must match number of table columns)\n\n::\n\n    table.setLabels(['String', 'Int', 'Double'])\n\n")
        .def("getColumn", &NtTable::getColumn, args("index"), "Retrieves specified column.\n\n:Parameter: index (int) - column index (must be in range [0,N-1], where N is the number of table columns)\n\n:Returns: list of values stored in the specified table column\n\n::\n\n    valueList = table.getColumn(0)\n\n")
        .def("setColumn", &NtTable::setColumn, args("index", "valueList"), "Sets column values.\n\n:Parameter: index (int) - column index\n\n:Parameter: valueList (list) - list of column values\n\n::\n\n    table.setColumn(0, ['x', 'y', 'z'])\n\n")
        .def("getDescriptor", &NtTable::getDescriptor, "Retrieves table descriptor.\n\n:Returns: table descriptor\n\n::\n\n    descriptor = table.getDescriptor()\n\n")
        .def("setDescriptor", &NtTable::setDescriptor, args("descriptor"), "Sets table descriptor.\n\n:Parameter: descriptor (str) - table descriptor\n\n::\n\n    table.setDescriptor('myTable')\n\n")
        .def("getTimeStamp", &NtTable::getTimeStamp, "Retrieves table time stamp.\n\n:Returns: table time stamp object\n\n::\n\n    timeStamp = table.getTimeStamp()\n\n")
        .def("setTimeStamp", &NtTable::setTimeStamp, args("timeStamp"), "Sets table time stamp.\n\n:Parameter: timeStamp (PvTimeStamp) - table time stamp object\n\n::\n\n    timeStamp = PvTimeStamp(1234567890, 10000, 1)\n\n    table.setTimeStamp(timeStamp)\n\n")
        .def("getAlarm", &NtTable::getAlarm, "Retrieves table alarm.\n\n:Returns: table alarm object\n\n::\n\n    alarm = table.getAlarm()\n\n")
        .def("setAlarm", &NtTable::setAlarm, args("alarm"), "Sets table alarm.\n\n:Parameter: alarm (PvAlarm) - table alarm object\n\n::\n\n    alarm = PvAlarm(11, 126, 'Server SegFault')\n\n    table.setAlarm(alarm)\n\n")
        ;

    // Channel
    class_<Channel>("Channel", "This class represents PV channels.\n\n**Channel(name [, providerType=PVA])**\n\n\t:Parameter: name (str) - channel name\n\n\t:Parameter: providerType (PROVIDERTYPE) - provider type, either PVA (PV Access) or CA (Channel Access)\n\n\tNote that PV structures representing objects on CA channels always have a single key 'value'.\n\tThe following example creates PVA channel 'enum01':\n\n\t::\n\n\t\tpvaChannel = Channel('enum01')\n\n\tThis example allows access to CA channel 'CA:INT':\n\n\t::\n\n\t\tcaChannel = Channel('CA:INT', CA)\n\n", init<std::string>())
        .def(init<std::string, PvProvider::ProviderType>())
        .def("get", static_cast<PvObject*(Channel::*)(const std::string&)>(&Channel::get), 
            return_value_policy<manage_new_object>(), args("requestDescriptor"), "Retrieves PV data from the channel.\n\n:Parameter: requestDescriptor (str) - PV request descriptor\n\n:Returns: channel PV data corresponding to the specified request descriptor\n\n::\n\n    channel = Channel('enum01')\n\n    pv = channel.get('field(value.index)')\n\n")
        .def("get", static_cast<PvObject*(Channel::*)()>(&Channel::get), 
            return_value_policy<manage_new_object>(), "Retrieves PV data from the channel using the default request descriptor 'field(value)'.\n\n:Returns: channel PV data\n\n::\n\n    pv = channel.get()\n\n")
        .def("put", static_cast<void(Channel::*)(const PvObject&, const std::string&)>(&Channel::put), args("pv", "requestDescriptor"), "Assigns PV data to the channel process variable.\n\n:Parameter: pv (PvObject) - PV object that will be assigned to channel PV according to the specified request descriptor\n\n:Parameter: requestDescriptor (str) - PV request descriptor\n\n::\n\n    channel = Channel('enum01')\n\n    channel.put(PvInt(1), 'field(value.index)')\n\n")
        .def("put", static_cast<void(Channel::*)(const PvObject&)>(&Channel::put), args("pv"), "Assigns PV data to the channel process variable using the default request descriptor 'field(value)'.\n\n:Parameter: pv (PvObject) - PV object that will be assigned to the channel process variable\n\n::\n\n    channel = Channel('int01')\n\n    channel.put(PvInt(1))\n\n")
        .def("putString", static_cast<void(Channel::*)(const std::string&, const std::string&)>(&Channel::put), args("value", "requestDescriptor"), "Assigns string data to the channel PV.\n\n:Parameter: value (str) - string value that will be assigned to channel data according to the specified request descriptor\n\n:Parameter: requestDescriptor (str) - PV request descriptor\n\n")
        .def("putString", static_cast<void(Channel::*)(const std::string&)>(&Channel::put), args("value"), "Assigns string data to the channel PV using the default request descriptor 'field(value)'.\n\n:Parameter: value (str) - string value that will be assigned to the channel PV\n\n::\n\n    channel = Channel('string01')\n\n    channel.put('string value')\n\n")
        .def("putScalarArray", static_cast<void(Channel::*)(const boost::python::list&, const std::string&)>(&Channel::put))
        .def("putScalarArray", static_cast<void(Channel::*)(const boost::python::list&)>(&Channel::put))
        .def("putBoolean", static_cast<void(Channel::*)(bool, const std::string&)>(&Channel::put), args("value", "requestDescriptor"), "Assigns boolean data to the channel PV.\n\n:Parameter: value (bool) - boolean value that will be assigned to channel data according to the specified request descriptor\n\n:Parameter: requestDescriptor (str) - PV request descriptor\n\n")
        .def("putBoolean", static_cast<void(Channel::*)(bool)>(&Channel::put), args("value"), "Assigns boolean data to the channel PV using the default request descriptor 'field(value)'.\n\n:Parameter: value (bool) - boolean value that will be assigned to the channel PV\n\n::\n\n    channel = Channel('boolean01')\n\n    channel.putBoolean(True)\n\n")
        .def("putByte", static_cast<void(Channel::*)(char, const std::string&)>(&Channel::put), args("value", "requestDescriptor"), "Assigns byte data to the channel PV.\n\n:Parameter: value (str) - byte value that will be assigned to channel data according to the specified request descriptor\n\n:Parameter: requestDescriptor (str) - PV request descriptor\n\n")
        .def("putByte", static_cast<void(Channel::*)(char)>(&Channel::put), args("value"), "Assigns byte data to the channel using the default request descriptor 'field(value)'.\n\n:Parameter: value (str) - byte value that will be assigned to the channel PV\n\n::\n\n    channel = Channel('byte01')\n\n    channel.putByte('a')\n\n")
        .def("putUByte", static_cast<void(Channel::*)(unsigned char, const std::string&)>(&Channel::put), args("value", "requestDescriptor"), "Assigns unsigned byte data to the channel PV.\n\n:Parameter: value (int) - unsigned byte value that will be assigned to channel data according to the specified request descriptor\n\n:Parameter: requestDescriptor (str) - PV request descriptor\n\n")
        .def("putUByte", static_cast<void(Channel::*)(unsigned char)>(&Channel::put), args("value"), "Assigns unsigned byte data to the channel PV using the default request descriptor 'field(value)'.\n\n:Parameter: value (int) - unsigned byte value that will be assigned to the channel PV\n\n::\n\n    channel = Channel('ubyte01')\n\n    channel.putUByte(10)\n\n")
        .def("putShort", static_cast<void(Channel::*)(short, const std::string&)>(&Channel::put), args("value", "requestDescriptor"), "Assigns short data to the channel PV.\n\n:Parameter: value (int) - short value that will be assigned to channel data according to the specified request descriptor\n\n:Parameter: requestDescriptor (str) - PV request descriptor\n\n")
        .def("putShort", static_cast<void(Channel::*)(short)>(&Channel::put), args("value"), "Assigns short data to the channel PV using the default request descriptor 'field(value)'.\n\n:Parameter: value (int) - short value that will be assigned to the channel PV\n\n::\n\n    channel = Channel('short01')\n\n    channel.putShort(10)\n\n")
        .def("putUShort", static_cast<void(Channel::*)(unsigned short, const std::string&)>(&Channel::put), args("value", "requestDescriptor"), "Assigns unsigned short data to the channel PV.\n\n:Parameter: value (int) - unsigned short value that will be assigned to channel data according to the specified request descriptor\n\n:Parameter: requestDescriptor (str) - PV request descriptor\n\n")
        .def("putUShort", static_cast<void(Channel::*)(unsigned short)>(&Channel::put), args("value"), "Assigns unsigned short data to the channel PV using the default request descriptor 'field(value)'.\n\n:Parameter: value (int) - unsigned short value that will be assigned to the channel PV\n\n::\n\n    channel = Channel('ushort01')\n\n    channel.putUShort(10)\n\n")
        .def("putInt", static_cast<void(Channel::*)(int, const std::string&)>(&Channel::put), args("value", "requestDescriptor"), "Assigns integer data to the channel PV.\n\n:Parameter: value (int) - integer value that will be assigned to channel data according to the specified request descriptor\n\n:Parameter: requestDescriptor (str) - PV request descriptor\n\n")
        .def("putInt", static_cast<void(Channel::*)(int)>(&Channel::put), args("value"), "Assigns integer data to the channel PV using the default request descriptor 'field(value)'.\n\n:Parameter: value (int) - integer value that will be assigned to the channel PV\n\n::\n\n    channel = Channel('int01')\n\n    channel.putInt(1000)\n\n")
        .def("putUInt", static_cast<void(Channel::*)(unsigned int, const std::string&)>(&Channel::put), args("value", "requestDescriptor"), "Assigns unsigned integer data to the channel PV.\n\n:Parameter: value (int) - unsigned integer value that will be assigned to channel data according to the specified request descriptor\n\n:Parameter: requestDescriptor (str) - PV request descriptor\n\n")
        .def("putUInt", static_cast<void(Channel::*)(unsigned int)>(&Channel::put), args("value"), "Assigns unsigned integer data to the channel PV using the default request descriptor 'field(value)'.\n\n:Parameter: value (int) - unsigned integer value that will be assigned to the channel PV\n\n::\n\n    channel = Channel('uint01')\n\n    channel.putUInt(1000)\n\n")
        .def("putLong", static_cast<void(Channel::*)(long long, const std::string&)>(&Channel::put), args("value", "requestDescriptor"), "Assigns long data to the channel PV.\n\n:Parameter: value (long) - long value that will be assigned to channel data according to the specified request descriptor\n\n:Parameter: requestDescriptor (str) - PV request descriptor\n\n")
        .def("putLong", static_cast<void(Channel::*)(long long)>(&Channel::put), args("value"), "Assigns long data to the channel PV using the default request descriptor 'field(value)'.\n\n:Parameter: value (long) - long value that will be assigned to the channel PV\n\n::\n\n    channel = Channel('long01')\n\n    channel.putLong(100000L)\n\n")
        .def("putULong", static_cast<void(Channel::*)(unsigned long long, const std::string&)>(&Channel::put), args("value", "requestDescriptor"), "Assigns unsigned long data to the channel PV.\n\n:Parameter: value (long) - unsigned long value that will be assigned to channel data according to the specified request descriptor\n\n:Parameter: requestDescriptor (str) - PV request descriptor\n\n")
        .def("putULong", static_cast<void(Channel::*)(unsigned long long)>(&Channel::put), args("value"), "Assigns unsigned long data to the channel PV using the default request descriptor 'field(value)'.\n\n:Parameter: value (long) - unsigned long value that will be assigned to the channel PV\n\n::\n\n    channel = Channel('ulong01')\n\n    channel.putULong(100000L)\n\n")
        .def("putFloat", static_cast<void(Channel::*)(float, const std::string&)>(&Channel::put), args("value", "requestDescriptor"), "Assigns float data to the channel PV.\n\n:Parameter: value (float) - float value that will be assigned to channel data according to the specified request descriptor\n\n:Parameter: requestDescriptor (str) - PV request descriptor\n\n")
        .def("putFloat", static_cast<void(Channel::*)(float)>(&Channel::put), args("value"), "Assigns float data to the channel PV using the default request descriptor 'field(value)'.\n\n:Parameter: value (float) - float value that will be assigned to the channel PV\n\n::\n\n    channel = Channel('float01')\n\n    channel.putFloat(1.1)\n\n")
        .def("putDouble", static_cast<void(Channel::*)(double, const std::string&)>(&Channel::put), args("value", "requestDescriptor"), "Assigns double data to the channel PV.\n\n:Parameter: value (float) - double value that will be assigned to channel data according to the specified request descriptor\n\n:Parameter: requestDescriptor (str) - PV request descriptor\n\n")
        .def("putDouble", static_cast<void(Channel::*)(double)>(&Channel::put), args("value"), "Puts double data into the channel using the default request descriptor 'field(value)'.\n\n:Parameter: value (float) - double value that will be assigned to the channel PV\n\n::\n\n    channel = Channel('double01')\n\n    channel.putDouble(1.1)\n\n")
        .def("subscribe", &Channel::subscribe, args("subscriberName", "subscriber"), "Subscribes python object to notifications of changes in PV value. Channel can have any number of subscribers that start receiving PV updates after *startMonitor()* is invoked. Updates stop after channel monitor is stopped via *stopMonitor()* call, or object is unsubscribed from notifications using *unsubscribe()* call.\n\n:Parameter: name (str) - subscriber object name\n\n:Parameter: subscriber (object) - reference to python subscriber object (e.g., python function) that will be executed when PV value changes\n\nThe following code snippet defines a simple subscriber object, subscribes it to PV value changes, and starts channel monitor:\n\n::\n\n    def echo(x):\n\n        print 'New PV value: ', x\n\n    channel = Channel('float01')\n\n    channel.subscribe('echo', echo)\n\n    channel.startMonitor()\n\n")
        .def("unsubscribe", &Channel::unsubscribe, args("name"), "Unsubscribes subscriber object from notifications of changes in PV value.\n\n:Parameter: name (str) - subscriber name\n\n::\n\n    channel.unsubscribe('echo')\n\n")
        .def("startMonitor", static_cast<void(Channel::*)(const std::string&)>(&Channel::startMonitor), args("requestDescriptor"), "Starts channel monitor for PV value changes.\n\n:Parameter: requestDescriptor (str) - describes what PV data should be sent to subscribed channel clients\n\n::\n\n    channel.startMonitor('field(value.index)')\n\n")
        .def("startMonitor", static_cast<void(Channel::*)()>(&Channel::startMonitor), "Starts channel monitor for PV value changes using the default request descriptor 'field(value)'.\n\n::\n\n    channel.startMonitor()\n\n")
        .def("stopMonitor", &Channel::stopMonitor, "Stops channel monitor for PV value changes.\n\n::\n\n    channel.stopMonitor()\n\n")
        .def("getTimeout", &Channel::getTimeout, "Retrieves channel timeout.\n\n:Returns: channel timeout in seconds\n\n::\n\n    timeout = channel.getTimeout()\n\n")
        .def("setTimeout", &Channel::setTimeout, args("timeout"), "Sets channel timeout.\n\n:Parameter: timeout (float) - channel timeout in seconds\n\n::\n\n    channel.setTimeout(10.0)\n\n")
        ;

    // RPC Client
    class_<RpcClient>("RpcClient", "RpcClient is a client class for PVA RPC services.\n\n**RpcClient(channelName)**\n\n\t:Parameter: channelName (str) - RPC service channel name\n\n\tThis example creates RPC client for channel 'createNtTable':\n\n\t::\n\n\t\trpcClient = RpcClient('createNtTable')\n\n", init<std::string>())
        .def("invoke", &RpcClient::invoke, return_value_policy<manage_new_object>(), args("pvRequest"), "Invokes RPC call against service registered on the PV specified channel.\n\n:Parameter: pvRequest (PvObject) - PV request object with a structure conforming to requirements of the RPC service registered on the given PV channel\n\n:Returns: PV response object\n\nThe following code works with the above RPC service example:\n\n::\n\n    pvRequest = PvObject({'nRows' : INT, 'nColumns' : INT})\n\n    pvRequest.set({'nRows' : 10, 'nColumns' : 10})\n\n    pvResponse = rpcClient(pvRequest)\n\n    ntTable = NtTable(pvRequest)\n\n")
        ;
    
    // RPC Service Impl
    //class_<RpcServiceImpl, boost::shared_ptr<RpcServiceImpl> >("RpcServiceImpl", init<boost::python::object>())
        ;

    // RPC Server
    class_<RpcServer>("RpcServer", "RpcServer is class used for hosting PVA RPC services. One instance of RpcServer can host multiple RPC services.\n\n**RpcServer()**:\n\n\t::\n\n\t\trpcServer = RpcServer()\n\n", init<>())
        .def("registerService", &RpcServer::registerService, args("serviceName", "serviceImpl"), "Registers service implementation with RPC server. Typically, all services are registered before RPC server starts listening for client requests.\n\n:Parameter: serviceName (str) - service name (name of the PV channel used for RPC client/server communication)\n\n:Parameter: serviceImpl (object) - reference to service implementation object (e.g., python function) that returns PV Object upon invocation\n\nThe following is an example of RPC service that creates NT Table according to client specifications:\n\n::\n\n    import pvaccess\n\n    import random\n\n    def createNtTable(pvRequest):\n\n        nRows = x.getInt('nRows')\n\n        nColumns = x.getInt('nColumns')\n\n        print 'Creating table with %d rows and %d columns' % (nRows, nColumns)\n\n        ntTable = pvaccess.NtTable(nColumns, pvaccess.DOUBLE)\n\n        labels = []\n\n        for j in range (0, nColumns):\n\n            labels.append('Column%s' % j)\n\n            column = []\n\n            for i in range (0, nRows):\n\n                column.append(random.uniform(0,1))\n\n            ntTable.setColumn(j, column)\n\n        ntTable.setLabels(labels)\n\n        ntTable.setDescriptor('Automatically created by pvaPy RPC Server')\n\n        return ntTable\n\n    \n\n    rpcServer = pvaccess.RpcServer()\n\n    rpcServer.registerService('createNtTable', createNtTable)\n\n    rpcServer.listen()\n\n")
        .def("unregisterService", &RpcServer::unregisterService, args("serviceName"), "Unregisters given service from RPC server.\n\n:Parameter: serviceName (str) - service name (name of the PV channel used for RPC client/server communication)\n\n::\n\n    rpcServer.unregisterService('createNtTable')\n\n")
        .def("startListener", &RpcServer::startListener, "Starts RPC listener in its own thread. This method is typically used for multi-threaded programs, or for testing and debugging in python interactive mode. It should be used in conjunction with *stopListener()* call.\n\n::\n\n    rpcServer.startListener()")
        .def("stopListener", &RpcServer::stopListener, "Stops RPC listener thread. This method is used in conjunction with *startListener()* call.\n\n::\n\n    rpcServer.stopListener()\n\n")
        .def("start", &RpcServer::start, "Start serving RPC requests. This method is equivalent to *listen()*, and blocks until either *stop()* or *shutdown()* methods are invoked.\n\n::\n\n    rpcServer.start()")
        .def("stop", &RpcServer::stop, "Stop serving RPC requests. This method is equivalent to *shutdown()*.\n\n::\n\n    rpcServer.stop()")
        .def("listen", static_cast<void(RpcServer::*)(int)>(&RpcServer::listen), RpcServerListen(args("seconds=0"), "Start serving RPC requests.\n\n:Parameter: seconds (int) - specifies the amount of time server should be listening for requests (0 indicates 'forever')\n\n::\n\n    rpcServer.listen(60)\n\n"))
        .def("shutdown", &RpcServer::shutdown, "Stop serving RPC requests. This method is equivalent to *stop()*.\n\n::\n\n    rpcServer.shutdown()")
        ;
    
}
