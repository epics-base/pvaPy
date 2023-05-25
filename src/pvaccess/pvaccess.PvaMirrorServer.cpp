// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include <boost/python/class.hpp>

#include "PvaMirrorServer.h"

using namespace boost::python;

//
// PVA Mirror Server class
//
void wrapPvaMirrorServer() 
{

class_<PvaMirrorServer, bases<PvaServer> >("PvaMirrorServer", 
    "PvaMirrorServer is a class that provides PVA mirror (read-only) functionality, i.e. it replicates PVA or CA channel data that can be retrieved using standard PVA command line tools and APIs.\n\n"

    "**PvaMirrorServer()**:\n\n"
    "\t::\n\n"
    "\t\tpvaMirrorServer = PvaMirrorServer()  # Initializes server without any records\n\n",
    init<>())

    .def("addMirrorRecord",
        static_cast<void(PvaMirrorServer::*)(const std::string&,const std::string&,PvProvider::ProviderType)>(&PvaMirrorServer::addMirrorRecord),
        args("mirrorChannelName", "srcChannelName", "srcProviderType"),
        "Adds mirror PV record to the server database.\n\n"
        ":Parameter: *mirrorChannelName* (str) - mirror channel name\n\n"
        ":Parameter: *srcChannelName* (str) - source channel name\n\n"
        ":Parameter: *srcProviderType* (PROVIDERTYPE) - provider type, either PVA (PV Access) or CA (Channel Access)\n\n"
        ":Raises: *ObjectAlreadyExists* - when database already contains record associated with a given mirror channel name\n\n"
        ":Raises: *PvaException* - in case of any other errors\n\n"
        "::\n\n"
        "    pvaMirrorServer.addMirrorRecord('mirrorPair', 'pair', PVA)\n\n")

    .def("addMirrorRecord",
        static_cast<void(PvaMirrorServer::*)(const std::string&,const std::string&,PvProvider::ProviderType, unsigned int)>(&PvaMirrorServer::addMirrorRecord),
        args("mirrorChannelName", "srcChannelName", "srcProviderType", "srcQueueSize"),
        "Adds mirror PV record to the server database. This method allows users to set server queue size for the source channel.\n\n"
        ":Parameter: *mirrorChannelName* (str) - mirror channel name\n\n"
        ":Parameter: *srcChannelName* (str) - source channel name\n\n"
        ":Parameter: *srcProviderType* (PROVIDERTYPE) - provider type, either PVA (PV Access) or CA (Channel Access)\n\n"
        ":Parameter: *srcQueueSize* (int) - source queue size (should be >= 0)\n\n"
        ":Raises: *ObjectAlreadyExists* - when database already contains record associated with a given mirror channel name\n\n"
        ":Raises: *PvaException* - in case of any other errors\n\n"
        "::\n\n"
        "    pvaMirrorServer.addMirrorRecord('mirrorPair', 'pair', PVA, 10)\n\n")

    .def("addMirrorRecord",
        static_cast<void(PvaMirrorServer::*)(const std::string&,const std::string&,PvProvider::ProviderType, unsigned int, unsigned int, const std::string&)>(&PvaMirrorServer::addMirrorRecord),
        args("mirrorChannelName", "srcChannelName", "srcProviderType", "srcQueueSize, nSrcMonitors, srcFieldRequestDescriptor"),
        "Adds mirror PV record to the server database. This method allows users to set server queue size and to use multiple monitors for the source channel, typically in combination with the data distributor plugin.\n\n"
        ":Parameter: *mirrorChannelName* (str) - mirror channel name\n\n"
        ":Parameter: *srcChannelName* (str) - source channel name\n\n"
        ":Parameter: *srcProviderType* (PROVIDERTYPE) - provider type, either PVA (PV Access) or CA (Channel Access)\n\n"
        ":Parameter: *srcQueueSize* (int) - source queue size (should be >= 0)\n\n"
        ":Parameter: *nSrcMonitors* (int) - number of listeners to use for the source channel; this option is typically used with the data distributor plugin, which requires appropriate setting for the field request descriptor\n\n"
        ":Parameter: *srcFieldRequestDescriptor* (str) - field descriptor for the source channel name, typically used for turning on the data distributor plugin\n\n"
        ":Raises: *ObjectAlreadyExists* - when database already contains record associated with a given mirror channel name\n\n"
        ":Raises: *InvalidRequest* - in case of invalid parameter values\n\n"
        ":Raises: *PvaException* - in case of any other errors\n\n"
        "::\n\n"
        "    pvaMirrorServer.addMirrorRecord('mirrorPair', 'pair', PVA, 10, 2, '_[pydistributor=updates:1]')\n\n")

    .def("removeMirrorRecord",
        static_cast<void(PvaMirrorServer::*)(const std::string&)>(&PvaMirrorServer::removeMirrorRecord),
        args("mirrorChannelName"),
        "Removes mirror PV record from the server database.\n\n"
        ":Parameter: *mirrorChannelName* (str) - mirror channel name\n\n"
        ":Raises: *ObjectNotFound* - when database does not contain record associated with a given channel name\n\n"
        ":Raises: *PvaException* - in case of any other errors\n\n"
        "::\n\n"
        "    pvaMirrorServer.removeRecord('mirrorPair')\n\n")

    .def("removeAllMirrorRecords",
        static_cast<void(PvaMirrorServer::*)()>(&PvaMirrorServer::removeAllMirrorRecords),
        "Removes all mirror PV records from the server database.\n\n"
        ":Raises: *PvaException* - in case of any errors\n\n"
        "::\n\n"
        "    pvaMirrorServer.removeAllMirrorRecords()\n\n")

    .def("hasMirrorRecord",
        static_cast<bool(PvaMirrorServer::*)(const std::string&)>(&PvaMirrorServer::hasMirrorRecord),
        args("mirrorChannelName"),
        "Determines if server database contains mirror PV record associated with a given channel name.\n\n"
        ":Parameter: *mirrorChannelName* (str) - mirror channel name\n\n"
        ":Returns: True if record exists, false otherwise\n\n"
        "::\n\n"
        "    if pvaMirrorServer.hasMirrorRecord('pair'): print('Server contains mirror pair channel.)'\n\n")

    .def("resetMirrorRecordCounters",
        static_cast<void(PvaMirrorServer::*)(const std::string&)>(&PvaMirrorServer::resetMirrorRecordCounters),
        "Reset all record counters to zero.\n\n"
        ":Parameter: *mirrorChannelName* (str) - mirror channel name\n\n"
        "::\n\n"
        "    pvaMirrorServer.resetMirrorRecordCounters('pair')\n\n")

    .def("getMirrorRecordCounters",
        static_cast<dict(PvaMirrorServer::*)(const std::string&)>(&PvaMirrorServer::getMirrorRecordCounters),
        "Retrieve dictionary with record counters, which include number of updates received and number of monitor overruns.\n\n"
        ":Parameter: *mirrorChannelName* (str) - mirror channel name\n\n"
        ":Returns: dictionary containing available statistics counters\n\n"
        "::\n\n"
        "    counterDict = pvaMirrorServer.getMirrorRecordCounters('pair')\n\n")

    .def("getMirrorRecordNames",
        static_cast<boost::python::list(PvaMirrorServer::*)()>(&PvaMirrorServer::getMirrorRecordNames),
        "Retrieves existing mirror channel names from the server's database.\n\n"
        ":Returns: list of known mirror channel names\n\n"
        "::\n\n"
        "    mirrorRecordNames = pvaMirrorServer.getMirrorRecordNames()\n\n")

;
} // wrapPvaMirrorServer()

