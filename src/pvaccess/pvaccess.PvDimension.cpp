// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "PvDimension.h"

using namespace boost::python;

//
// PV Dimension class
//
void wrapPvDimension() 
{

class_<PvDimension, bases<PvObject> >("PvDimension", 
    "PvDimension represents dimension structure used by NtNdArrays.\n\n"
    "**PvDimension()**\n\n"
    "\t::\n\n"
    "\t\tdim1 = PvDimension()\n\n"
    "**PvDimension(size, offset, fullSize, binning, reverse)**\n\n"
    "\t:Parameter: *size* (int) - number of elements in this dimension\n\n"
    "\t:Parameter: *offset* (int) - offset value relative to the origin of the original data source\n\n"
    "\t:Parameter: *fullSize* (int) - number of elements in this dimension of the original data source\n\n"
    "\t:Parameter: *binning* (int) - binning value (pixel summation, 1=no binning) in this dimension relative to the original data source\n\n"
    "\t:Parameter: *reverse* (bool) - orientation flag relative to the original data source (false=normal, true=reversed)\n\n"
    "\t::\n\n"
    "\t\tdim2 = PvDimension(1024, 0, 1024, 0, 0))\n\n", 
    init<>())

    .def(init<int, int, int, int, bool>())

    .def("getSize", 
        &PvDimension::getSize, 
        "Retrieves dimension size.\n\n"
        ":Returns: dimension size\n\n"
        "::\n\n"
        "    size = dim.getSize()\n\n")

    .def("setSize", 
        &PvDimension::setSize, 
        args("size"), 
        "Sets dimension size.\n\n"
        ":Parameter: *size* (int) - size value\n\n"
        "::\n\n"
        "    dim.setSize(1024)\n\n")

    .def("getOffset", 
        &PvDimension::getOffset, 
        "Retrieves dimension offset.\n\n"
        ":Returns: dimension offset\n\n"
        "::\n\n"
        "    offset = dim.getOffset()\n\n")

    .def("setOffset", 
        &PvDimension::setOffset, 
        args("offset"), 
        "Sets dimension offset.\n\n"
        ":Parameter: *offset* (int) - offset value\n\n"
        "::\n\n"
        "    dim.setOffset(100)\n\n")

    .def("getFullSize", 
        &PvDimension::getFullSize, 
        "Retrieves dimension full size.\n\n"
        ":Returns: dimension full size\n\n"
        "::\n\n"
        "    fullSize = dim.getFullSize()\n\n")

    .def("setFullSize", 
        &PvDimension::setFullSize, 
        args("fullSize"), 
        "Sets dimension full size.\n\n"
        ":Parameter: *fullSize* (int) - full size value\n\n"
        "::\n\n"
        "    dim.setFullSize(1024)\n\n")

    .def("getBinning", 
        &PvDimension::getBinning, 
        "Retrieves dimension binning.\n\n"
        ":Returns: dimension binning\n\n"
        "::\n\n"
        "    binning = dim.getBinning()\n\n")

    .def("setBinning", 
        &PvDimension::setBinning, 
        args("binning"), 
        "Sets dimension binning.\n\n"
        ":Parameter: *binning* (int) - binning value\n\n"
        "::\n\n"
        "    dim.setBinning(100)\n\n")

    .def("getReverse", 
        &PvDimension::getReverse, 
        "Retrieves dimension reverse.\n\n"
        ":Returns: dimension reverse\n\n"
        "::\n\n"
        "    reverse = dim.getReverse()\n\n")

    .def("setReverse", 
        &PvDimension::setReverse, 
        args("reverse"), 
        "Sets dimension reverse.\n\n"
        ":Parameter: *reverse* (bool) - reverse value\n\n"
        "::\n\n"
        "    dim.setReverse(1)\n\n")

;

} // wrapPvDimension()


