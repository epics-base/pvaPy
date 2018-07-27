#!/usr/bin/env python

#
# This example can be used to monitor large number of channels with slow 
# updates.
#
# For example, one could include the following "scale.db" into pvaSrv build:
# $ cat scale.db 
# record(calc, "$(N)") {
#   field(SCAN, "1 second")
#   field(INPA, "$(N) NPP")
#   field(CALC, "A+1")
# }
#
# Load N channels "X<id>" into your IOC by adding lines like this into your
# st.cmd file:
#
# dbLoadRecords("db/scale.db","N=X1");
#
# Run pvaSrv (e.g., "../../bin/linux-x86_64/testDbPv st.cmd.scale"), and then
# monitor channels using
#
# scaleN.py [nChannels, [runtime]]
#
# This example worked well with up to 75000 channels.
#

import sys
import time
from pvaccess import Channel, CA
from collections import OrderedDict

class ScaleMonitor:

    def __init__(self, name, outputMark):
        self.name = name
        self.value = 0
        self.nReceived = 0
        self.nMissed = 0
        self.percentageMissed = 0
        self.startTime = 0
        self.receiveRate = 0
        self.receiveRateHz = 0
        self.outputMark = outputMark

    def toString(self):
        return '%6s: Received: %7d (%6.2f [Hz]); Missed: %7d (%6.2f%%)' % (self.name, self.nReceived, self.receiveRateHz, self.nMissed, self.percentageMissed)
       
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
            
        if self.nReceived % self.outputMark == 0:
            print(self.toString())

if __name__ == '__main__':
    runtime = 60
    nChannels = 1
    if len(sys.argv) > 1:
        nChannels = int(sys.argv[1])
    if len(sys.argv) > 2:
        runtime = float(sys.argv[2])
    outputMark = int(runtime/5)

    channelMap = OrderedDict()
    for channelCnt in range(1,nChannels+1):
        channelName = 'X%s' % channelCnt
        #c = Channel(channelName, CA)
        c = Channel(channelName)
        #print('CONNECT TO %s:\n%s\n' % (channelName, c.get()))
        m = ScaleMonitor(channelName, outputMark)
        c.subscribe('m%s' % channelName, m.monitor)
        channelMap[channelName] = (c,m)

    # Monitor
    print('STARTING MONITORS\n')
    t0 = time.time()
    channelCount = 0
    for cName,(c,m) in channelMap.items():
        print('STARTING MONITOR for %s\n' % (cName))
        c.startMonitor()
        channelCount += 1
        if channelCount % 1000 == 0:
            time.sleep(1)
        #if channelCount > 90000 and channelCount % 100 == 0:
        #    time.sleep(60)
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
    receiveRateHz = totalReceived/deltaT
    percentageMissed = 0.0
    if totalReceived+totalMissed > 0:
        percentageMissed = totalMissed/(totalReceived+totalMissed)*100.0
    print('')
    print('RUNTIME: %.2f [s]' % (deltaT))
    print('CHANNELS: %9d' % (nChannels))
    print('TOTAL RECEIVED: %9d (%6.2f [Hz])' % (totalReceived, receiveRateHz))
    print('TOTAL MISSED:   %9d (%6.2f%%)' % (totalMissed, percentageMissed))
    print('\nDONE')

