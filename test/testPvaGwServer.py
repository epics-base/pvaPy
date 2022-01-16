#!/usr/bin/env python
import time
import pvaccess as pva
from testUtility import TestUtility

class TestPvaGwServer:

    def testAddAndRemoveGwRecord(self):
        s = pva.PvaGwServer()

        value = TestUtility.getRandomShort()
        c = TestUtility.getShortChannel()
        print('Putting value to channel %s: %s' % (c.getName(), value))
        c.put(pva.PvShort(value))
        value2 = c.get().getPyObject()
        print('Retrieved value from channel %s: %s' % (c.getName(), value2))
        assert(value == value2)

        gwChannelName = 'gw_' + TestUtility.getRandomString(5)
        s.addGwRecord(gwChannelName , c.getName(), pva.PVA)
        time.sleep(1.0)
        c2 = pva.Channel(gwChannelName)
        value2 = c2.get().getPyObject()
        print('Retrieved value from gateway channel %s: %s' % (gwChannelName, value2))
        assert(value == value2)

        assert(len(s.getGwRecordNames()) == 1)
        assert(len(s.getRecordNames()) == 1)
        s.removeGwRecord(gwChannelName)
        assert(len(s.getGwRecordNames()) == 0)
        assert(len(s.getRecordNames()) == 0)
        s.stop()
