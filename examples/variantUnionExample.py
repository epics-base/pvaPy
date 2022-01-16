#!/usr/bin/env python

from __future__ import print_function

from pvaccess import *

print('****************************')
print('Testing variant union')
channelName = "exampleVariantUnion"
c = Channel(channelName)
pv = c.get()
print("original value",pv)
isVariant = pv.isUnionVariant()
if isVariant==False :
    print(channelName,"is not a variant union")
    exit
    
value = PvObject({ 'value' : STRING })
newval = "ccccccc"
value.setString(newval)
pv.setUnion(value)
print("putting",pv)
c.put(pv)
pv = c.get()
print("received",pv)
retval = pv.getUnion().getString();
if newval!=retval :
    print("newval",newval,"not equal retval",retval)
value = PvObject({ 'value' : [STRING] })
newval = ["aaaa","bbbbb"]
value.setScalarArray(newval)
pv.setUnion(value)
print("putting",pv)
c.put(pv)
pv = c.get()
print("received",pv)
retval = pv.getUnion().getScalarArray();
if newval!=retval :
    print("newval",newval,"not equal retval",retval)
