#!/usr/bin/env python

from __future__ import print_function

import pvaccess
import random
import time

srv = pvaccess.RpcServer()
def createNtTable(x):
    nRows = x.getInt('nRows')
    nColumns = x.getInt('nColumns')
    print('Creating table with %d rows and %d columns' % (nRows, nColumns))
    ntTable = pvaccess.NtTable(nColumns, pvaccess.DOUBLE)
    labels = []
    for j in range (0, nColumns):
        labels.append('Column%s' % j)
        column = []
        for i in range (0, nRows):
            column.append(random.uniform(0,1))
        ntTable.setColumn(j, column)
    ntTable.setLabels(labels)
    ntTable.setDescriptor('Automatically created by RPC Server')
    return ntTable

srv.registerService('createNtTable', createNtTable)

# Listen in interactive mode for couple of minutes
print('Starting server listener')
srv.startListener()
cnt = 120
while cnt > 0:
    print('Remaining listener uptime:', cnt)
    time.sleep(1)
    cnt = cnt - 1
print('Stopping server listener')
srv.stopListener()
print('Server listener done')




