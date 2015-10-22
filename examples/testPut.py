#!/usr/bin/env python

from __future__ import print_function

from pvaccess import Channel

print('****************************')
print('Testing string')
c = Channel('string01')
oldValue = c.get().getString()
print('Got old value: ', oldValue)
value = oldValue + '; a'
print('Putting value: ', value)
c.putString(value) 
newValue = c.get().getString()
print('Got new value: ', newValue)
assert newValue == value

value = oldValue + '; b'
print('Putting value: ', value)
c.put(value) 
newValue = c.get().getString()
print('Got new value: ', newValue)
assert newValue == value

print()
print('****************************')
print('Testing byte')
c = Channel('byte01')
oldValue = ord(c.get().getByte())
print('Got old value: ', oldValue)
value = oldValue + 10
print('Putting value via putByte(): ', value)
c.putByte(chr(value)) 
newValue = ord(c.get().getByte())
print('Got new value: ', newValue)
assert newValue == value

value = oldValue + 5
print('Putting value via put(): ', value)
c.put(value) 
newValue = ord(c.get().getByte())
print('Got new value: ', newValue)
assert newValue == value


print()
print('****************************')
print('Testing ubyte')
c = Channel('ubyte01')
oldValue = c.get().getUByte()
print('Got old value: ', oldValue)
value = oldValue + 10
print('Putting value via putUByte(): ', value)
c.putUByte(value) 
newValue = c.get().getUByte()
print('Got new value: ', newValue)
assert newValue == value

value = oldValue + 11
print('Putting value via put(): ', value)
c.put(value) 
newValue = c.get().getUByte()
print('Got new value: ', newValue)
assert newValue == value

print()
print('****************************')
print('Testing short')
c = Channel('short01')
oldValue = c.get().getShort()
print('Got old value: ', oldValue)
value = oldValue - 1
print('Putting value via putShort(): ', value)
c.putShort(value) 
newValue = c.get().getShort()
print('Got new value: ', newValue)
assert newValue == value

# undocumented put() for byte type should still work
value = oldValue - 2
print('Putting value via put(): ', value)
c.put(value) 
newValue = c.get().getShort()
print('Got new value: ', newValue)
assert newValue == value

print()
print('****************************')
print('Testing ushort')
c = Channel('ushort01')
oldValue = c.get().getUShort()
print('Got old value: ', oldValue)
value = oldValue + 1
print('Putting value via putUShort(): ', value)
c.putUShort(value) 
newValue = c.get().getUShort()
print('Got new value: ', newValue)
assert newValue == value

value = oldValue + 2
print('Putting value via put(): ', value)
c.put(value) 
newValue = c.get().getUShort()
print('Got new value: ', newValue)
assert newValue == value

print()
print('****************************')
print('Testing int')
c = Channel('int01')
oldValue = c.get().getInt()
print('Got old value: ', oldValue)
value = oldValue + 1
print('Putting value via putInt(): ', value)
c.putInt(value) 
newValue = c.get().getInt()
print('Got new value: ', newValue)
assert newValue == value

value = oldValue + 2
print('Putting value via put(): ', value)
c.put(value) 
newValue = c.get().getInt()
print('Got new value: ', newValue)
assert newValue == value

print()
print('****************************')
print('Testing uint')
c = Channel('uint01')
oldValue = c.get().getUInt()
print('Got old value: ', oldValue)
value = oldValue + 1
print('Putting value via putUInt(): ', value)
c.putUInt(value) 
newValue = c.get().getUInt()
print('Got new value: ', newValue)
assert newValue == value

value = oldValue + 2
print('Putting value via put(): ', value)
c.put(value) 
newValue = c.get().getUInt()
print('Got new value: ', newValue)
assert newValue == value

print()
print('****************************')
print('Testing float')
c = Channel('float01')
oldValue = c.get().getFloat()
print('Got old value: ', oldValue)
value = oldValue + 1.1
print('Putting value via putFloat(): ', value)
c.putFloat(value) 
newValue = c.get().getFloat()
print('Got new value: ', newValue)
assert abs(newValue-value) < 0.00001

value = oldValue + 1.1
print('Putting value via put(): ', value)
c.put(value) 
newValue = c.get().getFloat()
print('Got new value: ', newValue)
assert abs(newValue-value) < 0.00001

print()
print('****************************')
print('Testing double')
c = Channel('double01')
oldValue = c.get().getDouble()
print('Got old value: ', oldValue)
value = oldValue + 1.1
print('Putting value via putDouble(): ', value)
c.putDouble(value) 
newValue = c.get().getDouble()
print('Got new value: ', newValue)
assert abs(newValue-value) < 0.00001

value = oldValue + 1.1
print('Putting value via put(): ', value)
c.put(value) 
newValue = c.get().getDouble()
print('Got new value: ', newValue)
assert abs(newValue-value) < 0.00001


