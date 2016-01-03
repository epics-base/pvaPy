// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "NtTable.h"

using namespace boost::python;

// 
// NT Table class
// 
void wrapNtTable()
{

class_<NtTable, bases<NtType> >("NtTable", 
    "NtTable represents NT table structure.\n\n"
    "**NtTable(nColumns, scalarType)**\n\n"
    "\t:Parameter: *nColumns* (int) - number of table columns\n\n"
    "\t:Parameter: *scalarType* (PVTYPE) - scalar type (BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING)\n\n"
    "\tThis example creates NT Table with 3 columns of DOUBLE values:\n\n"
    "\t::\n\n"
    "\t\ttable1 = NtTable(3, DOUBLE)\n\n"
    "\t**NtTable(scalarTypeList)**\n\n"
    "\t:Parameter: *scalarTypeList* ([PVTYPE]) - list of column scalar types (BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING)\n\n"
    "\tThis example creates NT Table with STRING, INT and DOUBLE columns:\n\n"
    "\t::\n\n"
    "\t\ttable2 = NtTable([STRING, INT, DOUBLE])\n\n"
    "**NtTable(pvObject)**\n\n"
    "\t:Parameter: *pvObject* (PvObject) - PV object that has a structure containing required NT Table elements:\n\n"
    "\t- labels ([STRING]) - list of column labels\n\n"
    "\t- value (dict) - dictionary of column<index>:[PVTYPE] pairs, where <index> is an integer in range [0,N-1], with N being NT Table dimension\n\n"
    "\tThe following example creates NT Table with 3 DOUBLE columns:\n\n"
    "\t::\n\n"
    "\t\tpvObject = PvObject({'labels' : [STRING], 'value' : {'column0' : [DOUBLE], 'column1' : [DOUBLE], 'column2' : [DOUBLE]}})\n\n"
    "\t\tpvObject.setScalarArray('labels', ['x', 'y', 'z'])\n\n"
    "\t\tpvObject.setStructure('value', {'column0' : [0.1, 0.2, 0.3], 'column1' : [1.1, 1.2, 1.3], 'column2' : [2.1, 2.2, 2.3]})\n\n"
    "\t\ttable3 = NtTable(pvObject)", 
    init<int, PvType::ScalarType>())

    .def(init<const boost::python::list&>())

    .def(init<const PvObject&>())

    .def("getNColumns", 
        &NtTable::getNColumns, 
        "Retrieves number of columns.\n\n"
        ":Returns: number of table columns\n\n"
        "::\n\n"
        "    nColumns = table.getNColumns()\n\n")

    .def("getLabels", 
        &NtTable::getLabels, 
        "Retrieves list of column labels.\n\n"
        ":Returns: list of column labels\n\n"
        "::\n\n"
        "    labelList = table.getLabels()\n\n")

    .def("setLabels", 
        &NtTable::setLabels, 
        args("labelList"), 
        "Sets column labels.\n\n"
        ":Parameter: *labelList* ([str]) - list of strings containing column labels (the list length must match number of table columns)\n\n"
        "::\n\n"
        "    table.setLabels(['String', 'Int', 'Double'])\n\n")

    .def("getColumn", 
        &NtTable::getColumn, args("index"), 
        "Retrieves specified column.\n\n"
        ":Parameter: *index* (int) - column index (must be in range [0,N-1], where N is the number of table columns)\n\n"
        ":Returns: list of values stored in the specified table column\n\n"
        "::\n\n"
        "    valueList = table.getColumn(0)\n\n")

    .def("setColumn", 
        &NtTable::setColumn, 
        args("index", "valueList"), 
        "Sets column values.\n\n"
        ":Parameter: *index* (int) - column index\n\n"
        ":Parameter: *valueList* (list) - list of column values\n\n"
        "::\n\n"
        "    table.setColumn(0, ['x', 'y', 'z'])\n\n")

    .def("getDescriptor", 
        &NtTable::getDescriptor, 
        "Retrieves table descriptor.\n\n"
        ":Returns: table descriptor\n\n"
        "::\n\n"
        "    descriptor = table.getDescriptor()\n\n")

    .def("setDescriptor", 
        &NtTable::setDescriptor, 
        args("descriptor"), 
        "Sets table descriptor.\n\n"
        ":Parameter: *descriptor* (str) - table descriptor\n\n"
        "::\n\n"
        "    table.setDescriptor('myTable')\n\n")

    .def("getTimeStamp", 
        &NtTable::getTimeStamp, 
        "Retrieves table time stamp.\n\n"
        ":Returns: table time stamp object\n\n"
        "::\n\n"
        "    timeStamp = table.getTimeStamp()\n\n")

    .def("setTimeStamp", 
        &NtTable::setTimeStamp, 
        args("timeStamp"), 
        "Sets table time stamp.\n\n"
        ":Parameter: *timeStamp* (PvTimeStamp) - table time stamp object\n\n"
        "::\n\n"
        "    timeStamp = PvTimeStamp(1234567890, 10000, 1)\n\n    table.setTimeStamp(timeStamp)\n\n")

    .def("getAlarm", 
        &NtTable::getAlarm, 
        "Retrieves table alarm.\n\n"
        ":Returns: table alarm object\n\n"
        "::\n\n"
        "    alarm = table.getAlarm()\n\n")

    .def("setAlarm", 
        &NtTable::setAlarm, 
        args("alarm"), 
        "Sets table alarm.\n\n"
        ":Parameter: *alarm* (PvAlarm) - table alarm object\n\n"
        "::\n\n"
        "    alarm = PvAlarm(11, 126, 'Server SegFault')\n\n"
        "    table.setAlarm(alarm)\n\n")
;

} // wrapNtTable()

