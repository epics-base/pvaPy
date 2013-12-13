#!/usr/bin/env python

import time
from pvaccess import *

c = Channel('float03')
def echo(x=125):
    print 'Got value: ', x
c.subscribe('echo', echo)
c.startMonitor()

time.sleep(10)
c.unsubscribe('echo')
time.sleep(5)
c.stopMonitor()
