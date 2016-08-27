#!/usr/bin/env python

import time
import math
import pvaccess
from pvaccess import Channel
from pvaccess import PvObject

MEGA_BYTE = 1024*1024

class Stats:

    def __init__(self, name):
        self.nElements = 0
        self.sum = 0.0
        self.sum2 = 0.0
        self.min = float('inf')
        self.max = -float('inf')
        self.last = None
        self.name = name
        self.sigma = 0.0
        self.average = 0.0

    def add(self, x):
        self.last = x
        self.sum += x
        self.sum2 += x*x
        self.nElements += 1
        if x < self.min:
            self.min = x
        if x > self.max:
            self.max = x

    def calculate(self):
        if self.nElements:
            nElements = float(self.nElements)
            average = self.sum / nElements
            self.sigma = math.sqrt(self.sum2/nElements - average*average)
            self.average = average

    def __str__(self):
        return '%s stats: # samples=%s; average=%s; sigma=%s; max=%s; min=%s; last=%s; sum=%s' % (self.name, self.nElements, self.average, self.sigma, self.max, self.min, self.last, self.sum)
       
class SrfbMonitor:

    def __init__(self, nSkip=1):
        self.startTime = None
        self.nReceivedArrays = 0
        self.previousArrayId = 0
        self.nTotalMissedArrays = 0
        self.arraySize = None
        self.maxMissedArrays = 0
        self.missedArraysStats = Stats('Missed Arrays')
        self.nSkip = nSkip


    def process1(self, pv):
        return

    def process2(self, pv):
        floatArray = pv['value'][0]['floatValue']
        return

    def process3(self, pv):
        pv.useNumPyArrays = False
        floatArray = pv['value'][0]['floatValue']
        return

    def process4(self, pv):
        pv.useNumPyArrays = False
        floatArray = pv['value'][0]['floatValue']
        sum = 0
        for value in floatArray:
            sum += value
        return

    def process5(self, pv):
        floatArray = pv['value'][0]['floatValue']
        sum = 0
        for value in floatArray:
            sum += value
        return

    def process6(self, pv):
        floatArray = pv['value'][0]['floatValue']
        sum = floatArray.sum()
        return

    def process(self, pv):
        #arrayId = pv['uniqueId']
        #print 'NUMPY Array id %s sum: %s' % (arrayId, sum)
        self.process6(pv)
        return

    def monitor(self, pv):
        arrayId = pv['uniqueId']
        previousArrayId = self.previousArrayId
        self.previousArrayId = arrayId

        if not self.arraySize:
            self.nElements = len(pv['value'][0]['floatValue'])
            self.arraySize = 4*self.nElements
            print 'NTNDARRAY Size: %s MB, nElements: %s' % (self.arraySize/float(MEGA_BYTE), self.nElements)

        # Processing
        self.process(pv)

        # Do not take statistics first few arrays, start timer when
        # we stop skipping
        if self.nSkip:
            self.nSkip -= 1
            if not self.nSkip:
                self.startTime = time.time()
            return

        # Statistics
        now = time.time()
        deltaT = now-self.startTime
        self.nReceivedArrays += 1
        nMissedArrays = arrayId - previousArrayId - 1
        if nMissedArrays < 0:
            print "INVERSION FOR ARRAY ID: ", arrayId, "; PREVIOUS ARRAY ID: ", previousArrayId
        if nMissedArrays > self.maxMissedArrays:
            self.maxMissedArrays = nMissedArrays
        self.nTotalMissedArrays += nMissedArrays 
        arrayRate = self.nReceivedArrays/deltaT
        dataRate = self.arraySize*self.nReceivedArrays/float(MEGA_BYTE)/deltaT
        averageMissed = self.nTotalMissedArrays/float(self.nReceivedArrays)
        self.missedArraysStats.add(nMissedArrays)
        if self.nReceivedArrays % 100 == 0:
            print 'NTNDARRAY ID %s; Arrays Missed: %s (Average Missed Per Cycle: %s, Max. Missed Per Cycle: %s, Total Missed: %s), Total Received: %s; Array Receiving Rate: %s [Hz], Data Receiving Rate: %s [MB/s]; Runtime: %s [s]' % (arrayId, nMissedArrays, averageMissed, self.maxMissedArrays, self.nTotalMissedArrays, self.nReceivedArrays, arrayRate, dataRate, deltaT) 
            self.missedArraysStats.calculate()
            print self.missedArraysStats
            print

if __name__ == '__main__':
    maxQueueSize = 10
    c = Channel('srfb_ndarray')
    c.setMonitorMaxQueueLength(maxQueueSize)
    for i in range (0,100):
        print "TEST: ", i
        srfbMonitor = SrfbMonitor(10*(maxQueueSize+1))
        c.subscribe('monitor', srfbMonitor.monitor)
        c.startMonitor('')
        time.sleep(10)
        print "Stopping monitor"
        c.stopMonitor()
        print "Unsubscribing monitor"
        c.unsubscribe('monitor')
        print "TEST: ", i, " DONE"
        time.sleep(3)

