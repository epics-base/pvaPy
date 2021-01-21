#include <boost/python/class.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/manage_new_object.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "MultiChannel.h"

using namespace boost::python;

//
// MultiChannel class
//
void wrapMultiChannel()
{

#if PVA_API_VERSION >= 481


class_<MultiChannel>("MultiChannel", 
    "This class is used to communicate with multiple PV channels.\n\n"
    "**MultiChannel(names [, providerType=PVA])**\n\n"
    "\t:Parameter: *names* (list) - channel names\n\n"
    "\t:Parameter: *providerType* (PROVIDERTYPE) - provider type, either PVA (PV Access) or CA (Channel Access)\n\n"
    "\tThe following example allows access to PVA channels 'int01' and 'double01':\n\n"
    "\t::\n\n"
    "\t\tmChannel = MultiChannel(['int01','double01'])\n\n",
    init<const boost::python::list&>())

    .def(init<const boost::python::list&, PvProvider::ProviderType>())

    //
    // Get methods
    //

    .def("get",
        static_cast<PvObject*(MultiChannel::*)(const std::string&)>(&MultiChannel::get),
        return_value_policy<manage_new_object>(),
        args("requestDescriptor"),
        "Retrieves PV data from multiple channels.\n\n"
        ":Parameter: *requestDescriptor* (str) - PV request descriptor\n\n"
        ":Returns: PvObject with NTMultiChannel structure that contains retrieved data from all member channels as a variant union array\n\n"
        "::\n\n"
        "    pv = mChannel.get('field(value,alarm)')\n\n")

    .def("get", 
        static_cast<PvObject*(MultiChannel::*)()>(&MultiChannel::get), 
        return_value_policy<manage_new_object>(), 
        "Retrieves PV data from multiple channels using the default request descriptor 'field(value,alarm,timeStamp)'.\n\n"
        ":Returns: PvObject with NTMultiChannel structure that contains retrieved data from all member channels as a variant union array\n\n"
        "::\n\n"
        "    pv = mChannel.get()\n\n")

    .def("getAsDoubleArray", 
        static_cast<boost::python::list(MultiChannel::*)()>(&MultiChannel::getAsDoubleArray), 
        "Retrieves PV data from multiple channels as array of doubles.\n\n"
        ":Returns: list of floats\n\n"
        "::\n\n"
        "    valueList = mChannel.getAsDoubleArray()\n\n")

    .def("put",
        static_cast<void(MultiChannel::*)(const boost::python::list&)>(&MultiChannel::put),
        args("pvObjectList"),
        "Assigns data to multi-channel member PVs'.\n\n"
        ":Parameter: *pvObjectList* (list) - list of PvObject instances that will be assigned to the multi-channel PVs\n\n"
        "::\n\n"
        "    mChannel = MultiChannel(['PVRstringArray', 'PVRdouble'])\n\n"
        "    pv1 = PvObject({'value' : [STRING]}, {'value' : ['ccc', 'ddd']})\n\n"
        "    pv2 = PvDouble(44.44)\n\n"
        "    mChannel.put([pv1,pv2])\n\n")

    .def("putAsDoubleArray",
        static_cast<void(MultiChannel::*)(const boost::python::list&)>(&MultiChannel::putAsDoubleArray),
        args("valueList"),
        "Assigns data to multi-channel member PVs'.\n\n"
        ":Parameter: *valueList* (list) - list of float values that will be assigned to the multi-channel PVs\n\n"
        "::\n\n"
        "    mChannel = MultiChannel(['PVRdouble01', 'PVRdouble02'])\n\n"
        "    mChannel.put([1.0,2.0])\n\n")

    .def("monitor",
        static_cast<void(MultiChannel::*)(const boost::python::object&)>(&MultiChannel::monitor),
        args("subscriber"),
        "Starts multi-channel monitor with default poll period and request descriptor 'field(value,alarm,timeStamp)'.\n\n"
        ":Parameter: *subscriber* (object) - reference to python function that will be executed when PV value changes; the function should take PvObject instance as its argument\n\n"
        "::\n\n"
        "    def echo(pvObject):\n\n"
        "        print('New PV values: %s' % pvObject)\n\n"
        "    mChannel.monitor(echo)\n\n")

    .def("monitor",
        static_cast<void(MultiChannel::*)(const boost::python::object&, double)>(&MultiChannel::monitor),
        args("subscriber", "pollPeriod"),
        "Starts multi-channel monitor with default request descriptor 'field(value,alarm,timeStamp)'.\n\n"
        ":Parameter: *subscriber* (object) - reference to python function that will be executed when PV value changes; the function should take PvObject instance as its argument\n\n"
        ":Parameter: *pollPeriod* (float) - period in seconds between two multi-channel polls\n\n"
        "::\n\n"
        "    def echo(pvObject):\n\n"
        "        print('New PV values: %s' % pvObject)\n\n"
        "    mChannel.monitor(echo, 1.0)\n\n")

    .def("monitor",
        static_cast<void(MultiChannel::*)(const boost::python::object&, double, const std::string&)>(&MultiChannel::monitor),
        args("subscriber", "pollPeriod", "requestDescriptor"),
        "Starts multi-channel monitor.\n\n"
        ":Parameter: *subscriber* (object) - reference to python function that will be executed when PV value changes; the function should take PvObject instance as its argument\n\n"
        ":Parameter: *pollPeriod* (float) - period in seconds between two multi-channel polls\n\n"
        ":Parameter: *requestDescriptor* (str) - describes what PV data should be sent to subscribed channel clients\n\n"
        "::\n\n"
        "    def echo(pvObject):\n\n"
        "        print('New PV values: %s' % pvObject)\n\n"
        "    mChannel.monitor(echo, 1.0, 'field(value,alarm,timeStamp)')\n\n")

    .def("monitorAsDoubleArray",
        static_cast<void(MultiChannel::*)(const boost::python::object&)>(&MultiChannel::monitorAsDoubleArray),
        args("subscriber"),
        "Starts multi-channel monitor for processing list of double values.\n\n"
        ":Parameter: *subscriber* (object) - reference to python function that will be executed when PV values change; the function should take list of python floats as its argument\n\n"
        "::\n\n"
        "    def echo(valueList):\n\n"
        "        print('New PV values: %s' % x)\n\n"
        "    mChannel.monitorAsDoubleArray(echo, 1.0)\n\n")

    .def("monitorAsDoubleArray",
        static_cast<void(MultiChannel::*)(const boost::python::object&, double)>(&MultiChannel::monitorAsDoubleArray),
        args("subscriber", "pollPeriod"),
        "Starts multi-channel monitor for processing list of double values.\n\n"
        ":Parameter: *subscriber* (object) - reference to python function that will be executed when PV values change; the function should take list of python floats as its argument\n\n"
        ":Parameter: *pollPeriod* (float) - period in seconds between two multi-channel polls\n\n"
        "::\n\n"
        "    def echo(valueList):\n\n"
        "        print('New PV values: %s' % x)\n\n"
        "    mChannel.monitorAsDoubleArray(echo, 1.0)\n\n")

    .def("stopMonitor",
        &MultiChannel::stopMonitor,
        "Stops multi-channel monitor for PV value changes.\n\n"
        "::\n\n"
        "    mChannel.stopMonitor()\n\n")

;

#endif // if PVA_API_VERSION >= 481

} // wrapChannel()

