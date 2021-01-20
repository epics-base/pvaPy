#!/usr/bin/env python

from __future__ import print_function

from pvaccess import *

print('for contrast first show non union')
alarm = PvAlarm()
print('alarm: ', alarm)

pv = PvObject({ 'value' : DOUBLE, 'alarm' : alarm })
print('pv',pv)
print('dictionary',pv.get())
print('introspection',pv.getStructureDict())


print('\ntest variant union')
union = PvUnion()
print('PvUnion()\n',union)
print('union introspection',union.getStructureDict())

pv = PvObject({ 'value' : union})

print('pv',pv)
print('dictionary',pv.get())
print('introspection',pv.getStructureDict())

isVariant = pv.isUnionVariant()
if isVariant==False :
    print(channelName,'is not a variant union')
    exit

value = PvObject({ 'value' : STRING })
newval = 'ccccccc'
value.setString(newval)
pv.setUnion(value)
print('after setString',pv)
print('dictionary',pv.get())
value = PvObject({ 'value' : [STRING] })
newval = ['aaaa','bbbbb']
value.setScalarArray(newval)
pv.setUnion(value)
print('after setScalarArray',pv)
print('dictionary',pv.get())

print('\ntest variant union constructed via tuple')
pv = PvObject({ 'value' : ()})

print('pv',pv)
print('dictionary',pv.get())
print('introspection',pv.getStructureDict())

isVariant = pv.isUnionVariant()
if isVariant==False :
    print(channelName,'is not a variant union')
    exit

value = PvObject({ 'value' : STRING })
newval = 'ccccccc'
value.setString(newval)
pv.setUnion(value)
print('after setString',pv)
print('dictionary',pv.get())
value = PvObject({ 'value' : [STRING] })
newval = ['aaaa','bbbbb']
value.setScalarArray(newval)
pv.setUnion(value)
print('after setScalarArray',pv)
print('dictionary',pv.get())

print('\n*************************')
print('\ntest regular union')
pvtemp = PvObject({ 'double' : DOUBLE, 'doubleArr' : [DOUBLE]})
print("pvtemp structure: ")
print(pvtemp.getStructureDict())

union = PvUnion(pvtemp)
print('Union(pvtemp)\n',union)
print('introspection\n',union.getStructureDict())
print('dictionary\n',union.toDict())

#pv = PvObject({ 'value' : union})
pv = union
print('pv',pv)
print('dictionary',pv.get())
print('introspection',pv.getStructureDict())
isVariant = pv.isUnionVariant()
if isVariant==True :
    print(channelName,'is a variant array')
    exit
names = pv.getUnionFieldNames()
print('union fieldNames',names)
value = pv.selectUnionField(names[0])
print('value: ', value)
value.setDouble(2.0)
print('after setDouble',pv)
print('dictionary',pv.get())
value = pv.selectUnionField(names[1])
value.setScalarArray([1.0,2.0,3.0,4.0])
print('after setScalarArray',pv)
print('dictionary',pv.get())

print('\n*************************')
print('\ntest variant union array')
union = PvUnion()
pv = PvObject({ 'value' : [union]})
print('pv\n',pv)
print('dictionary\n',pv.get())
print('introspection\n',pv.getStructureDict())
isVariant = pv.isUnionArrayVariant()
if isVariant==False :
    print(channelName,'is not a variant union')
    exit
values = []
value = PvObject({ 'value' : STRING })
newval = 'ccccccc'
value.setString(newval)
values.append(value)
value = PvObject({ 'value' : [STRING] })
newval = ['aaaa','bbbbb']
value.setScalarArray(newval)
values.append(value)
pv.setUnionArray(values)
print('after unionArraySet\n',pv)
print('structure\n',pv.getStructureDict())
print('dictionary\n',pv.toDict())

print('\n*************************')
print('\ntest regular union array')
pvtemp = PvObject({ 'double' : DOUBLE, 'doubleArr' : [DOUBLE]})
union = PvUnion(pvtemp)
print('union',union)
print('dictionary\n',union.get())
print('introspection\n',union.getStructureDict())

pv = PvObject({ 'value' : [union]})
print('pv\n',pv)
print('introspection\n',pv.getStructureDict())
print('dictionary\n',pv.get())
isVariant = pv.isUnionArrayVariant()
if isVariant==True :
    print(channelName,'is a variant array')
    exit
names = pv.getUnionArrayFieldNames()
print('unionArray fieldNames: ',names)
unionArray = []
# This also works
#value = PvObject({'double' : DOUBLE})
value = pv.createUnionArrayElementField('double')
print('created union array element field:')
print(value)
value.setDouble(10.0)
unionArray.append(value)

#value = pv.createUnionArrayElementField('doubleArr')
value = PvObject({'doubleArr' : [DOUBLE]})
value.setScalarArray([1.0,2.0,3.0])
unionArray.append(value)
print('unionArray','[',unionArray[0].getDouble(),',',unionArray[1].getScalarArray(),']')
print('Union Array Objects:')
print(unionArray[0])
print(unionArray[1])

#pv['value'] = unionArray
pv.setUnionArray(unionArray)
print('after unionArraySet',pv)
print('dictionary',pv.get())



