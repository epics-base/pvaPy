#!/usr/bin/env python

#
# This example can be used to monitor few channels with very fast updates.
#
# For example, one could include the following "spam.db" into pvaSrv build:
# $ cat spam.db 
# record(calc, "$(N)") {
#  field(INPA, "$(N).VAL CP")
#  field(CALC, "A+1")
# }
#
# Load N channels "X<id>" into your IOC by adding lines like this into your 
# st.cmd file:
#
# dbLoadRecords("db/spam.db","N=X1");
#
# Run pvaSrv (e.g., "../../bin/linux-x86_64/testDbPv st.cmd.spam"), and then
# monitor channels using 
#
# spamN.py [nChannels, [runtime]]
#


import sys
import time
from pvaccess import Channel
from collections import OrderedDict

class SpamMonitor:

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
        self.value = pv['value']
        self.nReceived += 1
        diff = self.value - oldValue
        if oldValue > 0:
            self.nMissed += diff-1
        else:
            self.startTime = time.time()
 
        if self.nReceived % 10000 == 0:
            currentTime = time.time()
            deltaT = currentTime - self.startTime
            self.receiveRateKHz = self.nReceived/deltaT/1000
            self.percentageMissed = self.nMissed/(self.nReceived+self.nMissed)*100.0
            
        if self.nReceived % 100000 == 0:
            print(self.toString())

if __name__ == '__main__':
    runtime = 60
    nChannels = 1
    if len(sys.argv) > 1:
        nChannels = int(sys.argv[1])
    if len(sys.argv) > 2:
        runtime = float(sys.argv[2])

    channelMap = OrderedDict()
    for channelCnt in range(1,nChannels+1):
        channelName = 'X%s' % channelCnt
        c = Channel(channelName)
        #print('CONNECT TO %s:\n%s\n' % (channelName, c.get()))
        m = SpamMonitor(channelName)
        c.subscribe('m%s' % channelName, m.monitor)
        channelMap[channelName] = (c,m)

    # Monitor
    print('STARTING MONITORS\n')
    t0 = time.time()
    for cName,(c,m) in channelMap.items():
        print('STARTING MONITOR for %s\n' % (cName))
        c.startMonitor()
    print('START MONITORS at %s\n' % t0)

    time.sleep(runtime)

    for cName,(c,m) in channelMap.items():
        c.stopMonitor()
    t1 = time.time()
    deltaT = t1-t0
    print('STOP MONITORS at %s\n' % t1)

    print('FINAL STATS:') 
    totalReceived = 0
    totalMissed = 0
    for cName,(c,m) in channelMap.items():
        print(m.toString())
        totalReceived += m.nReceived
        totalMissed += m.nMissed
    receiveRateKHz = totalReceived/deltaT/1000
    percentageMissed = totalMissed/(totalReceived+totalMissed)*100.0
    print('')
    print('RUNTIME: %.2f [s]' % (deltaT))
    print('TOTAL RECEIVED: %9d (%6.2f [kHz])' % (totalReceived, receiveRateKHz)) 
    print('TOTAL MISSED:   %9d (%6.2f%%)' % (totalMissed, percentageMissed))  
    print('\nDONE')
      

