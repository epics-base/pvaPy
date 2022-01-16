#!/usr/bin/env python

import time
import random

from pvaccess import *

print 'Connecting'
c = Channel('float03')
print 'Connected'
c.setMonitorMaxQueueLength(3)
print 'Set monitor q length'
    
def echo(x=125):
    time.sleep(1.5)
    print 'ECHO: Got value in python: ', x
def echo2(x=125):
    #time.sleep(1.5)
    print 'ECHO2: Got value in python: ', x

for i in range (0,1):
    print 'Starting monitor: ', i
    c.subscribe('echo', echo)
    c.subscribe('echo2', echo2)
    c.startMonitor()

    time.sleep(10)
    print 'Stopping monitor: ', i
    #c.stopMonitor()
    #c.unsubscribe('echo2')

print 'Monitor loop done, exiting'
