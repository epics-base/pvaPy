#!/usr/bin/env python

from __future__ import print_function

from pvaccess import Channel

print('****************************')
print('Testing union')
c = Channel('exampleRegularUnion')
pv = c.get()
print("original value",pv)
isVariant = pv.isUnionVariant()
if isVariant==True :
    print(channelName,"is not a regular union")
    exit
print("union introspection: ")
print(pv.getStructureDict())

names = pv.getUnionFieldNames()
print("union fieldNames",names)
value = pv.selectUnionField(names[0])
value.setString("new value")
value = pv.getUnion()
print("putting",pv)
c.put(pv)
pv = c.get()
print("received",pv)
print("value:",pv.get())
value = pv.selectUnionField(names[1])
print('selected field: ', pv.getSelectedUnionFieldName())
value.setScalarArray(["aaaa","bbbbb","ccccc"])
print("putting",pv)
c.put(pv)
pv = c.get()
print("received",pv)
print("value:",pv.get())
print("class:",pv.__class__.__name__)
print('selected field: ', pv.getSelectedUnionFieldName())

# This should fail
value.setInt(6)
print("putting",pv)
c.put(pv)
pv = c.get()
print("received",pv)
