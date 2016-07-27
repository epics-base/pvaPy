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
#if defined PVA_RPC_API_VERSION && PVA_RPC_API_VERSION == 460
    "**RpcClient(channelName[, pvRequest])**\n\n"
    "\t:Parameter: *channelName* (str) - RPC service channel name\n\n"
    "\t:Parameter: *pvRequest* (PvObject) - pvRequest object specifying structure sent on creating RPC if default structure not used\n\n"
#else
    "**RpcClient(channelName)**\n\n"
    "\t:Parameter: *channelName* (str) - RPC service channel name\n\n"
#endif
    "\tThis example creates RPC client for channel 'createNtTable':\n\n"
    "\t::\n\n"
    "\t\trpcClient = RpcClient('createNtTable')\n\n", 
    init<std::string>())
#if defined PVA_RPC_API_VERSION && PVA_RPC_API_VERSION == 460
    .def(init<std::string, const PvObject&>())
#endif
    .def("invoke", 
        &RpcClient::invoke, 
        return_value_policy<manage_new_object>(), 
        args("pvArgument"), 
        "Invokes RPC call against service registered on the PV specified channel.\n\n"
        ":Parameter: *pvArgument* (PvObject) - PV argument object with a structure conforming to requirements of the RPC service registered on the given PV channel\n\n"
        ":Returns: PV response object\n\n"
        "The following code works with the above RPC service example:\n\n"
        "::\n\n"
        "    pvArgument = PvObject({'nRows' : INT, 'nColumns' : INT})\n\n"
        "    pvArgument.set({'nRows' : 10, 'nColumns' : 10})\n\n"
        "    pvResponse = rpcClient(pvArgument)\n\n"
        "    ntTable = NtTable(pvResponse)\n\n")
;

} // wrapRpcClient()

