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
c.startMonitor()

# do something else
time.sleep(1)
print "Average(100000): ", average(100000)

time.sleep(10)
print "Average(100): ", average(100)
c.unsubscribe('echo')
time.sleep(5)
#c.stopMonitor()
#print "Average(1000000): ", average(1000000)
