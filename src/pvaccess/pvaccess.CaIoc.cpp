// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"

#include "CaIoc.h"

using namespace boost::python;
namespace bp = boost::python;

//
// CA IOC class
//
void wrapCaIoc() 
{

class_<CaIoc>("CaIoc", 
    "CaIoc is a class that wrapps CA IOC functionality. Channel data can be retrieved and manipulated using standard CA command line tools and APIs. Typical sequence of calls involves loading a database, registering device driver, loading records, initializing (starting) IOC, and manipulating records.\n\n"
    "**CaIoc()**:\n\n"
    "\t::\n\n"
    "\t\tcaIoc = CaIoc()\n\n", 
    init<>())

    .def("start",
        static_cast<void(CaIoc::*)()>(&CaIoc::start),
        "Starts (initializes) CA IOC. This method is equivalent to iocInit(), which does not throw any exceptions.\n\n"
        ":Raises: *InvalidState* - in case of any errors.\n\n"
        "::\n\n"
        "    caIoc.start()\n\n")

    .def("stop",
        static_cast<void(CaIoc::*)()>(&CaIoc::stop),
        "Stops (shuts down) CA IOC. This methid is equivalent to iocShutdown(), which does not throw any exceptions.\n\n"
        ":Raises: *InvalidState* - in case of any errors.\n\n"
        "::\n\n"
        "    caIoc.stop()\n\n")

    .def("loadDatabase",
        static_cast<void(CaIoc::*)(const std::string&, const std::string&, const std::string&)>(&CaIoc::loadDatabase),
        args("file", "path", "substitutions"),
        "Load database definition file. This method is equivalent to dbLoadDatabase(), which does not throw any exceptions.\n\n"
        ":Parameter: *file* (str) - DBD file.\n\n"
        ":Parameter: *path* (str) - DBD search path. If not provided, the system will use EPICS_DB_INCLUDE_PATH environment variable.\n\n"
        ":Parameter: *substitutions* (str) - Macro substitutions string.\n\n"
        ":Raises: *InvalidArgument* - in case of empty file name.\n\n"
        ":Raises: *InvalidState* - in case of any other errors.\n\n"
        "::\n\n"
        "    caIoc.loadDatabase('base.dbd', '', '')\n\n")

    .def("registerRecordDeviceDriver",
        static_cast<void(CaIoc::*)()>(&CaIoc::registerRecordDeviceDriver),
        "Register record device driver. This method must be called after databases are loaded, and is equivalent to pvapyRegisterRecordDeviceDriver(), which does not throw any exceptions.\n\n"
        ":Raises: *InvalidState* - in case of any errors.\n\n"
        "::\n\n"
        "    caIoc.registerRecordDeviceDriver()\n\n")

    .def("loadRecords",
        static_cast<void(CaIoc::*)(const std::string&, const std::string&)>(&CaIoc::loadRecords),
        args("file", "substitutions"),
        "Load DB records from given file and apply specified substitutions. This method is equivalent to dbLoadRecords(), which does not throw any exceptions.\n\n"
        ":Parameter: *file* (str) - DB records file.\n\n"
        ":Parameter: *substitutions* (str) - Macro substitutions string.\n\n"
        ":Raises: *InvalidArgument* - in case of empty file.\n\n"
        ":Raises: *InvalidState* - in case of attempting to load records after IOC was initialized, or any other errors.\n\n"
        "::\n\n"
        "    caIoc.loadRecords('calc.db', 'NAME=C1')\n\n")

    .def("getRecordNames",
        static_cast<bp::list(CaIoc::*)()>(&CaIoc::getRecordNames),
        "Get DB record names.\n\n"
        ":Returns: list of record names.\n\n"
        ":Raises: *InvalidState* - in case of attempting to retrieve record names before loading a database.\n\n"
        "::\n\n"
        "    recordNames = caIoc.getRecordNames()\n\n")

    .def("putField",
        static_cast<void(CaIoc::*)(const std::string&, const bp::object&)>(&CaIoc::putField),
        args("name", "value"),
        "Put field. This method is equivalent to dbpf(), which does not throw any exceptions.\n\n"
        ":Parameter: *name* (str) - Field name.\n\n"
        ":Parameter: *value* (object) - Field value as a python object.\n\n"
        ":Raises: *InvalidArgument* - in case of empty field name.\n\n"
        ":Raises: *ObjectNotFound* - in case of unknown field name.\n\n"
        ":Raises: *InvalidState* - in case of attempting to call this method before initializing IOC, out of memory, or any other errors.\n\n"
        "::\n\n"
        "    caIoc.putField('I1', 5)\n\n")

    .def("putField",
        static_cast<void(CaIoc::*)(const std::string&, const std::string&)>(&CaIoc::putField),
        args("name", "value"),
        "Put field. This method is equivalent to dbpf(), which does not throw any exceptions.\n\n"
        ":Parameter: *name* (str) - Field name.\n\n"
        ":Parameter: *value* (str) - Field value as a string.\n\n"
        ":Raises: *InvalidArgument* - in case of empty field name.\n\n"
        ":Raises: *ObjectNotFound* - in case of unknown field name.\n\n"
        ":Raises: *InvalidState* - in case of attempting to call this method before initializing IOC, out of memory, or any other errors.\n\n"
        "::\n\n"
        "    caIoc.putField('I1', '5')\n\n")

    .def("getField",
        static_cast<bp::object(CaIoc::*)(const std::string&)>(&CaIoc::getField),
        args("name"),
        "Get field.\n\n"
        ":Parameter: *name* (str) - Field name.\n\n"
        ":Returns: Field value, which may be a list if the number if elements in the record is greater than 1.\n\n"
        ":Raises: *InvalidArgument* - in case of empty field name.\n\n"
        ":Raises: *ObjectNotFound* - in case of unknown field name.\n\n"
        ":Raises: *InvalidState* - in case of attempting to call this method before initializing IOC, out of memory, or any other errors.\n\n"
        "::\n\n"
        "    value = caIoc.getField('I1')\n\n")

    .def("printRecord",
        static_cast<void(CaIoc::*)(const std::string&, int)>(&CaIoc::printRecord),
        args("name", "level"),
        "Print record. This method is equivalent to dbpr(), which does not throw any exceptions.\n\n"
        ":Parameter: *name* (str) - Record name.\n\n"
        ":Parameter: *level* (int) - Interest (detail) level.\n\n"
        ":Raises: *InvalidArgument* - in case of empty record name.\n\n"
        ":Raises: *ObjectNotFound* - in case of unknown record name.\n\n"
        ":Raises: *InvalidState* - in case of any other errors.\n\n"
        "::\n\n"
        "    caIoc.printRecord('I1', 1)\n\n")

    //
    // Wrappers for calls available in iocsh
    //
    
    .def("dbLoadDatabase",
        static_cast<int(CaIoc::*)(const std::string&, const std::string&, const std::string&)>(&CaIoc::dbLoadDatabase),
        args("file", "path", "substitutions"),
        "Load database definition file.\n\n"
        ":Parameter: *file* (str) - DBD file.\n\n"
        ":Parameter: *path* (str) - DBD search path. If not provided, the system will use EPICS_DB_INCLUDE_PATH environment variable.\n\n"
        ":Parameter: *substitutions* (str) - Macro substitutions string.\n\n"
        ":Returns: exit status, where 0 indicates success and all other values indicate failure.\n\n"
        "::\n\n"
        "    status = caIoc.dbLoadDatabase('base.dbd', '', '')\n\n")

    .def("dbLoadRecords",
        static_cast<int(CaIoc::*)(const std::string&, const std::string&)>(&CaIoc::dbLoadRecords),
        args("file", "substitutions"),
        "Load DB records from given file and apply specified substitutions.\n\n"
        ":Parameter: *file* (str) - DB records file.\n\n"
        ":Parameter: *substitutions* (str) - Macro substitutions string.\n\n"
        ":Returns: exit status, where 0 indicates success and all other values indicate failure.\n\n"
        "::\n\n"
        "    status = caIoc.dbLoadRecords('calc.db', 'NAME=C1')\n\n")

    .def("iocInit",
        static_cast<int(CaIoc::*)()>(&CaIoc::iocInit),
        "Starts (initializes) CA IOC.\n\n"
        ":Returns: exit status, where 0 indicates success and all other values indicate failure.\n\n"
        "::\n\n"
        "    status = caIoc.iocInit()\n\n")

    .def("iocShutdown",
        static_cast<int(CaIoc::*)()>(&CaIoc::iocShutdown),
        "Shuts down CA IOC.\n\n"
        ":Returns: exit status, where 0 indicates success and all other values indicate failure.\n\n"
        "::\n\n"
        "    status = caIoc.iocShutdown()\n\n")

    .def("pvapyRegisterRecordDeviceDriver",
        static_cast<int(CaIoc::*)()>(&CaIoc::pvapyRegisterRecordDeviceDriver),
        "Register record device driver. This method must be called after databases are loaded.\n\n"
        "::\n\n"
        "    caIoc.pvapyRegisterRecordDeviceDriver()\n\n")

    .def("dbl",
        static_cast<int(CaIoc::*)(const std::string&, const std::string&)>(&CaIoc::dbl),
        args("recordType", "fields"),
        "List DB record/field names. If empty strings are provided as argument values, the method will list all record names.\n\n"
        ":Parameter: *recordType* (str) - DB record type.\n\n"
        ":Parameter: *fields* (str) - Record fields.\n\n"
        ":Returns: exit status, where 0 indicates success and all other values indicate failure.\n\n"
        "::\n\n"
        "    status = caIoc.dbl('', '')\n\n")

    .def("dbpr",
        static_cast<int(CaIoc::*)(const std::string&, int)>(&CaIoc::dbpr),
        args("name", "level"),
        "Print record.\n\n"
        ":Parameter: *name* (str) - Record name.\n\n"
        ":Parameter: *level* (int) - Interest (detail) level.\n\n"
        ":Returns: exit status, where 0 indicates success and all other values indicate failure.\n\n"
        "::\n\n"
        "    status = caIoc.dbpr('I1', 1)\n\n")

    .def("dbgf",
        static_cast<int(CaIoc::*)(const std::string&)>(&CaIoc::dbgf),
        args("name"),
        "Print field.\n\n"
        ":Parameter: *name* (str) - Field name.\n\n"
        ":Returns: exit status, where 0 indicates success and all other values indicate failure.\n\n"
        "::\n\n"
        "    status = caIoc.dbgf('I1')\n\n")

    .def("dbpf",
        static_cast<int(CaIoc::*)(const std::string&, const std::string&)>(&CaIoc::dbpf),
        args("name", "value"),
        "Put field.\n\n"
        ":Parameter: *name* (str) - Field name.\n\n"
        ":Parameter: *value* (str) - Field value.\n\n"
        ":Returns: exit status, where 0 indicates success and all other values indicate failure.\n\n"
        "::\n\n"
        "    status = caIoc.dbpf('I1', '5')\n\n")

;
} // wrapCaIoc()

