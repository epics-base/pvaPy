#!/usr/bin/env python

from pvaccess import Channel

name = "PVRbyte"
chan = Channel(name)
str1 = 'value=' +  '20'
args = [str1]
chan.parsePut(args,"value",True) 
result = chan.get("value");
print(result)

name = "PVRstring"
chan = Channel(name)
str1 = 'value=' +  '"this is a string"'
args = [str1]
chan.parsePut(args,"value",True) 
result = chan.get("value");
print(result)

name = "PVRenum"
chan = Channel(name)
str1 = 'value=' +'one'
args = [str1]
chan.parsePut(args,"value",False) 
result = chan.get("value,alarm,timeStamp")
print(result)

name = "PVRBigRecord"
chan = Channel(name)
str1 = 'scalar.double.value=' +'10'
str2 = 'scalarArray.double.value=' +'[10,20,30]'
args = [str1,str2]
chan.parsePut(args,"scalar.double,scalarArray.double",True) 
result = chan.get("scalar.double,scalarArray.double")
print(result)

name = "PVRBigRecord"
chan = Channel(name)
str1 = '{"scalar":{"double":{"value":20}}}'
str2 = '{"scalarArray":{"double":{"value":[1000,2000]}}}'
args = [str1,str2]
chan.parsePut(args,"scalar.double,scalarArray.double",True) 
result = chan.get("scalar.double,scalarArray.double")
print(result)

