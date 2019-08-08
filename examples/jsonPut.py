#!/usr/bin/env python

from pvaccess import Channel

chan = Channel('PVRdouble')
str1 = 'value=' +  '20'
str2 = '{"timeStamp":{"userTag":"30"}}'
args = [str1,str2]
chan.parsePut(args,"value,timeStamp",True) 
result = chan.get()
print(result)
result = chan.getJSON()
print(result)
result = chan.getJSON("value,timeStamp",True)
print(result)

chan = Channel('PVRdoubleArray')
str1 = 'value=' +'[1,2,3,4,5,6]'
args = [str1]
chan.parsePut(args,"value,timeStamp",True) 
result = chan.get()
print(result)
result = chan.getJSON()
print(result)
result = chan.getJSON("value,timeStamp",True)
print(result)
