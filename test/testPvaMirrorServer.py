#!/usr/bin/env python
import time
import pvaccess as pva
from testUtility import TestUtility

class TestPvaMirrorServer:

    def testAddAndRemoveMirrorRecord(self):
        s = pva.PvaMirrorServer()

        value = TestUtility.getRandomShort()
        c = TestUtility.getShortChannel()
        print('Putting value to channel %s: %s' % (c.getName(), value))
        c.put(pva.PvShort(value))
        value2 = c.get().getPyObject()
        print('Retrieved value from channel %s: %s' % (c.getName(), value2))
        assert(value == value2)

        mirrorChannelName = 'mirror_' + TestUtility.getRandomString(5)
        s.addMirrorRecord(mirrorChannelName , c.getName(), pva.PVA)
        time.sleep(1.0)
        c2 = pva.Channel(mirrorChannelName)
        value2 = c2.get().getPyObject()
        print('Retrieved value from mirror channel %s: %s' % (mirrorChannelName, value2))
        assert(value == value2)

        assert(len(s.getMirrorRecordNames()) == 1)
        assert(len(s.getRecordNames()) == 1)
        s.removeMirrorRecord(mirrorChannelName)
        assert(len(s.getMirrorRecordNames()) == 0)
        assert(len(s.getRecordNames()) == 0)
        s.stop()
