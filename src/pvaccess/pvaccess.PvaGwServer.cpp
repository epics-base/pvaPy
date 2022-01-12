// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include <boost/python/class.hpp>

#include "PvaGwServer.h"

using namespace boost::python;

//
// PVA GW Server class
//
void wrapPvaGwServer() 
{

class_<PvaGwServer, bases<PvaServer> >("PvaGwServer", 
    "PvaGwServer is a class that provides PVA gateway functionality, i.e. it replicates PVA or CA channel data that can be retrieved using standard PVA command line tools and APIs.\n\n"

    "**PvaGwServer()**:\n\n"
    "\t::\n\n"
    "\t\tpvaGwServer = PvaGwServer()  # Initializes server without any records\n\n",
    init<>())

    .def("addGwRecord",
        static_cast<void(PvaGwServer::*)(const std::string&,const std::string&,PvProvider::ProviderType)>(&PvaGwServer::addGwRecord),
        args("gwChannelName", "srcChannelName", "srcProviderType"),
        "Adds gateway PV record to the server database.\n\n"
        ":Parameter: *gwChannelName* (str) - gateway channel name\n\n"
        ":Parameter: *srcChannelName* (str) - source channel name\n\n"
        ":Parameter: *srcProviderType* (PROVIDERTYPE) - provider type, either PVA (PV Access) or CA (Channel Access)\n\n"
        ":Raises: *ObjectAlreadyExists* - when database already contains record associated with a given gateway channel name\n\n"
        ":Raises: *PvaException* - in case of any other errors\n\n"
        "::\n\n"
        "    pvaGwServer.addGwRecord('gwPair', 'pair', PVA)\n\n")

    .def("removeGwRecord",
        static_cast<void(PvaGwServer::*)(const std::string&)>(&PvaGwServer::removeGwRecord),
        args("gwChannelName"),
        "Removes gateway PV record from the server database.\n\n"
        ":Parameter: *gwChannelName* (str) - gateway channel name\n\n"
        ":Raises: *ObjectNotFound* - when database does not contain record associated with a given channel name\n\n"
        ":Raises: *PvaException* - in case of any other errors\n\n"
        "::\n\n"
        "    pvaGwServer.removeRecord('gwPair')\n\n")

;
} // wrapPvaGwServer()

