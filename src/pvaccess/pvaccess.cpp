// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

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

#include "PvaException.h"
#include "FieldNotFound.h"
#include "InvalidArgument.h"
#include "InvalidDataType.h"
#include "InvalidRequest.h"

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
#include "PvUnion.h"

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
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ChannelGet, Channel::get, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ChannelPut, Channel::put, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ChannelStartMonitor, Channel::startMonitor, 0, 1)
//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RpcClientRequest, RpcClient::request, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RpcServerListen, RpcServer::listen, 0, 1)

PyObject* pvaException = NULL;
PyObject* fieldNotFoundException = NULL;
PyObject* invalidArgumentException = NULL;
PyObject* invalidDataTypeException = NULL;
PyObject* invalidRequestException = NULL;

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
    pvaException = PvaExceptionTranslator::createExceptionClass(PvaException::PyExceptionClassName);
    fieldNotFoundException = PvaExceptionTranslator::createExceptionClass(FieldNotFound::PyExceptionClassName, pvaException);
    invalidArgumentException = PvaExceptionTranslator::createExceptionClass(InvalidArgument::PyExceptionClassName, pvaException);
    invalidDataTypeException = PvaExceptionTranslator::createExceptionClass(InvalidDataType::PyExceptionClassName, pvaException);
    invalidRequestException = PvaExceptionTranslator::createExceptionClass(InvalidRequest::PyExceptionClassName, pvaException);

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

#include "pvaccess.PvObject.cpp"
#include "pvaccess.PvScalar.cpp"
#include "pvaccess.PvBoolean.cpp"
#include "pvaccess.PvByte.cpp"
#include "pvaccess.PvUByte.cpp"
#include "pvaccess.PvShort.cpp"
#include "pvaccess.PvUShort.cpp"
#include "pvaccess.PvInt.cpp"
#include "pvaccess.PvUInt.cpp"
#include "pvaccess.PvLong.cpp"
#include "pvaccess.PvULong.cpp"
#include "pvaccess.PvFloat.cpp"
#include "pvaccess.PvDouble.cpp"
#include "pvaccess.PvString.cpp"
#include "pvaccess.PvScalarArray.cpp"
#include "pvaccess.PvUnion.cpp"

