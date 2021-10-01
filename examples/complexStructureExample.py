#!/usr/bin/env python

import sys
import pvaccess as pva

ATTR_STRUCTURE = {
  'name': pva.STRING, 
  'value': (), 
  'descriptor': pva.STRING, 
  'sourceType': pva.INT, 
  'source': pva.INT
}

ATTR_STRUCTURE2 = {
  'name': pva.STRING, 
  'value': pva.DOUBLE, 
  'descriptor': pva.STRING, 
  'sourceType': pva.INT, 
  'source': pva.INT
}

ATTR_STRUCTURE3 = {
  'name': pva.STRING, 
  'value': ({'i' : pva.INT, 'd' : pva.DOUBLE, 's' : pva.STRING},), 
  'descriptor': pva.STRING, 
  'sourceType': pva.INT, 
  'source': pva.INT
}

STRUCTURE = {
  'value': {
    'column0': [pva.DOUBLE],
    'column1': [pva.DOUBLE], 
    'column2': [pva.DOUBLE]
  }, 
  'labels': [pva.STRING], 
  'descriptor': pva.STRING, 
  'counter': pva.LONG, 
  'digitizerTimeStamp': [pva.DOUBLE], 
  'attribute': [ATTR_STRUCTURE],
  'a': ATTR_STRUCTURE,
  'a2': ATTR_STRUCTURE2,
  'a3': ATTR_STRUCTURE3,
  'timeStamp': {
    'secondsPastEpoch': pva.LONG, 
    'nanoseconds': pva.INT, 
    'userTag': pva.INT
  }, 
  'alarm': {
    'severity': pva.INT, 
    'status': pva.INT, 
    'message': pva.STRING
  }
}

pv = pva.PvObject(STRUCTURE)
print('EMPTY PV OBJECT')
print(pv)
print('============================')

pv['value.column0'] = [0.1,0.2,0.3,0.4,0.5]
pv['value.column1'] = [1.1,1.2,1.3,1.4,1.5]
pv['value.column2'] = [2.1,2.2,2.3,2.4,2.5]
print('ADDED COLUMN DATA TO PV OBJECT')
print(pv)
print('============================')

attr1 = pva.PvObject(ATTR_STRUCTURE)
attr1['name'] = 'MY ATTR 1'
attr1['descriptor'] = 'very long description'
attr1['value'] = pva.PvDouble(13)
print('CREATED ATTR1')
print(attr1)
print('============================')

attr1.setUnion('value', pva.PvInt(14))
print('MODIFIED ATTR1')
print(attr1)
print('============================')

attr2 = pva.PvObject(ATTR_STRUCTURE)
attr2['name'] = 'MY ATTR 2'
attr2['descriptor'] = 'very long description'
attr2.setUnion('value', pva.PvInt(1124))
print('CREATED ATTR2')
print(attr2)
print('============================')

attrs = [attr1,attr2]
pv['attribute'] = attrs
print('MODIFIED PV OBJECT WITH ATTRS')
print(pv)
print('============================')

attrs = [attr2,attr1]
pv.setStructureArray('attribute',attrs)
print('MODIFIED PV OBJECT WITH ATTRS REVERSED')
print(pv)
print('============================')

a = pva.PvObject(ATTR_STRUCTURE)
a['name'] = 'A'
a['descriptor'] = 'very long A description'
a['value'] = pva.PvInt(13)
print('CREATED OBJECT A')
print(a)
print('============================')

print('MODIFIED PV OBJECT WITH OBJECT A')
pv['a'] = a
print(pv)
print('============================')

a['value'] = pva.PvFloat(88)
print('MODIFIED OBJECT A')
print(a)
print('============================')

pv.setStructure('a',a)
print('MODIFIED PV OBJECT WITH OBJECT A')
print(pv)
print('============================')

a2 = pva.PvObject(ATTR_STRUCTURE2)
a2['name'] = 'A2'
a2['descriptor'] = 'very long A2 description'
a2['value'] = 13
print('CREATED OBJECT A2')
print(a2)
print('============================')

pv['a2'] = a2
print('MODIFIED PV OBJECT WITH OBJECT A2')
print(pv)
print('============================')

a2['value'] = 99
print('MODIFIED OBJECT A2')
print(a2)
print('============================')

pv.setStructure('a2',a2)
print('MODIFIED PV OBJECT WITH OBJECT A2')
print(pv)
print('============================')

a3 = pva.PvObject(ATTR_STRUCTURE3)
a3['name'] = 'A3'
a3['descriptor'] = 'very long A3 description'
a3['value'] = pva.PvObject({'i':pva.INT}, {'i':555})
print('CREATED OBJECT A3')
print(a3)
print('============================')

pv['a3'] = a3
print('MODIFIED PV OBJECT WITH OBJECT A3')
print(pv)
print('============================')

a3.setUnion(pva.PvObject({'s':pva.STRING}, {'s':'hello there'}))
print('MODIFIED OBJECT A3')
print(a3)
print('============================')

pv.setStructure('a3',a3)
print('MODIFIED PV OBJECT WITH OBJECT A3')
print(pv)
print('============================')



