// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "boost/python/overloads.hpp"
#include "RpcServer.h"

using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RpcServerListen, RpcServer::listen, 0, 1)

//
// RPC Server class
//
void wrapRpcServer() 
{

class_<RpcServer>("RpcServer", 
    "RpcServer is class used for hosting PVA RPC services. One instance of RpcServer can host multiple RPC services.\n\n"
    "**RpcServer()**:\n\n"
    "\t::\n\n"
    "\t\trpcServer = RpcServer()\n\n", 
    init<>())

    .def("registerService", 
        &RpcServer::registerService, 
        args("serviceName", "serviceImpl"), 
        "Registers service implementation with RPC server. Typically, all services are registered before RPC server starts listening for client requests.\n\n"
        ":Parameter: *serviceName* (str) - service name (name of the PV channel used for RPC client/server communication)\n\n"
        ":Parameter: *serviceImpl* (object) - reference to service implementation object (e.g., python function) that returns PV Object upon invocation\n\n"
        "The following is an example of RPC service that creates NT Table according to client specifications:\n\n"
        "::\n\n"
        "    import pvaccess\n\n"
        "    import random\n\n"
        "    def createNtTable(pvRequest):\n\n"
        "        nRows = x.getInt('nRows')\n\n"
        "        nColumns = x.getInt('nColumns')\n\n"
        "        print 'Creating table with %d rows and %d columns' % (nRows, nColumns)\n\n"
        "        ntTable = pvaccess.NtTable(nColumns, pvaccess.DOUBLE)\n\n"
        "        labels = []\n\n"
        "        for j in range (0, nColumns):\n\n"
        "            labels.append('Column%s' % j)\n\n"
        "            column = []\n\n"
        "            for i in range (0, nRows):\n\n"
        "                column.append(random.uniform(0,1))\n\n"
        "            ntTable.setColumn(j, column)\n\n"
        "        ntTable.setLabels(labels)\n\n"
        "        ntTable.setDescriptor('Automatically created by pvaPy RPC Server')\n\n"
        "        return ntTable\n\n"
        "    \n\n"
        "    rpcServer = pvaccess.RpcServer()\n\n"
        "    rpcServer.registerService('createNtTable', createNtTable)\n\n"
        "    rpcServer.listen()\n\n")

    .def("unregisterService", 
        &RpcServer::unregisterService, 
        args("serviceName"), 
        "Unregisters given service from RPC server.\n\n"
        ":Parameter: *serviceName* (str) - service name (name of the PV channel used for RPC client/server communication)\n\n"
        "::\n\n"
        "    rpcServer.unregisterService('createNtTable')\n\n")

    .def("startListener", 
        &RpcServer::startListener, 
        "Starts RPC listener in its own thread. This method is typically used for multi-threaded programs, or for testing and debugging in python interactive mode. It should be used in conjunction with *stopListener()* call.\n\n"
        "::\n\n"
        "    rpcServer.startListener()\n\n")

    .def("stopListener", 
        &RpcServer::stopListener, 
        "Stops RPC listener thread. This method is used in conjunction with *startListener()* call.\n\n"
        "::\n\n"
        "    rpcServer.stopListener()\n\n")

    .def("start", 
        &RpcServer::start, 
        "Start serving RPC requests. This method is equivalent to *listen()*, and blocks until either *stop()* or *shutdown()* methods are invoked.\n\n"
        "::\n\n"
        "    rpcServer.start()\n\n")

    .def("stop", 
        &RpcServer::stop, 
        "Stop serving RPC requests. This method is equivalent to *shutdown()*.\n\n"
        "::\n\n"
        "    rpcServer.stop()\n\n")

    .def("listen", 
        static_cast<void(RpcServer::*)(int)>(&RpcServer::listen), 
        RpcServerListen(args("seconds=0"), 
        "Start serving RPC requests.\n\n"
        ":Parameter: *seconds* (int) - specifies the amount of time server should be listening for requests (0 indicates 'forever')\n\n"
        "::\n\n"
        "    rpcServer.listen(60)\n\n"))

    .def("shutdown", 
        &RpcServer::shutdown, 
        "Stop serving RPC requests. This method is equivalent to *stop()*.\n\n"
        "::\n\n"
        "    rpcServer.shutdown()\n\n")
;

} // wrapRpcServer()

