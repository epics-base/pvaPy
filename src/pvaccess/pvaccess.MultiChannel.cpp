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
        static_cast<PvObject*(MultiChannel::*)()>(&MultiChannel::get), 
        return_value_policy<manage_new_object>(), 
        "Retrieves PV data from multiple channels'.\n\n"
        ":Returns: PvObject with NTMultiChannel structure that contains retrieved channel data as a variant union array\n\n"
        "::\n\n"
        "    pv = mChannel.get()\n\n")

;

#endif // if PVA_API_VERSION >= 481

} // wrapChannel()

