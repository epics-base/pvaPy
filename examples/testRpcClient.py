#!/usr/bin/env python

import pvaccess
import random

rpc = pvaccess.RpcClient('createNtTable')
request = pvaccess.PvObject({'nRows' : pvaccess.INT, 'nColumns' : pvaccess.INT})
request.set({'nRows' : 10, 'nColumns' : 10})
print "Sending request for createNtTable:"
print request

print "Got response:"
response = rpc.invoke(request)
print response

print "Converting to NtTable:"
ntTable = pvaccess.NtTable(response)
print ntTable
print "Get column 3:"
print ntTable.getColumn(3)
print "Get descriptor:"
print ntTable.getDescriptor()

