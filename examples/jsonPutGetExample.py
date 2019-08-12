#!/usr/bin/env python

from pvaccess import Channel

name = "PVRbyte"
print(str("starting " + name))
chan = Channel(name)
str1 = 'value=' +  '20'
str2 = '{"timeStamp":{"userTag":"30"}}'
args = [str1,str2]
result = chan.parsePutGet(args,"putField(value,timeStamp)getField()",True) 
print(result.toJSON(True))
print(result.toJSON(False))
print("")

name = "PVRbyteArray"
print(str("starting " + name))
chan = Channel(name)
str1 = 'value=' +'[1,2,3,4,5,6]'
args = [str1]
result = chan.parsePutGet(args,"putField(value)getField()",True) 
print(result.toJSON(True))
print(result.toJSON(False))
print("")

name = "PVRBigRecord"
print(str("starting " + name))
chan = Channel(name)
str1 = 'scalar.double.value=' +'10'
str2 = 'scalarArray.double.value=' +'[10,20,30]'
args = [str1,str2]
result = chan.parsePutGet(args,"putField(scalar.double,scalarArray.double)getField(scalar.double,scalarArray.double)",True) 
print(result.toJSON(True))
print(result.toJSON(False))

