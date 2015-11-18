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

class TestChannelPutGet:

    #
    # Boolean PutGet
    #

    def testPutGet_PvBoolean(self):
        value = TestUtility.getRandomBoolean()
        c = TestUtility.getBooleanChannel()
        value2 = c.putGet(PvBoolean(value)).getPyObject()
        assert(value == value2)

    # put() must be done using strings 'true'/'false'
    def testPutGet_Boolean(self):
        value = TestUtility.getRandomBooleanString()
        c = TestUtility.getBooleanChannel()
        value2 = c.putGet(value).getPyObject()
        TestUtility.assertBooleanEquality(value,value2)

    def testPutGetBoolean_Boolean(self):
        value = TestUtility.getRandomBoolean()
        c = TestUtility.getBooleanChannel()
        value2 = c.putGetBoolean(value).getPyObject()
        assert(value == value2)

    #
    # Byte PutGet
    #

    # python chars are unsigned
    def testPutGet_PvByte(self):
        value = chr(TestUtility.getRandomUByte())
        c = TestUtility.getByteChannel()
        value2 = c.putGet(PvByte(value)).getPyObject()
        assert(value == value2)

    # put(byte) must be done using integers
    # we need to compare result in python chars, which are unsigned
    def testPutGet_Byte(self):
        value = TestUtility.getRandomByte()
        c = TestUtility.getByteChannel()
        value2 = c.putGet(value).getPyObject() 
        TestUtility.assertCharEquality(value,value2)

    def testPutGetByte_Byte(self):
        value = chr(TestUtility.getRandomUByte())
        c = TestUtility.getByteChannel()
        value2 = c.putGetByte(value).getPyObject()
        assert(value == value2)

    #
    # UByte PutGet
    #

    def testPutGet_PvUByte(self):
        value = TestUtility.getRandomUByte()
        c = TestUtility.getUByteChannel()
        value2 = c.putGet(PvUByte(value)).getPyObject()
        assert(value == value2)

    def testPutGet_UByte(self):
        value = TestUtility.getRandomUByte()
        c = TestUtility.getUByteChannel()
        value2 = c.putGet(value).getPyObject() 
        assert(value == value2)

    def testPutGetUByte_UByte(self):
        value = TestUtility.getRandomUByte()
        c = TestUtility.getUByteChannel()
        value2 = c.putGetUByte(value).getPyObject()
        assert(value == value2)

    #
    # Short PutGet
    #

    def testPutGet_PvShort(self):
        value = TestUtility.getRandomShort()
        c = TestUtility.getShortChannel()
        value2 = c.putGet(PvShort(value)).getPyObject()
        assert(value == value2)

    def testPutGet_Short(self):
        value = TestUtility.getRandomShort()
        c = TestUtility.getShortChannel()
        value2 = c.putGet(value).getPyObject() 
        assert(value == value2)

    def testPutGetShort_Short(self):
        value = TestUtility.getRandomShort()
        c = TestUtility.getShortChannel()
        value2 = c.putGetShort(value).getPyObject()
        assert(value == value2)

    #
    # UShort PutGet
    #

    def testPutGet_PvUShort(self):
        value = TestUtility.getRandomUShort()
        c = TestUtility.getUShortChannel()
        value2 = c.putGet(PvUShort(value)).getPyObject()
        assert(value == value2)

    def testPutGet_UShort(self):
        value = TestUtility.getRandomUShort()
        c = TestUtility.getUShortChannel()
        value2 = c.putGet(value).getPyObject() 
        assert(value == value2)

    def testPutGetUShort_UShort(self):
        value = TestUtility.getRandomUShort()
        c = TestUtility.getUShortChannel()
        value2 = c.putGetUShort(value).getPyObject()
        assert(value == value2)

    #
    # Int PutGet
    #

    def testPutGet_PvInt(self):
        value = TestUtility.getRandomInt()
        c = TestUtility.getIntChannel()
        value2 = c.putGet(PvInt(value)).getPyObject()
        assert(value == value2)

    def testPutGet_Int(self):
        value = TestUtility.getRandomInt()
        c = TestUtility.getIntChannel()
        value2 = c.putGet(value).getPyObject() 
        assert(value == value2)

    def testPutGetInt_Int(self):
        value = TestUtility.getRandomInt()
        c = TestUtility.getIntChannel()
        value2 = c.putGetInt(value).getPyObject()
        assert(value == value2)

    #
    # UInt PutGet
    #

    def testPutGet_PvUInt(self):
        value = TestUtility.getRandomUInt()
        c = TestUtility.getUIntChannel()
        value2 = c.putGet(PvUInt(value)).getPyObject()
        assert(value == value2)

    def testPutGet_UInt(self):
        value = TestUtility.getRandomUInt()
        c = TestUtility.getUIntChannel()
        value2 = c.putGet(value).getPyObject() 
        assert(value == value2)

    def testPutGetInt_UInt(self):
        value = TestUtility.getRandomUInt()
        c = TestUtility.getUIntChannel()
        value2 = c.putGetInt(value).getPyObject()
        assert(value == value2)

    #
    # Long PutGet
    #

    def testPutGet_PvLong(self):
        value = TestUtility.getRandomLong()
        c = TestUtility.getLongChannel()
        value2 = c.putGet(PvLong(value)).getPyObject()
        assert(value == value2)

    def testPutGet_Long(self):
        value = TestUtility.getRandomLong()
        c = TestUtility.getLongChannel()
        value2 = c.putGet(value).getPyObject() 
        assert(value == value2)

    def testPutGetLong_Long(self):
        value = TestUtility.getRandomLong()
        c = TestUtility.getLongChannel()
        value2 = c.putGetLong(value).getPyObject()
        assert(value == value2)

    #
    # ULong PutGet
    #

    def testPutGet_PvULong(self):
        value = TestUtility.getRandomULong()
        c = TestUtility.getULongChannel()
        value2 = c.putGet(PvULong(value)).getPyObject()
        assert(value == value2)

    def testPutGet_ULong(self):
        value = TestUtility.getRandomULong()
        c = TestUtility.getULongChannel()
        value2 = c.putGet(value).getPyObject() 
        assert(value == value2)

    def testPutGetULong_ULong(self):
        value = TestUtility.getRandomULong()
        c = TestUtility.getULongChannel()
        value2 = c.putGetULong(value).getPyObject()
        assert(value == value2)

    #
    # Float PutGet
    #

    def testPutGet_PvFloat(self):
        value = TestUtility.getRandomFloat()
        c = TestUtility.getFloatChannel()
        value2 = c.putGet(PvFloat(value)).getPyObject()
        TestUtility.assertFloatEquality(value, value2)

    def testPutGet_Float(self):
        value = TestUtility.getRandomFloat()
        c = TestUtility.getFloatChannel()
        value2 = c.putGet(value).getPyObject() 
        TestUtility.assertFloatEquality(value, value2)

    def testPutGetFloat_Float(self):
        value = TestUtility.getRandomFloat()
        c = TestUtility.getFloatChannel()
        value2 = c.putGetFloat(value).getPyObject()
        TestUtility.assertFloatEquality(value, value2)

    #
    # Double PutGet
    #

    def testPutGet_PvDouble(self):
        value = TestUtility.getRandomDouble()
        c = TestUtility.getDoubleChannel()
        value2 = c.putGet(PvDouble(value)).getPyObject()
        TestUtility.assertDoubleEquality(value, value2)

    def testPutGet_Double(self):
        value = TestUtility.getRandomDouble()
        c = TestUtility.getDoubleChannel()
        value2 = c.putGet(value).getPyObject() 
        TestUtility.assertDoubleEquality(value, value2)

    def testPutGetDouble_Double(self):
        value = TestUtility.getRandomDouble()
        c = TestUtility.getDoubleChannel()
        value2 = c.putGetDouble(value).getPyObject()
        TestUtility.assertDoubleEquality(value, value2)

    #
    # String PutGet
    #

    def testPutGet_PvString(self):
        value = TestUtility.getRandomString()
        c = TestUtility.getStringChannel()
        value2 = c.putGet(PvString(value)).getPyObject()
        assert(value == value2)

    def testPutGet_String(self):
        value = TestUtility.getRandomString()
        c = TestUtility.getStringChannel()
        value2 = c.putGet(value).getPyObject() 
        assert(value == value2)

    def testPutGetString_String(self):
        value = TestUtility.getRandomString()
        c = TestUtility.getStringChannel()
        value2 = c.putGetString(value).getPyObject()
        assert(value == value2)

