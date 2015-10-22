#!/usr/bin/env python

from __future__ import print_function

from pvaccess import *

pv = PvObject({
    'i' : INT, 
    'ui' : UINT, 
    's' : STRING,
    'sa' : [STRING],
    'd' : DOUBLE,
    'l' : LONG,
    'lint' : [INT],
    'llong' : [LONG],
    'struct' : { 'a' : STRING, 'b' : [BOOLEAN], 'c' : { 'a1' : FLOAT, 'a2' : [STRING] }},
    'lstruct' : [{'one' : INT, 'two' : INT, 'three' : DOUBLE}]
})
print(pv)

print('Setting int: ')
pv.setInt('i', 678)
print(pv)

print('Setting uint: ')
pv.setUInt('ui', 678678)
print(pv)

print('Setting long: ')
pv.setLong('l', 1234567891012345678)
print(pv)

print('Setting string: ')
pv.setString('s', 'Very long string')
print(pv)

print('Setting double: ')
pv.setDouble('d', 1.2345e30)
print(pv)

print('Setting int list: ')
pv.setScalarArray('lint', [0,1,2,3,4,5,6,7,8,9])
print(pv)

print('Setting long list: ')
pv.setScalarArray('llong', [123456789, 987654321])
print(pv)

print('Setting string list: ')
pv.setScalarArray('sa', ['xyz', 'zyx', 'wwwwwwwwe'])
print(pv)

print('Setting struct: ')
pv.setStructure('struct', { 'a' : 'Another string', 'b' : [False, True, True], 'c' : { 'a1' : 13.45, 'a2' : ['aaa', 'bbb', 'ccc'] }})
print(pv)

print('Setting lstruct: ')
pv.setStructureArray('lstruct', [
    {'one' : 1, 'two' : 2, 'three' : 3.0},
    {'one' : 11, 'two' : 22, 'three' : 33.0},
    {'one' : 111, 'two' : 222, 'three' : 333.0},
    ])
print(pv)

pv.set({'i' : 1, 'ui' : 22})
print(pv)

print('Python dictionary: ')
pyDict = pv.get()
print(pyDict)

print('Testing boolean functions: ')
pv = PvObject({'value' : BOOLEAN})
pv.setBoolean(True)
print(pv)

