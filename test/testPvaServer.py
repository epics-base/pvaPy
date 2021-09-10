#!/usr/bin/env python
import pvaccess as pva

class TestPvaServer:

    def testAddAndRemoveRecord(self):
        s = pva.PvaServer()
        s.addRecord('foo', pva.PvObject({'value': pva.INT}))
        assert(len(s.getRecordNames()) == 1)
        c = pva.Channel('foo')
        pv = c.get()
        print('Retrieved PV after adding record:\n%s' % pv)
        del c
        s.removeRecord('foo')
        assert(len(s.getRecordNames()) == 0)
        s.stop()
