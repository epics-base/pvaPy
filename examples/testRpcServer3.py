#!/usr/bin/env python

import pvaccess
import random
import time

srv = pvaccess.RpcServer()
def getObject(x):
    result = pvaccess.PvObject({'tag' : 't1
    name = x.getString('name')
    nCalls += 1
     
    return ntTable

srv.registerService('createNtTable', createNtTable)

# Listen in interactive mode for couple of minutes
print 'Starting server listener'
srv.startListener()
cnt = 120
while cnt > 0:
    print 'Remaining listener uptime:', cnt
    time.sleep(1)
    cnt = cnt - 1
print 'Stopping server listener'
srv.stopListener()
print 'Server listener done'




