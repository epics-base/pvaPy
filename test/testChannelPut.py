#!/usr/bin/env python

from pvaccess import Channel
from pvaccess import PvBoolean
from pvaccess import PvByte
from pvaccess import PvUByte
from pvaccess import PvShort
from pvaccess import PvUShort
from pvaccess import PvInt
from pvaccess import PvUInt
from pvaccess import PvLong
from pvaccess import PvULong
from pvaccess import PvFloat
from pvaccess import PvDouble
from pvaccess import PvString
from testUtility import TestUtility

class TestChannelPut:

    #
    # Boolean Put
    #

    def testPut_PvBoolean(self):
        value = TestUtility.getRandomBoolean()
        c = TestUtility.getBooleanChannel()
        c.put(PvBoolean(value))
        value2 = c.get().getPyObject()
        assert(value == value2)

    # put() must be done using strings 'true'/'false'
    def testPut_Boolean(self):
        value = TestUtility.getRandomBooleanString()
        c = TestUtility.getBooleanChannel()
        c.put(value)
        value2 = c.get().getPyObject()
        TestUtility.assertBooleanEquality(value,value2)

    def testPutBoolean_Boolean(self):
        value = TestUtility.getRandomBoolean()
        c = TestUtility.getBooleanChannel()
        c.putBoolean(value)
        value2 = c.get().getPyObject()
        assert(value == value2)

    #
    # Byte Put
    #

    # python chars are unsigned
    def testPut_PvByte(self):
        value = chr(TestUtility.getRandomUByte())
        c = TestUtility.getByteChannel()
        c.put(PvByte(value))
        value2 = c.get().getPyObject()
        assert(value == value2)

    # put(byte) must be done using integers
    # we need to compare result in python chars, which are unsigned
    def testPut_Byte(self):
        value = TestUtility.getRandomByte()
        c = TestUtility.getByteChannel()
        c.put(value)
        value2 = c.get().getPyObject() # Same as: value2 = c.get().getByte()
        TestUtility.assertCharEquality(value,value2)

    def testPutByte_Byte(self):
        value = chr(TestUtility.getRandomUByte())
        c = TestUtility.getByteChannel()
        c.putByte(value)
        value2 = c.get().getPyObject()
        assert(value == value2)

    #
    # UByte Put
    #

    def testPut_PvUByte(self):
        value = TestUtility.getRandomUByte()
        c = TestUtility.getUByteChannel()
        c.put(PvUByte(value))
        value2 = c.get().getPyObject()
        assert(value == value2)

    def testPut_UByte(self):
        value = TestUtility.getRandomUByte()
        c = TestUtility.getUByteChannel()
        c.put(value)
        value2 = c.get().getPyObject() 
        assert(value == value2)

    def testPutUByte_UByte(self):
        value = TestUtility.getRandomUByte()
        c = TestUtility.getUByteChannel()
        c.putUByte(value)
        value2 = c.get().getPyObject()
        assert(value == value2)

    #
    # Short Put
    #

    def testPut_PvShort(self):
        value = TestUtility.getRandomShort()
        c = TestUtility.getShortChannel()
        c.put(PvShort(value))
        value2 = c.get().getPyObject()
        assert(value == value2)

    def testPut_Short(self):
        value = TestUtility.getRandomShort()
        c = TestUtility.getShortChannel()
        c.put(value)
        value2 = c.get().getPyObject() 
        assert(value == value2)

    def testPutShort_Short(self):
        value = TestUtility.getRandomShort()
        c = TestUtility.getShortChannel()
        c.putShort(value)
        value2 = c.get().getPyObject()
        assert(value == value2)

    #
    # UShort Put
    #

    def testPut_PvUShort(self):
        value = TestUtility.getRandomUShort()
        c = TestUtility.getUShortChannel()
        c.put(PvUShort(value))
        value2 = c.get().getPyObject()
        assert(value == value2)

    def testPut_UShort(self):
        value = TestUtility.getRandomUShort()
        c = TestUtility.getUShortChannel()
        c.put(value)
        value2 = c.get().getPyObject() 
        assert(value == value2)

    def testPutUShort_UShort(self):
        value = TestUtility.getRandomUShort()
        c = TestUtility.getUShortChannel()
        c.putUShort(value)
        value2 = c.get().getPyObject()
        assert(value == value2)

    #
    # Int Put
    #

    def testPut_PvInt(self):
        value = TestUtility.getRandomInt()
        c = TestUtility.getIntChannel()
        c.put(PvInt(value))
        value2 = c.get().getPyObject()
        assert(value == value2)

    def testPut_Int(self):
        value = TestUtility.getRandomInt()
        c = TestUtility.getIntChannel()
        c.put(value)
        value2 = c.get().getPyObject() 
        assert(value == value2)

    def testPutInt_Int(self):
        value = TestUtility.getRandomInt()
        c = TestUtility.getIntChannel()
        c.putInt(value)
        value2 = c.get().getPyObject()
        assert(value == value2)

    #
    # UInt Put
    #

    def testPut_PvUInt(self):
        value = TestUtility.getRandomUInt()
        c = TestUtility.getUIntChannel()
        c.put(PvUInt(value))
        value2 = c.get().getPyObject()
        assert(value == value2)

    def testPut_UInt(self):
        value = TestUtility.getRandomUInt()
        c = TestUtility.getUIntChannel()
        c.put(value)
        value2 = c.get().getPyObject() 
        assert(value == value2)

    def testPutInt_UInt(self):
        value = TestUtility.getRandomUInt()
        c = TestUtility.getUIntChannel()
        c.putInt(value)
        value2 = c.get().getPyObject()
        assert(value == value2)

    #
    # Long Put
    #

    def testPut_PvLong(self):
        value = TestUtility.getRandomLong()
        c = TestUtility.getLongChannel()
        c.put(PvLong(value))
        value2 = c.get().getPyObject()
        assert(value == value2)

    def testPut_Long(self):
        value = TestUtility.getRandomLong()
        c = TestUtility.getLongChannel()
        c.put(value)
        value2 = c.get().getPyObject() 
        assert(value == value2)

    def testPutLong_Long(self):
        value = TestUtility.getRandomLong()
        c = TestUtility.getLongChannel()
        c.putLong(value)
        value2 = c.get().getPyObject()
        assert(value == value2)

    #
    # ULong Put
    #

    def testPut_PvULong(self):
        value = TestUtility.getRandomULong()
        c = TestUtility.getULongChannel()
        c.put(PvULong(value))
        value2 = c.get().getPyObject()
        assert(value == value2)

    def testPut_ULong(self):
        value = TestUtility.getRandomULong()
        c = TestUtility.getULongChannel()
        c.put(value)
        value2 = c.get().getPyObject() 
        assert(value == value2)

    def testPutULong_ULong(self):
        value = TestUtility.getRandomULong()
        c = TestUtility.getULongChannel()
        c.putULong(value)
        value2 = c.get().getPyObject()
        assert(value == value2)

    #
    # Float Put
    #

    def testPut_PvFloat(self):
        value = TestUtility.getRandomFloat()
        c = TestUtility.getFloatChannel()
        c.put(PvFloat(value))
        value2 = c.get().getPyObject()
        TestUtility.assertFloatEquality(value, value2)

    def testPut_Float(self):
        value = TestUtility.getRandomFloat()
        c = TestUtility.getFloatChannel()
        c.put(value)
        value2 = c.get().getPyObject() 
        TestUtility.assertFloatEquality(value, value2)

    def testPutFloat_Float(self):
        value = TestUtility.getRandomFloat()
        c = TestUtility.getFloatChannel()
        c.putFloat(value)
        value2 = c.get().getPyObject()
        TestUtility.assertFloatEquality(value, value2)

    #
    # Double Put
    #

    def testPut_PvDouble(self):
        value = TestUtility.getRandomDouble()
        c = TestUtility.getDoubleChannel()
        c.put(PvDouble(value))
        value2 = c.get().getPyObject()
        TestUtility.assertDoubleEquality(value, value2)

    def testPut_Double(self):
        value = TestUtility.getRandomDouble()
        c = TestUtility.getDoubleChannel()
        c.put(value)
        value2 = c.get().getPyObject() 
        TestUtility.assertDoubleEquality(value, value2)

    def testPutDouble_Double(self):
        value = TestUtility.getRandomDouble()
        c = TestUtility.getDoubleChannel()
        c.putDouble(value)
        value2 = c.get().getPyObject()
        TestUtility.assertDoubleEquality(value, value2)

    #
    # String Put
    #

    def testPut_PvString(self):
        value = TestUtility.getRandomString()
        c = TestUtility.getStringChannel()
        c.put(PvString(value))
        value2 = c.get().getPyObject()
        assert(value == value2)

    def testPut_String(self):
        value = TestUtility.getRandomString()
        c = TestUtility.getStringChannel()
        c.put(value)
        value2 = c.get().getPyObject() 
        assert(value == value2)

    def testPutString_String(self):
        value = TestUtility.getRandomString()
        c = TestUtility.getStringChannel()
        c.putString(value)
        value2 = c.get().getPyObject()
        assert(value == value2)

