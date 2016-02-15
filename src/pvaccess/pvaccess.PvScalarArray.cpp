// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvScalarArray.h"

using namespace boost::python;

//
// PV Scalar Array class
//
void wrapPvScalarArray()
{

class_<PvScalarArray, bases<PvObject> >("PvScalarArray", 
    "PvScalarArray represents PV scalar array.\n\n"
    "**PvScalarArray(scalarType)**\n\n"
    "\t:Parameter: *scalarType* (PVTYPE) - scalar type of array elements\n\n"
    "\t- PVTYPE: scalar type, can be BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING\n\n"
    "\t::\n\n"
    "\t\tpv = PvScalarArray(INT)\n\n", 
    init<PvType::ScalarType>())

    .def("get", 
        &PvScalarArray::get, 
        "Retrieves PV value list.\n\n"
        ":Returns: list of scalar values\n\n"
        "::\n\n"
        "    valueList = pv.get()\n\n")

    .def("set", 
        &PvScalarArray::set, 
        args("valueList"), 
        "Sets PV value list.\n\n"
        ":Parameter: *valueList* (list) - list of scalar values\n\n"
        "::\n\n"
        "    pv.set([1,2,3,4,5])\n\n")

    .def("toList", 
        &PvScalarArray::toList, 
        "Converts PV to value list.\n\n"
        ":Returns: list of scalar values\n\n"
        "::\n\n"
        "    valueList = pv.toList()\n\n")
;

} // wrapPvScalarArray()

