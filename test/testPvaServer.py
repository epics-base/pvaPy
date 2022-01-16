#!/usr/bin/env python
import pvaccess as pva
from testUtility import TestUtility

class TestPvaServer:

    def testAddAndRemoveRecord(self):
        s = pva.PvaServer()
        cName = 'c' + TestUtility.getRandomString(5)        
        s.addRecord(cName, pva.PvInt())
        assert(len(s.getRecordNames()) == 1)

        value = TestUtility.getRandomInt()
        c = pva.Channel(cName)
        print('Putting value to channel %s: %s' % (cName, value))
        c.put(pva.PvInt(value))
        value2 = c.get().getPyObject()
        print('Retrieved value from channel %s: %s' % (cName, value2))
        assert(value == value2)

        s.removeRecord(cName)
        assert(len(s.getRecordNames()) == 0)
        s.stop()
