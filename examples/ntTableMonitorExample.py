#!/usr/bin/env python

from __future__ import print_function

import time
import pvaccess

def monitor(pvObject):
    ntTable = pvaccess.NtTable(pvObject)
    print("Full NT Table")
    print(ntTable)
    print("Column 0:")
    print(ntTable.getColumn(0))

c = pvaccess.Channel('testTable')
c.subscribe('m1', monitor)
c.startMonitor('field()')
time.sleep(10)
c.unsubscribe('m1')

