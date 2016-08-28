#include "boost/python/class.hpp"
#include "boost/python/manage_new_object.hpp"
#include "Channel.h"

using namespace boost::python;


//
// Channel class
//
void wrapChannel()
{

class_<Channel>("Channel", 
    "This class represents PV channels.\n\n"
    "**Channel(name [, providerType=PVA])**\n\n"
    "\t:Parameter: *fieldName* (str) - channel name\n\n"
    "\t:Parameter: *providerType* (PROVIDERTYPE) - provider type, either PVA (PV Access) or CA (Channel Access)\n\n"
    "\tNote that PV structures representing objects on CA channels always have a single key 'value'.\n"
    "\tThe following example creates PVA channel 'enum01':\n\n"
    "\t::\n\n"
    "\t\tpvaChannel = Channel('enum01')\n\n"
    "\tThis example allows access to CA channel 'CA:INT':\n\n"
    "\t::\n\n"
    "\t\tcaChannel = Channel('CA:INT', CA)\n\n", 
    init<std::string>())

    .def(init<std::string, PvProvider::ProviderType>())

    //
    // Get methods
    //

    .def("get", 
        static_cast<PvObject*(Channel::*)(const std::string&)>(&Channel::get), 
        return_value_policy<manage_new_object>(), 
        args("requestDescriptor"), 
        "Retrieves PV data from the channel.\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data corresponding to the specified request descriptor\n\n"
        "::\n\n"
        "    channel = Channel('enum01')\n\n"
        "    pv = channel.get('field(value.index)')\n\n")

    .def("get", 
        static_cast<PvObject*(Channel::*)()>(&Channel::get), 
        return_value_policy<manage_new_object>(), 
        "Retrieves PV data from the channel using the default request descriptor 'field(value)'.\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    pv = channel.get()\n\n")

    //
    // Put methods
    //

    .def("put", 
        static_cast<void(Channel::*)(const PvObject&, const std::string&)>(&Channel::put), 
        args("pvObject", "requestDescriptor"), 
        "Assigns PV data to the channel process variable.\n\n"
        ":Parameter: *pvObject* (PvObject) - PV object that will be assigned to channel PV according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        "::\n\n    channel = Channel('enum01')\n\n"
        "    channel.put(PvInt(1), 'field(value.index)')\n\n")

    .def("put", 
        static_cast<void(Channel::*)(const PvObject&)>(&Channel::put), 
        args("pvObject"), 
        "Assigns PV data to the channel process variable using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *pvObject* (PvObject) - PV object that will be assigned to the channel process variable\n\n"
        "::\n\n"
        "    channel = Channel('exampleInt')\n\n"
        "    channel.put(PvInt(1))\n\n")

    .def("putScalarArray", 
        static_cast<void(Channel::*)(const boost::python::list&, const std::string&)>(&Channel::put), 
        args("valueList", "requestDescriptor"), 
        "Assigns scalar array data to the channel PV according to the specified request descriptor.\n\n"
        ":Parameter: *valueList* (list) - list of scalar values that will be assigned to the channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("put", 
        static_cast<void(Channel::*)(const boost::python::list&, const std::string&)>(&Channel::put), 
        args("valueList", "requestDescriptor"), 
        "Assigns scalar array data to the channel PV according to the specified request descriptor.\n\n"
        ":Parameter: *valueList* (list) - list of scalar values that will be assigned to the channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("putScalarArray", 
        static_cast<void(Channel::*)(const boost::python::list&)>(&Channel::put), 
        args("valueList"), 
        "Assigns scalar array data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *valueList* (list) - list of scalar values that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('intArray01')\n\n"
        "    channel.putScalarArray([0,1,2,3,4])\n\n")

    .def("put", 
        static_cast<void(Channel::*)(const boost::python::list&)>(&Channel::put), 
        args("valueList"), 
        "Assigns scalar array data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *valueList* (list) - list of scalar values that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('intArray01')\n\n"
        "    channel.put([0,1,2,3,4])\n\n")

     //
     // For scalar types, overloading put() method
     // for types that are not distinguished by python will not do anything,
     // except being used for documentation 
     //
     // However, order in which overloaded methods are defined is
     // important (method defined later will be used): 
     // For example, put(double) must be defined after put(float), put(string)
     // must be defined after put(unsigned char), etc.
     //
     
    .def("putFloat", 
        static_cast<void(Channel::*)(float, const std::string&)>(&Channel::put),
        args("value", "requestDescriptor"), 
        "Assigns float data to the channel PV.\n\n"
        ":Parameter: *value* (float) - float value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("put", 
        static_cast<void(Channel::*)(float, const std::string&)>(&Channel::put),
        args("value", "requestDescriptor"), 
        "Assigns float data to the channel PV.\n\n"
        ":Parameter: *value* (float) - float value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("putFloat", 
        static_cast<void(Channel::*)(float)>(&Channel::put), 
        args("value"), 
        "Assigns float data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (float) - float value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('exampleFloat')\n\n"
        "    channel.putFloat(1.1)\n\n")

    .def("put", 
        static_cast<void(Channel::*)(float)>(&Channel::put), 
        args("value"), 
        "Assigns float data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (float) - float value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('exampleFloat')\n\n"
        "    channel.put(1.1)\n\n")

     .def("putDouble", 
         static_cast<void(Channel::*)(double, const std::string&)>(&Channel::put), 
         args("value", "requestDescriptor"), 
         "Assigns double data to the channel PV.\n\n"
         ":Parameter: *value* (float) - double value that will be assigned to channel data according to the specified request descriptor\n\n"
         ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("put", 
        static_cast<void(Channel::*)(double, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns double data to the channel PV.\n\n"
        ":Parameter: *value* (float) - double value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("putDouble", 
        static_cast<void(Channel::*)(double)>(&Channel::put), 
        args("value"), 
        "Puts double data into the channel using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (float) - double value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('double01')\n\n"
        "    channel.putDouble(1.1)\n\n")

    .def("put", 
        static_cast<void(Channel::*)(double)>(&Channel::put), 
        args("value"), 
        "Puts double data into the channel using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (float) - double value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('double01')\n\n"
        "    channel.put(1.1)\n\n")

    .def("putBoolean", 
        static_cast<void(Channel::*)(bool, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns boolean data to the channel PV.\n\n"
        ":Parameter: *value* (bool) - boolean value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("put", 
        static_cast<void(Channel::*)(bool, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns boolean data to the channel PV.\n\n"
        ":Parameter: *value* (bool) - boolean value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("putBoolean", 
        static_cast<void(Channel::*)(bool)>(&Channel::put), 
        args("value"), 
        "Assigns boolean data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (bool) - boolean value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('boolean01')\n\n"
        "    channel.putBoolean(True)\n\n")

    .def("put", 
        static_cast<void(Channel::*)(bool)>(&Channel::put), 
        args("value"), 
        "Assigns boolean data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (bool) - boolean value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('boolean01')\n\n"
        "    channel.put(True)\n\n")

    .def("putUByte", 
        static_cast<void(Channel::*)(unsigned char, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns unsigned byte data to the channel PV.\n\n"
        ":Parameter: *value* (int) - unsigned byte value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("put", 
        static_cast<void(Channel::*)(unsigned char, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns unsigned byte data to the channel PV.\n\n"
        ":Parameter: *value* (int) - unsigned byte value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("putUByte", 
        static_cast<void(Channel::*)(unsigned char)>(&Channel::put), 
        args("value"), 
        "Assigns unsigned byte data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (int) - unsigned byte value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('ubyte01')\n\n"
        "    channel.putUByte(10)\n\n")

    .def("put", 
        static_cast<void(Channel::*)(unsigned char)>(&Channel::put), 
        args("value"), 
        "Assigns unsigned byte data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (int) - unsigned byte value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('ubyte01')\n\n"
        "    channel.put(10)\n\n")

    .def("putByte", 
        static_cast<void(Channel::*)(char, const std::string&)>(&Channel::put),
        args("value", "requestDescriptor"), 
        "Assigns byte data to the channel PV.\n\n"
        ":Parameter: *value* (int) - byte value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")
       
    .def("put", 
        static_cast<void(Channel::*)(char, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns byte data to the channel PV.\n\n"
        ":Parameter: *value* (int) - byte value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("putByte", 
        static_cast<void(Channel::*)(char)>(&Channel::put), 
        args("value"), 
        "Assigns byte data to the channel using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (int) - byte value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('byte01')\n\n"
        "    channel.putByte(-10)\n\n")
        
    .def("put", 
        static_cast<void(Channel::*)(char)>(&Channel::put), 
        args("value"), "Assigns byte data to the channel using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (int) - byte value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('byte01')\n\n"
        "    channel.put(-10)\n\n")

    .def("putUShort", 
        static_cast<void(Channel::*)(unsigned short, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns unsigned short data to the channel PV.\n\n"
        ":Parameter: *value* (int) - unsigned short value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("put", 
        static_cast<void(Channel::*)(unsigned short, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns unsigned short data to the channel PV.\n\n"
        ":Parameter: *value* (int) - unsigned short value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("putUShort", 
        static_cast<void(Channel::*)(unsigned short)>(&Channel::put), 
        args("value"), 
        "Assigns unsigned short data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (int) - unsigned short value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('ushort01')\n\n"
        "    channel.putUShort(10)\n\n")

    .def("put", 
        static_cast<void(Channel::*)(unsigned short)>(&Channel::put), 
        args("value"), 
        "Assigns unsigned short data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (int) - unsigned short value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('ushort01')\n\n"
        "    channel.put(10)\n\n")

    .def("putShort", 
        static_cast<void(Channel::*)(short, const std::string&)>(&Channel::put),
        args("value", "requestDescriptor"), 
        "Assigns short data to the channel PV.\n\n"
        ":Parameter: *value* (int) - short value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("put", 
        static_cast<void(Channel::*)(short, const std::string&)>(&Channel::put),
        args("value", "requestDescriptor"), "Assigns short data to the channel PV.\n\n"
        ":Parameter: *value* (int) - short value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("putShort", 
        static_cast<void(Channel::*)(short)>(&Channel::put), 
        args("value"), 
        "Assigns short data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (int) - short value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('short01')\n\n"
        "    channel.putShort(10)\n\n")

    .def("put", 
        static_cast<void(Channel::*)(short)>(&Channel::put), 
        args("value"), 
        "Assigns short data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (int) - short value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('short01')\n\n    channel.put(10)\n\n")

    .def("putUInt", 
        static_cast<void(Channel::*)(unsigned long int, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns unsigned integer data to the channel PV.\n\n"
        ":Parameter: *value* (int) - unsigned integer value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("put", 
        static_cast<void(Channel::*)(unsigned long int, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns unsigned integer data to the channel PV.\n\n"
        ":Parameter: *value* (int) - unsigned integer value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("putUInt", 
        static_cast<void(Channel::*)(unsigned long int)>(&Channel::put), 
        args("value"), 
        "Assigns unsigned integer data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (int) - unsigned integer value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('uexampleInt')\n\n"
        "    channel.putUInt(1000)\n\n")

    .def("put", 
        static_cast<void(Channel::*)(unsigned long int)>(&Channel::put), 
        args("value"), 
        "Assigns unsigned integer data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (int) - unsigned integer value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('uexampleInt')\n\n"
        "    channel.putUInt(1000)\n\n")

    .def("putInt", 
        static_cast<void(Channel::*)(long int, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns integer data to the channel PV.\n\n"
        ":Parameter: *value* (int) - integer value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("put", 
        static_cast<void(Channel::*)(long int, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns integer data to the channel PV.\n\n"
        ":Parameter: *value* (int) - integer value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("putInt", 
        static_cast<void(Channel::*)(long int)>(&Channel::put), 
        args("value"), 
        "Assigns integer data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (int) - integer value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('exampleInt')\n\n"
        "    channel.putInt(1000)\n\n")

    .def("put", 
        static_cast<void(Channel::*)(long int)>(&Channel::put), 
        args("value"), 
        "Assigns integer data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (int) - integer value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('exampleInt')\n\n"
        "    channel.put(1000)\n\n")

    .def("putULong", 
        static_cast<void(Channel::*)(unsigned long long, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns unsigned long data to the channel PV.\n\n"
        ":Parameter: *value* (long) - unsigned long value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("put", 
        static_cast<void(Channel::*)(unsigned long long, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns unsigned long data to the channel PV.\n\n"
        ":Parameter: *value* (long) - unsigned long value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("putULong", 
        static_cast<void(Channel::*)(unsigned long long)>(&Channel::put), 
        args("value"), 
        "Assigns unsigned long data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (long) - unsigned long value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('ulong01')\n\n"
        "    channel.putULong(100000L)\n\n")

    .def("put", 
        static_cast<void(Channel::*)(unsigned long long)>(&Channel::put), 
        args("value"), 
        "Assigns unsigned long data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (long) - unsigned long value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('ulong01')\n\n"
        "    channel.put(100000L)\n\n")

    .def("putLong", 
        static_cast<void(Channel::*)(long long, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns long data to the channel PV.\n\n"
        ":Parameter: *value* (long) - long value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("put", 
        static_cast<void(Channel::*)(long long, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns long data to the channel PV.\n\n"
        ":Parameter: *value* (long) - long value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("putLong", 
        static_cast<void(Channel::*)(long long)>(&Channel::put), 
        args("value"), 
        "Assigns long data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (long) - long value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('long01')\n\n"
        "    channel.putLong(100000L)\n\n")

    .def("put", 
        static_cast<void(Channel::*)(long long)>(&Channel::put), 
        args("value"), 
        "Assigns long data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (long) - long value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('long01')\n\n"
        "    channel.put(100000L)\n\n")

    .def("putString", 
        static_cast<void(Channel::*)(const std::string&, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns string data to the channel PV.\n\n"
        ":Parameter: *value* (str) - string value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("put", 
        static_cast<void(Channel::*)(const std::string&, const std::string&)>(&Channel::put), 
        args("value", "requestDescriptor"), 
        "Assigns string data to the channel PV.\n\n"
        ":Parameter: *value* (str) - string value that will be assigned to channel data according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n")

    .def("putString", 
        static_cast<void(Channel::*)(const std::string&)>(&Channel::put), 
        args("value"), 
        "Assigns string data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (str) - string value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('string01')\n\n"
        "    channel.putString('string value')\n\n")

    .def("put", 
        static_cast<void(Channel::*)(const std::string&)>(&Channel::put), 
        args("value"), 
        "Assigns string data to the channel PV using the default request descriptor 'field(value)'.\n\n"
        ":Parameter: *value* (str) - string value that will be assigned to the channel PV\n\n"
        "::\n\n"
        "    channel = Channel('string01')\n\n"
        "    channel.put('string value')\n\n")


    //
    // PutGet methods
    //

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(const PvObject&, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("pvObject", "requestDescriptor"), 
        "Assigns PV data to the channel process variable and returns new PV value.\n\n"
        ":Parameter: *pvObject* (PvObject) - PV object that will be assigned to channel PV according to the specified request descriptor\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data corresponding to the specified request descriptor\n\n"
        "::\n\n"
        "    channel = Channel('exampleInt')\n\n"
        "    pv = channel.putGet(PvInt(1), 'putField(value)getField(value)')\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(const PvObject&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("pvObject"), 
        "Assigns PV data to the channel process variable and returns new PV value.\n\n"
        ":Parameter: *pvObject* (PvObject) - PV object that will be assigned to channel PV according to the default request descriptor 'putField(value)getField(value)'\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleInt')\n\n"
        "    pv = channel.putGet(PvInt(1))\n\n")

    .def("putGetScalarArray", 
        static_cast<PvObject*(Channel::*)(const boost::python::list&, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("valueList", "requestDescriptor"), 
        "Assigns scalar array data to the channel process variable and returns new PV value.\n\n"
        ":Parameter: *valueList* (list) - list of scalar values that will be assigned to the channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleIntArray')\n\n"
        "    pv = channel.putGetScalarArray([0,1,2,3,4], 'putField(value)getField(value)')\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(const boost::python::list&, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("valueList", "requestDescriptor"), 
        "Assigns scalar array data to the channel process variable and returns new PV value.\n\n"
        ":Parameter: *valueList* (list) - list of scalar values that will be assigned to the channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleIntArray')\n\n"
        "    pv = channel.putGet([0,1,2,3,4], 'putField(value)getField(value)')\n\n")

    .def("putGetScalarArray", 
        static_cast<PvObject*(Channel::*)(const boost::python::list&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("valueList"), 
        "Assigns scalar array data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *valueList* (list) - list of scalar values that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleIntArray')\n\n"
        "    pv = channel.putGetScalarArray([0,1,2,3,4], 'putField(value)getField(value)')\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(const boost::python::list&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("valueList"), 
        "Assigns scalar array data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *valueList* (list) - list of scalar values that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleIntArray')\n\n"
        "    pv = channel.putGet([0,1,2,3,4], 'putField(value)getField(value)')\n\n")

    .def("putGetFloat", 
        static_cast<PvObject*(Channel::*)(float, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns float data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (float) - float value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(float, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns float data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (float) - float value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGetFloat", 
        static_cast<PvObject*(Channel::*)(float)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns float data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (float) - float value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleFloat')\n\n"
        "    pv = channel.putGetFloat(-1.1)\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(float)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns float data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (float) - float value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleFloat')\n\n"
        "    pv = channel.putGet(-1.1)\n\n")

    .def("putGetDouble", 
        static_cast<PvObject*(Channel::*)(double, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns double data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (float) - double value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(double, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns double data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (float) - double value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGetDouble", 
        static_cast<PvObject*(Channel::*)(double)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns double data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (float) - double value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleDouble')\n\n"
        "    pv = channel.putGetDouble(-1.1)\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(double)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns double data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (float) - double value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleDouble')\n\n"
        "    pv = channel.putGet(-1.1)\n\n")

    .def("putGetBoolean", 
        static_cast<PvObject*(Channel::*)(bool, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns boolean data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (bool) - boolean value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(bool, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns boolean data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (bool) - boolean value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGetBoolean", 
        static_cast<PvObject*(Channel::*)(bool)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns boolean data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (bool) - boolean value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleBoolean')\n\n"
        "    pv = channel.putGetBoolean(True)\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(bool)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns boolean data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (bool) - boolean value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleBoolean')\n\n"
        "    pv = channel.putGet(True)\n\n")

    .def("putGetUByte", 
        static_cast<PvObject*(Channel::*)(unsigned char, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns unsigned byte data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - unsigned byte value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(unsigned char, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns unsigned byte data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - unsigned byte value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGetUByte", 
        static_cast<PvObject*(Channel::*)(unsigned char)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns unsigned byte data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - unsigned byte value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleUByte')\n\n"
        "    pv = channel.putGetUByte(-10)\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(unsigned char)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns unsigned byte data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - unsigned byte value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleUByte')\n\n"
        "    pv = channel.putGet(-10)\n\n")

    .def("putGetByte", 
        static_cast<PvObject*(Channel::*)(char, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns byte data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - byte value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(char, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns byte data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - byte value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGetByte", 
        static_cast<PvObject*(Channel::*)(char)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns byte data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - byte value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleByte')\n\n"
        "    pv = channel.putGetByte(-10)\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(char)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns byte data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - byte value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleByte')\n\n"
        "    pv = channel.putGet(-10)\n\n")

    .def("putGetUShort", 
        static_cast<PvObject*(Channel::*)(unsigned short, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns unsigned short data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (int) - unsigned short value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(unsigned short, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns unsigned short data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (int) - short value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGetUShort", 
        static_cast<PvObject*(Channel::*)(unsigned short)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns unsigned short data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - unsigned short value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleUShort')\n\n"
        "    pv = channel.putGetUShort(1000)\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(unsigned short)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns unsigned short data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - unsigned short value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleUShort')\n\n"
        "    pv = channel.putGet(1000)\n\n")

    .def("putGetShort", 
        static_cast<PvObject*(Channel::*)(short, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns short data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (int) - short value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(short, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns short data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (int) - short value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGetShort", 
        static_cast<PvObject*(Channel::*)(short)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns short data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - short value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleShort')\n\n"
        "    pv = channel.putGetShort(-1000)\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(short)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns short data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - short value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleShort')\n\n"
        "    pv = channel.putGet(-1000)\n\n")

    .def("putGetUInt", 
        static_cast<PvObject*(Channel::*)(unsigned long int, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns unsigned int data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (int) - unsigned int value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(unsigned long int, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns unsigned int data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (int) - int value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGetUInt", 
        static_cast<PvObject*(Channel::*)(unsigned long int)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns unsigned int data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - unsigned int value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleUInt')\n\n"
        "    pv = channel.putGetUInt(1000)\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(unsigned long int)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns unsigned int data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - unsigned int value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleUInt')\n\n"
        "    pv = channel.putGet(1000)\n\n")

    .def("putGetInt", 
        static_cast<PvObject*(Channel::*)(long int, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns int data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (int) - int value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(long int, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns int data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (int) - int value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGetInt", 
        static_cast<PvObject*(Channel::*)(long int)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns int data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - int value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleInt')\n\n"
        "    pv = channel.putGetInt(1000)\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(long int)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns int data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (int) - int value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleInt')\n\n"
        "    pv = channel.putGet(1000)\n\n")

    .def("putGetULong", 
        static_cast<PvObject*(Channel::*)(unsigned long long, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns unsigned long data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (long) - unsigned long value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(unsigned long long, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns unsigned long data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (long) - unsigned long value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGetULong", 
        static_cast<PvObject*(Channel::*)(unsigned long long)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns unsigned long data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (long) - unsigned long value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleULong')\n\n"
        "    pv = channel.putGetULong(1000L)\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(unsigned long long)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns unsigned long data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (long) - unsigned long value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleULong')\n\n"
        "    pv = channel.putGet(1000L)\n\n")

    .def("putGetLong", 
        static_cast<PvObject*(Channel::*)(long long, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns long data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (long) - long value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(long long, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns long data to the channel PV and returns new PV value.\n\n"
        ":Parameter: *value* (long) - long value that will be assigned to channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n")

    .def("putGetLong", 
        static_cast<PvObject*(Channel::*)(long long)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns long data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (long) - long value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleLong')\n\n"
        "    pv = channel.putGetLong(-1000L)\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(long long)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns long data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (long) - long value that will be assigned to the channel PV\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleLong')\n\n"
        "    pv = channel.putGet(-1000L)\n\n")

    .def("putGetString", 
        static_cast<PvObject*(Channel::*)(const std::string&, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns string data to the channel process variable and returns new PV value.\n\n"
        ":Parameter: *value* (str) - string value that will be assigned to the channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleString')\n\n"
        "    pv = channel.putGetString('string value', 'putField(value)getField(value)')\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(const std::string&, const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value", "requestDescriptor"), 
        "Assigns string data to the channel process variable and returns new PV value.\n\n"
        ":Parameter: *value* (str) - string value that will be assigned to the channel PV\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleString')\n\n"
        "    pv = channel.putGet('string value', 'putField(value)getField(value)')\n\n")

    .def("putGetString", 
        static_cast<PvObject*(Channel::*)(const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns string data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (str) - string value that will be assigned to the channel PV'\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleString')\n\n"
        "    pv = channel.putGetString('string value')\n\n")

    .def("putGet", 
        static_cast<PvObject*(Channel::*)(const std::string&)>(&Channel::putGet), 
        return_value_policy<manage_new_object>(),
        args("value"), 
        "Assigns string data to the channel PV using the default request descriptor 'putField(value)getField(value)', and returns new PV value.\n\n"
        ":Parameter: *value* (str) - string value that will be assigned to the channel PV'\n\n"
        ":Returns: channel PV data\n\n"
        "::\n\n"
        "    channel = Channel('exampleString')\n\n"
        "    pv = channel.putGet('string value')\n\n")

    //
    // GetPut methods
    //

    .def("getPut", 
        static_cast<PvObject*(Channel::*)(const std::string&)>(&Channel::getPut), 
        return_value_policy<manage_new_object>(), 
        args("requestDescriptor"), 
        "Retrieves put PV data from the channel.\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: channel put PV data corresponding to the specified request descriptor\n\n"
        "::\n\n"
        "    channel = Channel('enum01')\n\n"
        "    pv = channel.getPut('field(value.index)')\n\n")

    .def("getPut", 
        static_cast<PvObject*(Channel::*)()>(&Channel::get), 
        return_value_policy<manage_new_object>(), 
        "Retrieves put PV data from the channel using the default request descriptor 'field(value)'.\n\n"
        ":Returns: channel put PV data\n\n"
        "::\n\n"
        "    pv = channel.getPut()\n\n")

    //
    // Monitor methods
    //
    
    .def("subscribe", 
        &Channel::subscribe, 
        args("subscriberName", "subscriber"), 
        "Subscribes python object to notifications of changes in PV value. Channel can have any number of subscribers that start receiving PV updates after *startMonitor()* is invoked. Updates stop after channel monitor is stopped via *stopMonitor()* call, or object is unsubscribed from notifications using *unsubscribe()* call.\n\n"
        ":Parameter: *subscriberName* (str) - subscriber object name\n\n"
        ":Parameter: *subscriber* (object) - reference to python subscriber object (e.g., python function) that will be executed when PV value changes\n\n"
        "The following code snippet defines a simple subscriber object, subscribes it to PV value changes, and starts channel monitor:\n\n"
        "::\n\n"
        "    def echo(x):\n\n"
        "        print 'New PV value: ', x\n\n"
        "    channel = Channel('exampleFloat')\n\n"
        "    channel.subscribe('echo', echo)\n\n"
        "    channel.startMonitor()\n\n")

    .def("unsubscribe", 
        &Channel::unsubscribe, 
        args("subscriberName"), 
        "Unsubscribes subscriber object from notifications of changes in PV value.\n\n"
        ":Parameter: *subscriberName* (str) - subscriber name\n\n"
        "::\n\n"
        "    channel.unsubscribe('echo')\n\n")

    .def("startMonitor", 
        static_cast<void(Channel::*)(const std::string&)>(&Channel::startMonitor), args("requestDescriptor"), 
        "Starts channel monitor for PV value changes.\n\n"
        ":Parameter: *requestDescriptor* (str) - describes what PV data should be sent to subscribed channel clients\n\n"
        "::\n\n"
        "    channel.startMonitor('field(value.index)')\n\n")

    .def("startMonitor", 
        static_cast<void(Channel::*)()>(&Channel::startMonitor), 
        "Starts channel monitor for PV value changes using the default request descriptor 'field(value)'.\n\n"
        "::\n\n"
        "    channel.startMonitor()\n\n")

    .def("stopMonitor", 
        &Channel::stopMonitor, 
        "Stops channel monitor for PV value changes.\n\n"
        "::\n\n"
        "    channel.stopMonitor()\n\n")

    .def("getTimeout", 
        &Channel::getTimeout, 
        "Retrieves channel timeout.\n\n"
        ":Returns: channel timeout in seconds\n\n"
        "::\n\n"
        "    timeout = channel.getTimeout()\n\n")

    .def("setTimeout", 
        &Channel::setTimeout, 
        args("timeout"), 
        "Sets channel timeout.\n\n"
        ":Parameter: *timeout* (float) - channel timeout in seconds\n\n"
        "::\n\n"
        "    channel.setTimeout(10.0)\n\n")

    .def("getMonitorMaxQueueLength", 
        &Channel::getMonitorMaxQueueLength, 
        "Retrieves maximum monitor queue length.\n\n"
        ":Returns: maximum monitor queue length\n\n"
        "::\n\n"
        "    maxQueueLength = channel.getMonitorMaxQueueLength()\n\n")

    .def("setMonitorMaxQueueLength", 
        &Channel::setMonitorMaxQueueLength, 
        args("maxQueueLength"), 
        "Sets maximum monitor queue length. Negative number means unlimited length, while the value of zero disables monitor queue. When monitor queue is disabled, incoming data is processed immediately by all python subscribers (i.e., there is no processing thread running in the background). When monitoring queue is full, channel will not be polled for new data. Default monitor queue length is zero.\n\n"
        ":Parameter: *maxQueueLength* (int) - maximum queue length\n\n"
        "::\n\n    channel.setMonitorMaxQueueLengthTimeout(10)\n\n")

;

} // wrapChannel()

