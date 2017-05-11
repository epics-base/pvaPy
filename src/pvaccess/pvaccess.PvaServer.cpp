// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvaServer.h"

using namespace boost::python;

//
// PVA Server class
//
void wrapPvaServer() 
{

class_<PvaServer>("PvaServer", 
    "PvaServer is a class that wrapps PVA server functionality. Channel data can be retrieved and manipulated using standard PVA command line tools and APIs.\n\n"
    "**PvaServer(channelName, pvObject)**:\n\n"
    "\t:Parameter: *channelName* (str) - channel name\n\n"
    "\t:Parameter: *pvObject* (PvObject) - PV object that will be exposed on the specified channel. Any changes to object's field values will be reflected on the channel.\n\n"
    "\t::\n\n"
    "\t\tpv = PvObject({'x': INT, 'y' : INT})\n\n" 
    "\t\tpvaServer = PvaServer('pair', pv)\n\n" 
    "\t\tpv['x'] = 3  # The 'pair' channel will now have field 'x' value set to 3\n\n", 
    init<std::string, const PvObject&>())

    .def("update",
        static_cast<void(PvaServer::*)(const PvObject&)>(&PvaServer::update),
        args("pvObject"),
        "Updates server's PV object.\n\n"
        ":Parameter: *pvObject* (PvObject) - PV object with a structure equivalent to the structure of the object registered on the server's PV channel.\n\n"
        "The following code works with the above PVA server example:\n\n"
        "::\n\n"
        "    pv2 = PvObject({'x' : INT, 'y' : INT}, {'x' : 3, 'y' : 5})\n\n"
        "    pvaServer.update(pv2)\n\n")
;

} // wrapPvaServer()

