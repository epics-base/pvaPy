//
// PV Union class
// 
class_<PvUnion, bases<PvObject> >("PvUnion",
    "PvUnion represents PV union type.\n\n"
    "\n",
    init<>())

    .def(init<boost::python::dict>(args("structureDict")))

    .def(init<PvObject>(args("pvObject")))

    .def(str(self))
;
