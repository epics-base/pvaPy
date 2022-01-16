#!/usr/bin/env python

import time
import random

from pvaccess import *

def average(n):
    sum = 0
    for i in range(0,n):
        sum += random.uniform(0,1)
    return sum/n

c = Channel('float03')
def echo(x=125):
    print 'Got value in python: ', x
c.subscribe('echo', echo)

for i in range (0,3):
    print 'Starting monitor: ', i
    c.startMonitor()

    # do something else
    #print "Average(100000): ", average(100000)
    time.sleep(10)
    #print "Average(100): ", average(100)
    print 'Stopping monitor: ', i
    c.stopMonitor()
    #print "Average(50): ", average(100)

print 'Monitor loop Done, unsubscribing monitor'
c.unsubscribe('echo')
#time.sleep(5)
print 'Exiting'
