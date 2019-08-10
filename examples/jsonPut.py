#!/usr/bin/env python

from pvaccess import Channel

name = raw_input("enter numeric scalar channel: ")
chan = Channel(name)
str1 = 'value=' +  '20'
str2 = '{"timeStamp":{"userTag":"30"}}'
args = [str1,str2]
chan.parsePut(args,"value,timeStamp",True) 
result = chan.get("");
val = result.toJSON(True)
print(val)
val = result.toJSON(False)
print(val)

name = raw_input("enter numeric scalar array channel: ")
chan = Channel(name)
str1 = 'value=' +'[1,2,3,4,5,6]'
args = [str1]
chan.parsePut(args,"value,timeStamp",True) 
result = chan.get("")
val = result.toJSON(True)
print(val)
val = result.toJSON(False)
print(val)

name = raw_input("enter PVRenum: ")
chan = Channel(name)
str1 = 'value=' +'one'
args = [str1]
chan.parsePut(args,"value",False) 
result = chan.get("value,alarm,timeStamp")
val = result.toJSON(True)
print(val)
val = result.toJSON(False)
print(val)

