#!/usr/bin/env python

from pvaccess import *

ntTable = NtTable(3, DOUBLE)
ntTable.setLabels(['Col1', 'Col2', 'Col3'])
ntTable.set(0, [0.1, 1.1, 2.2])
ntTable.set(1, [1.1, 2.2, 3.3])
ntTable.set(2, [2.1, 3.3, 4.4])

print ntTable
