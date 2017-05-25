#!/usr/bin/env python

import sys
import time
from pvaccess import Channel
from collections import OrderedDict

class ScaleMonitor:

    def __init__(self, name):
        self.name = name
        self.value = 0
        self.nReceived = 0
        self.nMissed = 0
        self.percentageMissed = 0
        self.startTime = 0
        self.receiveRate = 0

    def toString(self):
        return '%6s: Received: %7d (%6.2f [Hz]); Missed: %7d (%6.2f%%)' % (self.name, self.nReceived, self.receiveRateHz, self.nMissed, self.percentageMissed)
       
    def monitor2(self, pv):
        print('MONITOR 2: %s\n' % pv['value'])

    def monitor(self, pv):
        oldValue = self.value
        self.value = pv['value']
        self.nReceived += 1
        diff = self.value - oldValue
        if oldValue > 0:
            self.nMissed += diff-1
        else:
            self.startTime = time.time()
 
        if self.nReceived % 10 == 0:
            currentTime = time.time()
            deltaT = currentTime - self.startTime
            self.receiveRateHz = self.nReceived/deltaT
            self.percentageMissed = self.nMissed/(self.nReceived+self.nMissed)*100.0
            
        if self.nReceived % 10 == 0:
            print(self.toString())

if __name__ == '__main__':
    RUNTIME = 60
    channelName = 'X1' 
    c = Channel(channelName)
    print('CONNECT TO %s:\n%s\n' % (channelName, c.get()))
    m = ScaleMonitor(channelName)
    c.subscribe('m', m.monitor)

    # Monitor
    print('STARTING MONITOR for X1\n')
    c.startMonitor()
    time.sleep(10)
    print('SUBSCRIBE M2 for X1\n')
    c.subscribe('m2', m.monitor2)
    time.sleep(10)
    c.unsubscribe('m2')
    print('UNSUBSCRIBE M2 for X1\n')
    time.sleep(10)
    
    print('SUBSCRIBE M2 for X1\n')
    c.subscribe('m2', m.monitor2)
    time.sleep(10)
    c.unsubscribe('m2')
    print('UNSUBSCRIBE M2 for X1\n')

    time.sleep(RUNTIME)
    c.stopMonitor()

    print('STOP MONITOR for X1\n')
    print('FINAL STATS:')
    print(m.toString())
    print('DONE')
      

