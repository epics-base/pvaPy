#!/usr/bin/env python

import numpy as np
from pvaccess import PvObject
from pvaccess import BOOLEAN
from pvaccess import BYTE
from pvaccess import UBYTE
from pvaccess import SHORT
from pvaccess import USHORT
from pvaccess import INT
from pvaccess import UINT
from pvaccess import LONG
from pvaccess import ULONG
from pvaccess import FLOAT
from pvaccess import DOUBLE
from pvaccess import STRING

class TestNumPyArrays:

    #
    # Boolean 
    #

    def test_Boolean(self):
        a = np.random.randint(0,1, size=100, dtype=np.bool_)
        pv = PvObject({'a' : [BOOLEAN]}, {'a' : a})
        a2 = pv['a']
        c = a == a2
        assert(c.all())

    #
    # Byte
    #

    def test_Byte(self):
        a = np.random.randint(-128,128, size=100, dtype=np.int8)
        pv = PvObject({'a' : [BYTE]}, {'a' : a})
        a2 = pv['a']
        c = a == a2
        assert(c.all())
     
    #
    # UByte
    #

    def test_UByte(self):
        a = np.random.randint(0,256, size=100, dtype=np.uint8)
        pv = PvObject({'a' : [UBYTE]}, {'a' : a})
        a2 = pv['a']
        c = a == a2
        assert(c.all())
     
    #
    # Short
    #

    def test_Short(self):
        a = np.random.randint(-32768,32768, size=100, dtype=np.int16)
        pv = PvObject({'a' : [SHORT]}, {'a' : a})
        a2 = pv['a']
        c = a == a2
        assert(c.all())
     
    #
    # UShort
    #

    def test_UShort(self):
        a = np.random.randint(0,65536, size=100, dtype=np.uint16)
        pv = PvObject({'a' : [USHORT]}, {'a' : a})
        a2 = pv['a']
        c = a == a2
        assert(c.all())
     
    #
    # Int
    #

    def test_Int(self):
        a = np.random.randint(-214748364,214748364, size=100, dtype=np.int32)
        pv = PvObject({'a' : [INT]}, {'a' : a})
        a2 = pv['a']
        c = a == a2
        assert(c.all())
     
    #
    # UInt
    #

    def test_UInt(self):
        a = np.random.randint(0,4294967296, size=100, dtype=np.uint32)
        pv = PvObject({'a' : [UINT]}, {'a' : a})
        a2 = pv['a']
        c = a == a2
        assert(c.all())
     
    #
    # Long
    #

    def test_Long(self):
        a = np.random.randint(-9223372036854775808,9223372036854775808, size=100, dtype=np.int64)
        pv = PvObject({'a' : [LONG]}, {'a' : a})
        a2 = pv['a']
        c = a == a2
        assert(c.all())
     
    #
    # ULong
    #

    def test_ULong(self):
        a = np.random.randint(0,18446744073709551616, size=100, dtype=np.uint64)
        pv = PvObject({'a' : [ULONG]}, {'a' : a})
        a2 = pv['a']
        c = a == a2
        assert(c.all())
     
     
    #
    # Float
    #

    def test_Float(self):
        a = np.random.uniform(-9223372036854775808,9223372036854775808, size=100)
        a = a.astype(np.float32)
        pv = PvObject({'a' : [FLOAT]}, {'a' : a})
        a2 = pv['a']
        c = a == a2
        assert(c.all())
     
     
    #
    # Double
    #

    def test_Double(self):
        a = np.random.uniform(-9223372036854775808,9223372036854775808, size=100)
        pv = PvObject({'a' : [DOUBLE]}, {'a' : a})
        a2 = pv['a']
        c = a == a2
        assert(c.all())
     
       

