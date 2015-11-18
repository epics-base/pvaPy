#!/usr/bin/env python

import random
import string
import math
import types

from pvaccess import Channel

class TestUtility:

    FLOAT_PRECISION = 0.0001
    DOUBLE_PRECISION = 0.000001

    @classmethod
    def assertBooleanEquality(cls, value1, value2):
        assert(str(value1).lower() == str(value2).lower())

    @classmethod
    def convertToChar(cls, value):
        if type(value) == types.StringType:
            return value
        if type(value) == types.IntType and value < 0:
            value += 256
        return chr(value)

    @classmethod
    def assertCharEquality(cls, value1, value2):
        assert(cls.convertToChar(value1) == cls.convertToChar(value2))

    @classmethod
    def assertFloatEquality(cls, value1, value2):
        assert(math.fabs(value1-value2)<cls.FLOAT_PRECISION)

    @classmethod
    def assertDoubleEquality(cls, value1, value2):
        assert(math.fabs(value1-value2)<cls.DOUBLE_PRECISION)

    @classmethod
    def getRandomBoolean(cls):
        allowedValues = [False, True]
        return allowedValues[int(random.uniform(0,2))]

    @classmethod
    def getRandomBooleanString(cls):
        allowedValues = ['false', 'true']
        return allowedValues[int(random.uniform(0,2))]

    @classmethod
    def getRandomByte(cls):
        return int(random.uniform(-128,128))

    @classmethod
    def getRandomUByte(cls):
        return int(random.uniform(0,256))

    @classmethod
    def getRandomShort(cls):
        return int(random.uniform(-32768,32768))

    @classmethod
    def getRandomUShort(cls):
        return int(random.uniform(0,65536))

    @classmethod
    def getRandomInt(cls):
        #return int(random.uniform(-2147483648,2147483648))
        return int(random.uniform(-100,100))

    @classmethod
    def getRandomUInt(cls):
        return int(random.uniform(0,1000))
        #return int(random.uniform(0,4294967296))

    @classmethod
    def getRandomLong(cls):
        return long(random.uniform(-9223372036854775806,9223372036854775806))

    @classmethod
    def getRandomULong(cls):
        #return long(random.uniform(0,18446744073709551616))
        return long(random.uniform(0,10000))

    @classmethod
    def getRandomFloat(cls):
        #return random.uniform(-2147483648,2147483648)
        return random.uniform(-1,1)

    @classmethod
    def getRandomDouble(cls):
        #return random.uniform(-2147483648,2147483648)
        return random.uniform(-1,1)

    @classmethod
    def getRandomString(cls):
        size = int(random.uniform(1,256))
        allowedValues = string.ascii_uppercase + string.ascii_lowercase + string.digits
        return ''.join(random.choice(allowedValues) for _ in range(size))

    @classmethod
    def getBooleanChannel(cls):
        return Channel('exampleBoolean')

    @classmethod
    def getByteChannel(cls):
        return Channel('exampleByte')

    @classmethod
    def getUByteChannel(cls):
        return Channel('exampleUByte')

    @classmethod
    def getShortChannel(cls):
        return Channel('exampleShort')

    @classmethod
    def getUShortChannel(cls):
        return Channel('exampleUShort')

    @classmethod
    def getIntChannel(cls):
        return Channel('exampleInt')

    @classmethod
    def getUIntChannel(cls):
        return Channel('exampleUInt')

    @classmethod
    def getLongChannel(cls):
        return Channel('exampleLong')

    @classmethod
    def getULongChannel(cls):
        return Channel('exampleULong')

    @classmethod
    def getFloatChannel(cls):
        return Channel('exampleFloat')

    @classmethod
    def getDoubleChannel(cls):
        return Channel('exampleDouble')

    @classmethod
    def getStringChannel(cls):
        return Channel('exampleString')


