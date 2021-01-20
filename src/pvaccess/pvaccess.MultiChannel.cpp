#include "boost/python/class.hpp"
#include "boost/python/overloads.hpp"
#include "boost/python/manage_new_object.hpp"
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

    .def("put",
        static_cast<void(MultiChannel::*)(const boost::python::list&)>(&MultiChannel::put),
        args("pvObjectList"),
        "Assigns data to multi channel member PVs'.\n\n"
        ":Parameter: *pvObjectList* (list) - list of PvObject instance that will be assigned to the multi channel PVs\n\n"
        "::\n\n"
        "    mChannel = MultiChannel(['PVRstringArray', 'PVRdouble'])\n\n"
        "    pv1 = PvObject({'value' : [STRING]}, {'value' : ['ccc', 'ddd']})\n\n"
        "    pv2 = PvDouble(44.44)\n\n"
        "    mChannel.put([pv1,pv2])\n\n")

;

#endif // if PVA_API_VERSION >= 481

} // wrapChannel()

