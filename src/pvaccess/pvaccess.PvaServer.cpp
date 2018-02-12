// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "boost/python/overloads.hpp"

#include "PvaServer.h"

using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvaServerAddRecord, PvaServer::addRecord, 2, 3)

//
// PVA Server class
//
void wrapPvaServer() 
{

class_<PvaServer>("PvaServer", 
    "PvaServer is a class that wrapps PVA server functionality. Channel data can be retrieved and manipulated using standard PVA command line tools and APIs.\n\n"

    "**PvaServer()**:\n\n"
    "\t::\n\n"
    "\t\tpvaServer = PvaServer()  # Initializes server without any records\n\n" 
    "**PvaServer(channelName, pvObject)**:\n\n"
    "\t:Parameter: *channelName* (str) - channel name\n\n"
    "\t:Parameter: *pvObject* (PvObject) - PV object that will be exposed on the specified channel. Any changes to object's field values will be reflected on the channel.\n\n"
    "\t:Raises: *PvaException* - in case of any errors\n\n"
    "\t::\n\n"
    "\t\tpv = PvObject({'x': INT, 'y' : INT})\n\n" 
    "\t\tpvaServer = PvaServer('pair', pv)  # Initializes server with first record\n\n" 
    "\t\tpv['x'] = 3  # The 'pair' channel will now have field 'x' value set to 3\n\n" 
    "**PvaServer(channelName, pvObject, onWriteCallback)**:\n\n"
    "\t:Parameter: *channelName* (str) - channel name\n\n"
    "\t:Parameter: *pvObject* (PvObject) - PV object that will be exposed on the specified channel. Any changes to object's field values will be reflected on the channel.\n\n"
    "\t:Parameter: *onWriteCallback* (object) - reference to python object (e.g., python function) that will be executed on channel write.\n\n"
    "\t:Raises: *PvaException* - in case of any errors\n\n"
    "\t::\n\n"
    "\t\t# Server will call echo() any time clients write new value to the pair channel\n\n"
    "\t\tpv = PvObject({'x': INT, 'y' : INT})\n\n" 
    "\t\tdef echo(x):\n\n"
    "\t\t    print('New PV value was written: %s' % x)\n\n"
    "\t\tpvaServer = PvaServer('pair', pv, echo)\n\n",
    init<>())

    .def(init<std::string, const PvObject&>(args("channelName", "pvObject")))

    .def(init<std::string, const PvObject&, const boost::python::object&>(args("channelName", "pvObject", "onWriteCallback")))

    .def("update",
        static_cast<void(PvaServer::*)(const PvObject&)>(&PvaServer::update),
        args("pvObject"),
        "Updates server's PV object. This method is atomic, but can be used only when there is a single record in the master database.\n\n"
        ":Parameter: *pvObject* (PvObject) - PV object with a structure equivalent to the structure of the object registered on the server's PV channel.\n\n"
        ":Raises: *InvalidRequest* - when there is none or more than one record in the database\n\n"
        "::\n\n"
        "    pv2 = PvObject({'x' : INT, 'y' : INT}, {'x' : 3, 'y' : 5})\n\n"
        "    pvaServer.update(pv2)\n\n")

    .def("update",
        static_cast<void(PvaServer::*)(const std::string&, const PvObject&)>(&PvaServer::update),
        args("channelName", "pvObject"),
        "Updates server's PV object on a given channel. This method is atomic, and should be used when there are multiple records in the master database.\n\n"
        ":Parameter: *pvObject* (PvObject) - PV object with a structure equivalent to the structure of the object registered on the server's PV channel.\n\n"
        ":Raises: *ObjectNotFound* - when there is no record associated with a given channel\n\n"
        "::\n\n"
        "    pv = PvObject({'x' : INT, 'y' : INT}, {'x' : 3, 'y' : 5})\n\n"
        "    pvaServer.update('myChannel', pv)\n\n")

    .def("addRecord",
        static_cast<void(PvaServer::*)(const std::string&,const PvObject&,const boost::python::object&)>(&PvaServer::addRecord),
        PvaServerAddRecord(args("channelName","pvObject","onWriteCallback=None"),
        "Adds PV record to the server database.\n\n"
        ":Parameter: *channelName* (str) - channel name\n\n"
        ":Parameter: *pvObject* (PvObject) - PV object that will be exposed on the specified channel. Any changes to object's field values will be reflected on the channel.\n\n"
        ":Parameter: *onWriteCallback* (object) - reference to python object (e.g., python function) that will be executed on channel write.\n\n"
        ":Raises: *ObjectAlreadyExists* - when database already contains record associated with a given channel name\n\n"
        ":Raises: *PvaException* - in case of any other errors\n\n"
        "::\n\n"
        "    pv = PvObject({'x' : INT, 'y' : INT}, {'x' : 3, 'y' : 5})\n\n"
        "    def echo(x):\n\n"
        "        print('New PV value was written: %s' % x)\n\n"
        "    pvaServer.addRecord('pair', pv, echo)\n\n"))

    .def("removeRecord",
        static_cast<void(PvaServer::*)(const std::string&)>(&PvaServer::removeRecord),
        args("channelName"),
        "Removes PV record from the server database.\n\n"
        ":Parameter: *channelName* (str) - channel name\n\n"
        ":Raises: *ObjectNotFound* - when database does not contain record associated with a given channel name\n\n"
        ":Raises: *PvaException* - in case of any other errors\n\n"
        "::\n\n"
        "    pvaServer.removeRecord('pair')\n\n")

    .def("hasRecord",
        static_cast<bool(PvaServer::*)(const std::string&)>(&PvaServer::hasRecord),
        args("channelName"),
        "Determines if server database contains PV record associated with a given channel name.\n\n"
        ":Parameter: *channelName* (str) - channel name\n\n"
        ":Returns: True if record exists, false otherwise\n\n"
        "::\n\n"
        "    if pvaServer.hasRecord('pair'): print('Server contains the pair channel.)'\n\n")

    .def("getRecordNames",
        static_cast<boost::python::list(PvaServer::*)()>(&PvaServer::getRecordNames),
        "Retrieves existing channel names from the server's database.\n\n"
        ":Returns: list of known channel names\n\n"
        "::\n\n"
        "    recordNames = pvaServer.getRecordNames()\n\n")
;
} // wrapPvaServer()

