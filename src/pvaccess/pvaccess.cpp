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
#include "boost/shared_ptr.hpp"
#include "boost/operators.hpp"

#include "PvObject.h"
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

//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RpcClientRequest, RpcClient::request, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RpcServerListen, RpcServer::listen, 0, 1)

BOOST_PYTHON_MODULE(pvaccess)
{
    using namespace boost::python;

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
    // PvObject
    //
    class_<PvObject>("PvObject", init<boost::python::dict>())
        .def(str(self))

        .def("set", static_cast<void(PvObject::*)(const boost::python::dict&)>(&PvObject::set))
        .def("get", static_cast<boost::python::dict(PvObject::*)()const>(&PvObject::get))

        .def("setBoolean", static_cast<void(PvObject::*)(bool)>(&PvObject::setBoolean))
        .def("setBoolean", static_cast<void(PvObject::*)(const std::string&,bool)>(&PvObject::setBoolean))
        .def("getBoolean", static_cast<bool(PvObject::*)(const std::string&)const>(&PvObject::getBoolean), PvObjectGetBoolean())

        .def("setByte", static_cast<void(PvObject::*)(char)>(&PvObject::setByte))
        .def("setByte", static_cast<void(PvObject::*)(const std::string&,char)>(&PvObject::setByte))
        .def("getByte", static_cast<char(PvObject::*)(const std::string&)const>(&PvObject::getByte), PvObjectGetByte())

        .def("setUByte", static_cast<void(PvObject::*)(unsigned char)>(&PvObject::setUByte))
        .def("setUByte", static_cast<void(PvObject::*)(const std::string&,unsigned char)>(&PvObject::setUByte))
        .def("getUByte", static_cast<unsigned char(PvObject::*)(const std::string&)const>(&PvObject::getUByte), PvObjectGetUByte())

        .def("setShort", static_cast<void(PvObject::*)(short)>(&PvObject::setShort))
        .def("setShort", static_cast<void(PvObject::*)(const std::string&,short)>(&PvObject::setShort))
        .def("getShort", static_cast<short(PvObject::*)(const std::string&)const>(&PvObject::getShort), PvObjectGetShort())

        .def("setUShort", static_cast<void(PvObject::*)(unsigned short)>(&PvObject::setUShort))
        .def("setUShort", static_cast<void(PvObject::*)(const std::string&,unsigned short)>(&PvObject::setUShort))
        .def("getUShort", static_cast<unsigned short(PvObject::*)(const std::string&)const>(&PvObject::getUShort), PvObjectGetUShort())

        .def("setInt", static_cast<void(PvObject::*)(int)>(&PvObject::setInt))
        .def("setInt", static_cast<void(PvObject::*)(const std::string&,int)>(&PvObject::setInt))
        .def("getInt", static_cast<int(PvObject::*)(const std::string&)const>(&PvObject::getInt), PvObjectGetInt())

        .def("setUInt", static_cast<void(PvObject::*)(unsigned int)>(&PvObject::setUInt))
        .def("setUInt", static_cast<void(PvObject::*)(const std::string&,unsigned int)>(&PvObject::setUInt))
        .def("getUInt", static_cast<unsigned int(PvObject::*)(const std::string&)const>(&PvObject::getUInt), PvObjectGetUInt())

        .def("setLong", static_cast<void(PvObject::*)(long long)>(&PvObject::setLong))
        .def("setLong", static_cast<void(PvObject::*)(const std::string&,long long)>(&PvObject::setLong))
        .def("getLong", static_cast<long long(PvObject::*)(const std::string&)const>(&PvObject::getLong), PvObjectGetLong())

        .def("setULong", static_cast<void(PvObject::*)(unsigned long long)>(&PvObject::setULong))
        .def("setULong", static_cast<void(PvObject::*)(const std::string&,unsigned long long)>(&PvObject::setULong))
        .def("getULong", static_cast<unsigned long long(PvObject::*)(const std::string&)const>(&PvObject::getULong), PvObjectGetULong())

        .def("setFloat", static_cast<void(PvObject::*)(float)>(&PvObject::setFloat))
        .def("setFloat", static_cast<void(PvObject::*)(const std::string&,float)>(&PvObject::setFloat))
        .def("getFloat", static_cast<float(PvObject::*)(const std::string&)const>(&PvObject::getFloat), PvObjectGetFloat())

        .def("setDouble", static_cast<void(PvObject::*)(double)>(&PvObject::setDouble))
        .def("setDouble", static_cast<void(PvObject::*)(const std::string&,double)>(&PvObject::setDouble))
        .def("getDouble", static_cast<double(PvObject::*)(const std::string&)const>(&PvObject::getDouble), PvObjectGetDouble())

        .def("setString", static_cast<void(PvObject::*)(const std::string&)>(&PvObject::setString))
        .def("setString", static_cast<void(PvObject::*)(const std::string&,const std::string&)>(&PvObject::setString))
        .def("getString", static_cast<std::string(PvObject::*)(const std::string&)const>(&PvObject::getString), PvObjectGetString())

        .def("setScalarArray", static_cast<void(PvObject::*)(const boost::python::list&)>(&PvObject::setScalarArray))
        .def("setScalarArray", static_cast<void(PvObject::*)(const std::string&,const boost::python::list&)>(&PvObject::setScalarArray))
        .def("getScalarArray", static_cast<boost::python::list(PvObject::*)(const std::string&)const>(&PvObject::getScalarArray), PvObjectGetScalarArray())

        .def("setStructure", static_cast<void(PvObject::*)(const boost::python::dict&)>(&PvObject::setStructure))
        .def("setStructure", static_cast<void(PvObject::*)(const std::string&,const boost::python::dict&)>(&PvObject::setStructure))
        .def("getStructure", static_cast<boost::python::dict(PvObject::*)(const std::string&)const>(&PvObject::getStructure), PvObjectGetStructure())

        .def("setStructureArray", static_cast<void(PvObject::*)(const boost::python::list&)>(&PvObject::setStructureArray))
        .def("setStructureArray", static_cast<void(PvObject::*)(const std::string&,const boost::python::list&)>(&PvObject::setStructureArray))
        .def("getStructureArray", static_cast<boost::python::list(PvObject::*)(const std::string&)const>(&PvObject::getStructureArray), PvObjectGetStructureArray())

        .def("toDict", &PvObject::toDict)
        .def("getStructureDict", &PvObject::getStructureDict)
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
        .def("get", &Channel::get, 
            return_value_policy<manage_new_object>())
        .def("put", &Channel::put)
        .def("subscribe", &Channel::subscribe)
        .def("unsubscribe", &Channel::unsubscribe)
        .def("startMonitor", &Channel::startMonitor)
        .def("stopMonitor", &Channel::stopMonitor)
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
        .def("unregisterService", &RpcServer::registerService)
        .def("listen", static_cast<void(RpcServer::*)(int)>(&RpcServer::listen), RpcServerListen())
        ;
    
}
