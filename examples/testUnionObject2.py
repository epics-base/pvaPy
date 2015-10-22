#!/usr/bin/env python

from __future__ import print_function

from pvaccess import *

print('\n*************************')
print('\nTEST regular union')
pvtemp = PvObject({ 'd' : DOUBLE, 'dArr' : [DOUBLE]})
print('\nCONSTRUCT union via pvtemp structure:')
print(pvtemp.getStructureDict())

union = PvUnion(pvtemp)
print('union=PvUnion(pvtemp):\n',union)
print('introspection:\n',union.getStructureDict())
print('dictionary:\n',union.toDict())

union2 = PvObject({'value' : ({ 'd' : DOUBLE, 'dArr' : [DOUBLE]},)})
print('\nCONSTRUCT union via value tuple:')
print('union2=PvObject({"value":({},)}):\n',union2)
print('introspection:\n',union2.getStructureDict())
print('dictionary:\n',union2.toDict())


pv = union
isVariant = pv.isUnionVariant()
if isVariant==True :
    print(channelName,'is a variant array')
    exit
names = pv.getUnionFieldNames()
print('\nunion fieldNames: ',names)
print('\nSELECT double field')
value = pv.selectUnionField('d')
print('value:\n', value)

value.setDouble(2.0)
print('\nAFTER setDouble')
print('pv structure:\n',pv)
print('pv dictionary:\n',pv.get())
print('value:\n', value)

print('\nSELECT double array field')
value = pv.selectUnionField('dArr')
value.setScalarArray([1.0,2.0,3.0,4.0])
print('\nAFTER setScalarArray')
print('pv structure:\n',pv)
print('pv dictionary:\n',pv.get())
print('value:\n', value)

