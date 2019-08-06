#!/usr/bin/env python

from pvaccess import Channel

chan = Channel('PVRdouble')
str1 = 'value=' +  '20'
str2 = '{"timeStamp":{"userTag":"30"}}'
args = [str1,str2]
chan.parsePut(args,"value,timeStamp") 

result = chan.get("value,timeStamp")
print(result)
