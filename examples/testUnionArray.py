#!/usr/bin/env python

from __future__ import print_function

from pvaccess import *

print('****************************')
print('Testing union array')
c = Channel('exampleRegularUnionArray')
pv = c.get()
print("original value",pv)
isVariant = pv.isUnionArrayVariant()
if isVariant==True :
    print(channelName,"is not a regular union")
    exit
names = pv.getUnionArrayFieldNames()
print("union fieldNames",names)
unionArray = []
value = pv.createUnionArrayElementField(names[0])
value.setString("new value")
unionArray.append(value)
value = pv.createUnionArrayElementField(names[1])
value.setScalarArray(["xxxxxx","yyyyyyyy"])
unionArray.append(value)
pv.setUnionArray(unionArray)
print("putting",pv)
c.put(pv)
pv = c.get()
print("received",pv)
unionArray = pv.getUnionArray()
unionArray[0].setString("last value")
unionArray[1].setScalarArray(["aaa","bbb"])
pv.setUnionArray(unionArray)
print("putting",pv)
c.put(pv)
pv = c.get()
print("received",pv)