#include "pvaccess.PvTimeStamp.cpp"
#include "pvaccess.PvAlarm.cpp"

    //
    // NT Type
    //
    class_<NtType, bases<PvObject> >("NtType", "NtType is a base class for all NT structures. It cannot be instantiated directly from python.\n\n", no_init)
        ;

    //
    // NT Table 
    //
    class_<NtTable, bases<NtType> >("NtTable", "NtTable represents NT table structure.\n\n**NtTable(nColumns, scalarType)**\n\n\t:Parameter: *nColumns* (int) - number of table columns\n\n\t:Parameter: *scalarType* (PVTYPE) - scalar type (BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING)\n\n\tThis example creates NT Table with 3 columns of DOUBLE values:\n\n\t::\n\n\t\ttable1 = NtTable(3, DOUBLE)\n\n\t**NtTable(scalarTypeList)**\n\n\t:Parameter: *scalarTypeList* ([PVTYPE]) - list of column scalar types (BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING)\n\n\tThis example creates NT Table with STRING, INT and DOUBLE columns:\n\n\t::\n\n\t\ttable2 = NtTable([STRING, INT, DOUBLE])\n\n**NtTable(pvObject)**\n\n\t:Parameter: *pvObject* (PvObject) - PV object that has a structure containing required NT Table elements:\n\n\t- labels ([STRING]) - list of column labels\n\n\t- value (dict) - dictionary of column<index>:[PVTYPE] pairs, where <index> is an integer in range [0,N-1], with N being NT Table dimension\n\n\tThe following example creates NT Table with 3 DOUBLE columns:\n\n\t::\n\n\t\tpvObject = PvObject({'labels' : [STRING], 'value' : {'column0' : [DOUBLE], 'column1' : [DOUBLE], 'column2' : [DOUBLE]}})\n\n\t\tpvObject.setScalarArray('labels', ['x', 'y', 'z'])\n\n\t\tpvObject.setStructure('value', {'column0' : [0.1, 0.2, 0.3], 'column1' : [1.1, 1.2, 1.3], 'column2' : [2.1, 2.2, 2.3]})\n\n\t\ttable3 = NtTable(pvObject)", init<int, PvType::ScalarType>())
        .def(init<const boost::python::list&>())
        .def(init<const PvObject&>())
        .def("getNColumns", &NtTable::getNColumns, "Retrieves number of columns.\n\n:Returns: number of table columns\n\n::\n\n    nColumns = table.getNColumns()\n\n")
        .def("getLabels", &NtTable::getLabels, "Retrieves list of column labels.\n\n:Returns: list of column labels\n\n::\n\n    labelList = table.getLabels()\n\n")
        .def("setLabels", &NtTable::setLabels, args("labelList"), "Sets column labels.\n\n:Parameter: *labelList* ([str]) - list of strings containing column labels (the list length must match number of table columns)\n\n::\n\n    table.setLabels(['String', 'Int', 'Double'])\n\n")
        .def("getColumn", &NtTable::getColumn, args("index"), "Retrieves specified column.\n\n:Parameter: *index* (int) - column index (must be in range [0,N-1], where N is the number of table columns)\n\n:Returns: list of values stored in the specified table column\n\n::\n\n    valueList = table.getColumn(0)\n\n")
        .def("setColumn", &NtTable::setColumn, args("index", "valueList"), "Sets column values.\n\n:Parameter: *index* (int) - column index\n\n:Parameter: *valueList* (list) - list of column values\n\n::\n\n    table.setColumn(0, ['x', 'y', 'z'])\n\n")
        .def("getDescriptor", &NtTable::getDescriptor, "Retrieves table descriptor.\n\n:Returns: table descriptor\n\n::\n\n    descriptor = table.getDescriptor()\n\n")
        .def("setDescriptor", &NtTable::setDescriptor, args("descriptor"), "Sets table descriptor.\n\n:Parameter: *descriptor* (str) - table descriptor\n\n::\n\n    table.setDescriptor('myTable')\n\n")
        .def("getTimeStamp", &NtTable::getTimeStamp, "Retrieves table time stamp.\n\n:Returns: table time stamp object\n\n::\n\n    timeStamp = table.getTimeStamp()\n\n")
        .def("setTimeStamp", &NtTable::setTimeStamp, args("timeStamp"), "Sets table time stamp.\n\n:Parameter: *timeStamp* (PvTimeStamp) - table time stamp object\n\n::\n\n    timeStamp = PvTimeStamp(1234567890, 10000, 1)\n\n    table.setTimeStamp(timeStamp)\n\n")
        .def("getAlarm", &NtTable::getAlarm, "Retrieves table alarm.\n\n:Returns: table alarm object\n\n::\n\n    alarm = table.getAlarm()\n\n")
        .def("setAlarm", &NtTable::setAlarm, args("alarm"), "Sets table alarm.\n\n:Parameter: *alarm* (PvAlarm) - table alarm object\n\n::\n\n    alarm = PvAlarm(11, 126, 'Server SegFault')\n\n    table.setAlarm(alarm)\n\n")
        ;

