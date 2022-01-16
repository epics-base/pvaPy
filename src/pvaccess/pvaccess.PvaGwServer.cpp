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

    .def("removeAllGwRecords",
        static_cast<void(PvaGwServer::*)()>(&PvaGwServer::removeAllGwRecords),
        "Removes all gateway PV records from the server database.\n\n"
        ":Raises: *PvaException* - in case of any errors\n\n"
        "::\n\n"
        "    pvaGwServer.removeAllGwRecords()\n\n")

    .def("hasGwRecord",
        static_cast<bool(PvaGwServer::*)(const std::string&)>(&PvaGwServer::hasGwRecord),
        args("gwChannelName"),
        "Determines if server database contains gateway PV record associated with a given channel name.\n\n"
        ":Parameter: *gwChannelName* (str) - gateway channel name\n\n"
        ":Returns: True if record exists, false otherwise\n\n"
        "::\n\n"
        "    if pvaGwServer.hasGwRecord('pair'): print('Server contains gateway pair channel.)'\n\n")

    .def("getGwRecordNames",
        static_cast<boost::python::list(PvaGwServer::*)()>(&PvaGwServer::getGwRecordNames),
        "Retrieves existing gateway channel names from the server's database.\n\n"
        ":Returns: list of known gateway channel names\n\n"
        "::\n\n"
        "    gwRecordNames = pvaGwServer.getGwRecordNames()\n\n")

;
} // wrapPvaGwServer()

