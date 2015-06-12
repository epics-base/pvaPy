#!/usr/bin/env python

from pvaccess import *

print '\n*************************'
print '\nTEST regular union'
pv = PvObject({'anUnion' : ({'anInt' : INT, 'aFloat' : FLOAT},)})
print '\npv structure:\n', pv.getStructureDict()
print '\npv dict:\n', pv.get()
print '\npv:\n', pv

print '\nField names: ', pv.getUnionFieldNames()
print '\nSelected field name: ', pv.getSelectedUnionFieldName()

value = PvObject({'anInt' : INT})
value.setInt(33)
print '\nvalue:\n', value

print '\nSetting union using value object'
pv.setUnion(value)
print '\npv:\n', pv

print '\nField names: ', pv.getUnionFieldNames()
print '\nSelected field name: ', pv.getSelectedUnionFieldName()


print '\nSetting union using value object with different field name'
value = PvObject({'anInt2' : INT})
value.setInt(55)
print '\nvalue:\n', value
try:
    pv.setUnion(value)
except InvalidArgument, ex:
    print 'Caught exception as expected:', ex
print '\npv:\n', pv

print '\nField names: ', pv.getUnionFieldNames()
print '\nSelected field name: ', pv.getSelectedUnionFieldName()

print '\nSetting union using value object with different field type'
value = PvObject({'aDouble' : DOUBLE})
value.setDouble(33.22)
print '\nvalue:\n', value
try:
    pv.setUnion(value)
except InvalidArgument, ex:
    print 'Caught exception as expected:', ex

print '\npv:\n', pv

print '\nField names: ', pv.getUnionFieldNames()
print '\nSelected field name: ', pv.getSelectedUnionFieldName()
