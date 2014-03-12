#!/usr/bin/env python

import pvaccess
import random

srv = pvaccess.RpcServer()
def createNtTable(x):
    nRows = x.getInt('nRows')
    nColumns = x.getInt('nColumns')
    print 'Creating table with %d rows and %d columns' % (nRows, nColumns)
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
srv.listen()

