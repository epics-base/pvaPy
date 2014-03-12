#!/usr/bin/env python

import pvaccess
import random

rpc = pvaccess.RpcClient('createNtTable')
request = pvaccess.PvObject({'nRows' : pvaccess.INT, 'nColumns' : pvaccess.INT})
request.set({'nRows' : 10, 'nColumns' : 10})
print "Sending request for createNtTable:"
print request

print "Got NtTable:"
response = rpc.invoke(request)
print response

