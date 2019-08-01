#!/usr/bin/env python
import pvaccess as pva

s = pva.PvaServer('foo', pva.PvObject({'value': pva.INT}))
c = pva.Channel('foo')
c.get()
s.removeRecord('foo')
