#!/usr/bin/env python

import time
import sys

import pvaccess as pva

class TestServer(pva.PvaServer):

    BOOLEAN_CHANNEL_NAME = 'testBoolean'
    BYTE_CHANNEL_NAME = 'testByte'
    UBYTE_CHANNEL_NAME = 'testUByte'
    SHORT_CHANNEL_NAME = 'testShort'
    USHORT_CHANNEL_NAME = 'testUShort'
    INT_CHANNEL_NAME = 'testInt'
    UINT_CHANNEL_NAME = 'testUInt'
    LONG_CHANNEL_NAME = 'testLong'
    ULONG_CHANNEL_NAME = 'testULong'
    FLOAT_CHANNEL_NAME = 'testFloat'
    DOUBLE_CHANNEL_NAME = 'testDouble'
    STRING_CHANNEL_NAME = 'testString'
    STRUCT_CHANNEL_NAME = 'testStruct'

    STRUCT_OBJECT_TYPE_DICT = {
        'value' : pva.DOUBLE,
        'boolean' : pva.BOOLEAN,
        'byte' : pva.BYTE,
        'bytea' : [pva.BYTE],
        'ubyte' : pva.UBYTE,
        'ubytea' : [pva.UBYTE],
        'short' : pva.SHORT,
        'shorta' : [pva.SHORT],
        'ushort' : pva.USHORT,
        'ushorta' : [pva.USHORT],
        'int' : pva.INT,
        'inta' : [pva.INT],
        'uint' : pva.UINT,
        'uinta' : [pva.UINT],
        'long' : pva.LONG,
        'longa' : [pva.LONG],
        'ulong' : pva.ULONG,
        'ulonga' : [pva.ULONG],
        'string' : pva.STRING,
        'stringa' : [pva.STRING],
        'float' : pva.FLOAT,
        'floata' : [pva.FLOAT],
        'double' : pva.DOUBLE,
        'doublea' : [pva.DOUBLE],
        'timestamp' : pva.PvTimeStamp(),
        'alarm' : pva.PvAlarm()
    }

    def __init__(self):
        pva.PvaServer.__init__(self)
        self.booleanPv = pva.PvBoolean()
        self.addRecord(self.BOOLEAN_CHANNEL_NAME, self.booleanPv)

        self.bytePv = pva.PvByte()
        self.addRecord(self.BYTE_CHANNEL_NAME, self.bytePv)

        self.ubytePv = pva.PvUByte()
        self.addRecord(self.UBYTE_CHANNEL_NAME, self.ubytePv)

        self.shortPv = pva.PvShort()
        self.addRecord(self.SHORT_CHANNEL_NAME, self.shortPv)

        self.ushortPv = pva.PvUShort()
        self.addRecord(self.USHORT_CHANNEL_NAME, self.ushortPv)

        self.intPv = pva.PvInt()
        self.addRecord(self.INT_CHANNEL_NAME, self.intPv)

        self.uintPv = pva.PvUInt()
        self.addRecord(self.UINT_CHANNEL_NAME, self.uintPv)

        self.longPv = pva.PvLong()
        self.addRecord(self.LONG_CHANNEL_NAME, self.longPv)

        self.ulongPv = pva.PvULong()
        self.addRecord(self.ULONG_CHANNEL_NAME, self.ulongPv)

        self.floatPv = pva.PvFloat()
        self.addRecord(self.FLOAT_CHANNEL_NAME, self.floatPv)

        self.doublePv = pva.PvDouble()
        self.addRecord(self.DOUBLE_CHANNEL_NAME, self.doublePv)

        self.stringPv = pva.PvString('')
        self.addRecord(self.STRING_CHANNEL_NAME, self.stringPv)

        self.structPv = pva.PvObject(self.STRUCT_OBJECT_TYPE_DICT)
        self.addRecord(self.STRUCT_CHANNEL_NAME, self.structPv)

    def __del__(self):
        records = self.getRecordNames()
        for r in records:
            self.removeRecord(r)

######################################################################
# run test server
if __name__ == '__main__':
    SERVER_RUN_TIME = 30
    if len(sys.argv) > 1:
        SERVER_RUN_TIME = float(sys.argv[1])
    server = TestServer()
    time.sleep(SERVER_RUN_TIME)



