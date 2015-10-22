#!/usr/bin/env python

from __future__ import print_function

from pvaccess import *

ntTable = NtTable(3, DOUBLE)
ntTable.setLabels(['Col1', 'Col2', 'Col3'])
ntTable.setColumn(0, [0.1, 1.1, 2.2])
ntTable.setColumn(1, [1.1, 2.2, 3.3])
ntTable.setColumn(2, [2.1, 3.3, 4.4])

print("First NT Table:")
print(ntTable)

ntTable = NtTable([STRING, INT, DOUBLE])
ntTable.setLabels(['String', 'Int', 'Double'])
ntTable.setColumn(0, ['row0', 'row1', 'row2'])
ntTable.setColumn(1, [1, 2, 3])
ntTable.setColumn(2, [2.1, 3.3, 4.4])
ntTable.setDescriptor("Nice Table, Bad Results")

timeStamp = PvTimeStamp(12345678, 12)
ntTable.setTimeStamp(timeStamp)

alarm = PvAlarm(11, 126, "Server SegFault")
ntTable.setAlarm(alarm)

print()
print("Second NT Table:")
print(ntTable)

print()
print("Second NT Table: column1")
print(ntTable.getColumn(1))

print()
print("Second NT Table: timeStamp")
print(ntTable.getTimeStamp())

print()
print("Second NT Table: alarm")
print(ntTable.getAlarm())

print()
print("Second NT Table: column7 (expected error)")
try:
    print(ntTable.getColumn(7))
except Exception as ex:
    print(ex)
