//
// PV Long class
//
class_<PvLong, bases<PvScalar> >("PvLong", 
    "PvLong represents PV long type.\n\n"
    "**PvLong([value=0])**\n\n"
    "\t:Parameter: *value* (long) - long value\n\n"
    "\t::\n\n"
    "\t\tpv = PvLong(-100000L)\n\n", 
    init<>())

    .def(init<long long>())

    .def("get", 
        &PvLong::get, 
        "Retrieves long PV value.\n\n"
        ":Returns: long value\n\n"
        "::\n\n"
        "    value = pv.get()\n\n")

    .def("set", 
        &PvLong::set, 
        "Sets long PV value.\n\n"
        ":Parameter: *value* (long) - long value\n\n"
        "::\n\n"
        "    pv.set(-100000L)\n\n")
;
