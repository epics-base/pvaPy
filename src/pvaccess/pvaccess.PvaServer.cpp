// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "boost/python/overloads.hpp"

#include "PvaServer.h"

using namespace boost::python;

#ifndef WINDOWS
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvaServerAddRecord, PvaServer::addRecord, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvaServerAddRecordWithAs, PvaServer::addRecordWithAs, 4, 5)
#endif

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

#if PVA_API_VERSION >= 483

    .def("initAs",
        static_cast<void(PvaServer::*)(const std::string&, const std::string&)>(&PvaServer::initAs),
        args("filePath", "substitutions"),
        "Initialize access security.\n\n"
        ":Parameter: *filePath* (str) - AS definitions file.\n\n"
        ":Parameter: *substitutions* (str) - Macro substitutions.\n\n"
        ":Raises: *PvaException* - in case of any errors\n\n"
        "::\n\n"
        "    pvaServer.initAs('as.cnf', 'S=S27,P=BPM1')\n\n")

    .def("isAsActive",
        static_cast<bool(PvaServer::*)()>(&PvaServer::isAsActive),
        "Is access security active?\n\n"
        ":Returns: true if AS is active\n\n"
        "::\n\n"
        "    isActive = pvaServer.isAsActive()\n\n")

#endif // if PVA_API_VERSION >= 483

    .def("start",
        static_cast<void(PvaServer::*)()>(&PvaServer::start),
        "Starts PVA server. This method is called in all constructors automatically, but may be used to restart server if it has been stopped.\n\n"
        ":Raises: *PvaException* - in case of any errors\n\n"
        "::\n\n"
        "    pvaServer.start()\n\n")

    .def("stop",
        static_cast<void(PvaServer::*)()>(&PvaServer::stop),
        "Stops PVA server.\n\n"
        ":Raises: *PvaException* - in case of any errors\n\n"
        "::\n\n"
        "    pvaServer.stop()\n\n")

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

#ifndef WINDOWS
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
#else
    .def("addRecord",
        static_cast<void(PvaServer::*)(const std::string&,const PvObject&,const boost::python::object&)>(&PvaServer::addRecord),
        args("channelName","pvObject","onWriteCallback"),
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
        "    pvaServer.addRecord('pair', pv, echo)\n\n")
#endif

#if PVA_API_VERSION >= 483

#ifndef WINDOWS

    .def("addRecordWithAs",
        static_cast<void(PvaServer::*)(const std::string&,const PvObject&,int,const std::string&,const boost::python::object&)>(&PvaServer::addRecordWithAs),
        PvaServerAddRecordWithAs(args("channelName","pvObject","asLevel","asGroup","onWriteCallback=None"),
        "Adds PV record with access security to the server database.\n\n"
        ":Parameter: *channelName* (str) - channel name\n\n"
        ":Parameter: *pvObject* (PvObject) - PV object that will be exposed on the specified channel. Any changes to object's field values will be reflected on the channel.\n\n"
        ":Parameter: *asLevel* (int) - access security level\n\n"
        ":Parameter: *asGroup* (str) - access security group\n\n"
        ":Parameter: *onWriteCallback* (object) - reference to python object (e.g., python function) that will be executed on channel write.\n\n"
        ":Raises: *ObjectAlreadyExists* - when database already contains record associated with a given channel name\n\n"
        ":Raises: *PvaException* - in case of any other errors\n\n"
        "::\n\n"
        "    pv = PvObject({'x' : INT, 'y' : INT}, {'x' : 3, 'y' : 5})\n\n"
        "    def echo(x):\n\n"
        "        print('New PV value was written: %s' % x)\n\n"
        "    pvaServer.addRecordWithAs('pair', pv, 1, 'MyGroup', echo)\n\n"))
#else
    .def("addRecordWithAs",
        static_cast<void(PvaServer::*)(const std::string&,const PvObject&,int,const std::string&,const boost::python::object&)>(&PvaServer::addRecordWithAs),
        args("channelName","pvObject","asLevel","asGroup","onWriteCallback"),
        "Adds PV record with access security to the server database.\n\n"
        ":Parameter: *channelName* (str) - channel name\n\n"
        ":Parameter: *pvObject* (PvObject) - PV object that will be exposed on the specified channel. Any changes to object's field values will be reflected on the channel.\n\n"
        ":Parameter: *asLevel* (int) - access security level\n\n"
        ":Parameter: *asGroup* (str) - access security group\n\n"
        ":Parameter: *onWriteCallback* (object) - reference to python object (e.g., python function) that will be executed on channel write.\n\n"
        ":Raises: *ObjectAlreadyExists* - when database already contains record associated with a given channel name\n\n"
        ":Raises: *PvaException* - in case of any other errors\n\n"
        "::\n\n"
        "    pv = PvObject({'x' : INT, 'y' : INT}, {'x' : 3, 'y' : 5})\n\n"
        "    def echo(x):\n\n"
        "        print('New PV value was written: %s' % x)\n\n"
        "    pvaServer.addRecordWithAs('pair', pv, 1, 'MyGroup', echo)\n\n")

#endif // WINDOWS

#endif // if PVA_API_VERSION >= 483

    .def("removeRecord",
        static_cast<void(PvaServer::*)(const std::string&)>(&PvaServer::removeRecord),
        args("channelName"),
        "Removes PV record from the server database.\n\n"
        ":Parameter: *channelName* (str) - channel name\n\n"
        ":Raises: *ObjectNotFound* - when database does not contain record associated with a given channel name\n\n"
        ":Raises: *PvaException* - in case of any other errors\n\n"
        "::\n\n"
        "    pvaServer.removeRecord('pair')\n\n")

    .def("removeAllRecords",
        static_cast<void(PvaServer::*)()>(&PvaServer::removeAllRecords),
        "Removes all PV records from the server database.\n\n"
        ":Raises: *PvaException* - in case of any errors\n\n"
        "::\n\n"
        "    pvaServer.removeAllRecords()\n\n")

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

