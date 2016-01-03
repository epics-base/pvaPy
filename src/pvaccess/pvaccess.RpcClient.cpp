// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "boost/python/manage_new_object.hpp"
#include "RpcClient.h"

using namespace boost::python;

// 
// RPC Client class
// 
void wrapRpcClient()
{

class_<RpcClient>("RpcClient", 
    "RpcClient is a client class for PVA RPC services.\n\n"
    "**RpcClient(channelName)**\n\n"
    "\t:Parameter: *channelName* (str) - RPC service channel name\n\n"
    "\tThis example creates RPC client for channel 'createNtTable':\n\n"
    "\t::\n\n"
    "\t\trpcClient = RpcClient('createNtTable')\n\n", 
    init<std::string>())

    .def("invoke", 
        &RpcClient::invoke, 
        return_value_policy<manage_new_object>(), 
        args("pvRequest"), 
        "Invokes RPC call against service registered on the PV specified channel.\n\n"
        ":Parameter: *pvRequest* (PvObject) - PV request object with a structure conforming to requirements of the RPC service registered on the given PV channel\n\n"
        ":Returns: PV response object\n\n"
        "The following code works with the above RPC service example:\n\n"
        "::\n\n"
        "    pvRequest = PvObject({'nRows' : INT, 'nColumns' : INT})\n\n"
        "    pvRequest.set({'nRows' : 10, 'nColumns' : 10})\n\n"
        "    pvResponse = rpcClient(pvRequest)\n\n"
        "    ntTable = NtTable(pvRequest)\n\n")
;

} // wrapRpcClient()

