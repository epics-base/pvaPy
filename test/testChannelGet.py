#!/usr/bin/env python

import time
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
from pvaccess import PvTimeStamp
from testUtility import TestUtility

class TestChannelGet:

    def getCallback(self, pv):
        self.retrievedPv = pv.copy()

    def getErrorCallback(self, error):
        self.retrievalError = error

    #
    # Async Get
    #
    def testAsyncGet(self):
        i = TestUtility.getRandomInt()
        c = TestUtility.getStructChannel()
        pv = c.get('')
        pv['int'] = i
        print('\nSetting int field to: %s' % (i))
        c.put(pv, '')

        self.retrievedPv = {'int' : 0.1}
        c.asyncGet(self.getCallback, None, '')
        time.sleep(1.0)
        print('Testing equality: %s == %s' % (pv['int'], self.retrievedPv['int']))
        assert(pv['int'] == self.retrievedPv['int'])

    #
    # Async Get Error
    #
    def testAsyncGet_Error(self):
        i = TestUtility.getRandomInt()
        c = TestUtility.getStructChannel()
        self.retrievalError = None
        c.asyncGet(self.getCallback, self.getErrorCallback, 'field(nonexisting)')
        time.sleep(1.0)
        print('Testing error message is not None: %s' % (self.retrievalError))
        assert(self.retrievalError != None)

