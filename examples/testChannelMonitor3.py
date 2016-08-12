#!/usr/bin/env python

import time
import random

from pvaccess import *

c = Channel('float03')

def updateChannel(x):
  c.put(x)
    
for i in range (0,3):
    # do something else
    #print "Average(100000): ", average(100000)
    x = 0.1
    for i in range(0,1000):
        x = x+0.001
        print 'Updating x=',x
        updateChannel(x)
    time.sleep(1)

print 'Exiting'
