#!/usr/bin/env python

import time
from pvaccess import PvObject
from pvaccess import RpcServer
from pvaccess import STRING

### pvAccess RPC server
def echo(x):    # x is an instance of PvObject
    print 'Got object: ', x  
    return x

srv = RpcServer()
srv.registerService('echo', echo)

# Listen in interactive mode for couple of minutes
print 'Starting server listener'
srv.startListener()
cnt = 30
while cnt > 0:
    print ('Remaining listener uptime: %d' % cnt)
    time.sleep(1)
    cnt = cnt - 1
print 'Stopping server listener'
srv.stopListener()

