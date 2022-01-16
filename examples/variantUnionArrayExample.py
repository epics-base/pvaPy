#!/usr/bin/env python

from __future__ import print_function

from pvaccess import *

print('****************************')
print('Testing variant union array')
c = Channel('exampleVariantUnionArray')
pv = c.get()
print(pv)
isVariant = pv.isUnionArrayVariant()
if isVariant==False :
    print(channelName,"is not a variant union array")
    exit
unionArray = []
value = PvObject({ 'value' : STRING })
value.setString("ccccccc")
unionArray.append(value)
value = PvObject({ 'value' : [STRING] })
value.setScalarArray(["xxxxxx","yyyyyy"])
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
