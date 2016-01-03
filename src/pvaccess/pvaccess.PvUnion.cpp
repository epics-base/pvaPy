#include "boost/python/class.hpp"
#include "boost/python/self.hpp"
#include "boost/python/operators.hpp"
#include "PvUnion.h"

using namespace boost::python;

//
// PV Union class
// 
void wrapPvUnion()
{

class_<PvUnion, bases<PvObject> >("PvUnion",
    "PvUnion represents PV union type.\n\n"
    "\n",
    init<>())

    .def(init<boost::python::dict>(args("structureDict")))

    .def(init<PvObject>(args("pvObject")))

    .def(str(self))
;

} // wrapPvUnion()

