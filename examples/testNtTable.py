#!/usr/bin/env python

from pvaccess import *

ntTable = NtTable(3, DOUBLE)
ntTable.setLabels(['Col1', 'Col2', 'Col3'])
ntTable.setColumn(0, [0.1, 1.1, 2.2])
ntTable.setColumn(1, [1.1, 2.2, 3.3])
ntTable.setColumn(2, [2.1, 3.3, 4.4])

print ntTable

ntTable = NtTable([STRING, INT, DOUBLE])
ntTable.setLabels(['String', 'Int', 'Double'])
ntTable.setColumn(0, ['row0', 'row1', 'row2'])
ntTable.setColumn(1, [1, 2, 3])
ntTable.setColumn(2, [2.1, 3.3, 4.4])

print ntTable
