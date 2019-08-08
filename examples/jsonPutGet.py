#!/usr/bin/env python

from pvaccess import Channel

chan = Channel('PVRdouble')
str1 = 'value=' +  '20'
str2 = '{"timeStamp":{"userTag":"30"}}'
args = [str1,str2]
result = chan.parsePutGet(args,"putField(value,timeStamp)getField()",True) 
print(result)

chan = Channel('PVRdoubleArray')
str1 = 'value=' +'[1,2,3,4,5,6]'
args = [str1]
result = chan.parsePutGet(args,"putField(value,timeStamp)getField()",True) 
print(result)