#include "pvaccess.Channel.cpp"

    // RPC Client
    class_<RpcClient>("RpcClient", "RpcClient is a client class for PVA RPC services.\n\n**RpcClient(channelName)**\n\n\t:Parameter: *channelName* (str) - RPC service channel name\n\n\tThis example creates RPC client for channel 'createNtTable':\n\n\t::\n\n\t\trpcClient = RpcClient('createNtTable')\n\n", init<std::string>())
        .def("invoke", &RpcClient::invoke, return_value_policy<manage_new_object>(), args("pvRequest"), "Invokes RPC call against service registered on the PV specified channel.\n\n:Parameter: *pvRequest* (PvObject) - PV request object with a structure conforming to requirements of the RPC service registered on the given PV channel\n\n:Returns: PV response object\n\nThe following code works with the above RPC service example:\n\n::\n\n    pvRequest = PvObject({'nRows' : INT, 'nColumns' : INT})\n\n    pvRequest.set({'nRows' : 10, 'nColumns' : 10})\n\n    pvResponse = rpcClient(pvRequest)\n\n    ntTable = NtTable(pvRequest)\n\n")
        ;
    
    // RPC Server
    class_<RpcServer>("RpcServer", "RpcServer is class used for hosting PVA RPC services. One instance of RpcServer can host multiple RPC services.\n\n**RpcServer()**:\n\n\t::\n\n\t\trpcServer = RpcServer()\n\n", init<>())
        .def("registerService", &RpcServer::registerService, args("serviceName", "serviceImpl"), "Registers service implementation with RPC server. Typically, all services are registered before RPC server starts listening for client requests.\n\n:Parameter: *serviceName* (str) - service name (name of the PV channel used for RPC client/server communication)\n\n:Parameter: *serviceImpl* (object) - reference to service implementation object (e.g., python function) that returns PV Object upon invocation\n\nThe following is an example of RPC service that creates NT Table according to client specifications:\n\n::\n\n    import pvaccess\n\n    import random\n\n    def createNtTable(pvRequest):\n\n        nRows = x.getInt('nRows')\n\n        nColumns = x.getInt('nColumns')\n\n        print 'Creating table with %d rows and %d columns' % (nRows, nColumns)\n\n        ntTable = pvaccess.NtTable(nColumns, pvaccess.DOUBLE)\n\n        labels = []\n\n        for j in range (0, nColumns):\n\n            labels.append('Column%s' % j)\n\n            column = []\n\n            for i in range (0, nRows):\n\n                column.append(random.uniform(0,1))\n\n            ntTable.setColumn(j, column)\n\n        ntTable.setLabels(labels)\n\n        ntTable.setDescriptor('Automatically created by pvaPy RPC Server')\n\n        return ntTable\n\n    \n\n    rpcServer = pvaccess.RpcServer()\n\n    rpcServer.registerService('createNtTable', createNtTable)\n\n    rpcServer.listen()\n\n")
        .def("unregisterService", &RpcServer::unregisterService, args("serviceName"), "Unregisters given service from RPC server.\n\n:Parameter: *serviceName* (str) - service name (name of the PV channel used for RPC client/server communication)\n\n::\n\n    rpcServer.unregisterService('createNtTable')\n\n")
        .def("startListener", &RpcServer::startListener, "Starts RPC listener in its own thread. This method is typically used for multi-threaded programs, or for testing and debugging in python interactive mode. It should be used in conjunction with *stopListener()* call.\n\n::\n\n    rpcServer.startListener()")
        .def("stopListener", &RpcServer::stopListener, "Stops RPC listener thread. This method is used in conjunction with *startListener()* call.\n\n::\n\n    rpcServer.stopListener()\n\n")
        .def("start", &RpcServer::start, "Start serving RPC requests. This method is equivalent to *listen()*, and blocks until either *stop()* or *shutdown()* methods are invoked.\n\n::\n\n    rpcServer.start()")
        .def("stop", &RpcServer::stop, "Stop serving RPC requests. This method is equivalent to *shutdown()*.\n\n::\n\n    rpcServer.stop()")
        .def("listen", static_cast<void(RpcServer::*)(int)>(&RpcServer::listen), RpcServerListen(args("seconds=0"), "Start serving RPC requests.\n\n:Parameter: *seconds* (int) - specifies the amount of time server should be listening for requests (0 indicates 'forever')\n\n::\n\n    rpcServer.listen(60)\n\n"))
        .def("shutdown", &RpcServer::shutdown, "Stop serving RPC requests. This method is equivalent to *stop()*.\n\n::\n\n    rpcServer.shutdown()")
        ;
    
}
