#!/usr/bin/env python

# 
# This example can be used to demonstrate pvaPy server/client channel 
# monitoring
# 
# Run server.py in one window, and client.py in another one.
#

import sys
import time
from pvaccess import Channel
from collections import OrderedDict

class ClientMonitor:

    def __init__(self, name):
        self.name = name
        self.value = 0
        self.nReceived = 0
        self.nMissed = 0
        self.percentageMissed = 0
        self.startTime = 0
        self.receiveRate = 0

    def toString(self):
        return '%6s: Received: %7d (%6.2f [kHz]); Missed: %7d (%6.2f%%)' % (self.name, self.nReceived, self.receiveRateKHz, self.nMissed, self.percentageMissed)
       
    def monitor(self, pv):
        oldValue = self.value
        self.value = pv['c']
        self.nReceived += 1
        diff = self.value - oldValue
        if oldValue > 0:
            self.nMissed += diff-1
        else:
            self.startTime = time.time()
 
        if self.nReceived % 10000 == 0:
            currentTime = time.time()
            deltaT = currentTime - self.startTime
            self.receiveRateKHz = self.nReceived/deltaT/1000.0
            self.percentageMissed = (self.nMissed*100.0)/(self.nReceived+self.nMissed)
            
        if self.nReceived % 100000 == 0:
            print(self.toString())

if __name__ == '__main__':
    runtime = 60
    if len(sys.argv) > 1:
        runtime = float(sys.argv[1])

    channelName = 'counter'
    c = Channel(channelName)
    #print('CONNECT TO %s:\n%s\n' % (channelName, c.get()))
    m = ClientMonitor(channelName)

    t0 = time.time()
    print('STARTING MONITOR for %s at %s\n' % (channelName, t0))
    #c.monitor(m.monitor)
    c.monitor(m.monitor, 'field(c)')
    time.sleep(runtime)
    c.stopMonitor()
    t1 = time.time()
    deltaT = t1-t0
    print('STOP MONITOR at %s\n' % t1)

    print('FINAL STATS:') 
    print(m.toString())
    print('')
    print('RUNTIME: %.2f [s]' % (deltaT))
    print('\nDONE')

