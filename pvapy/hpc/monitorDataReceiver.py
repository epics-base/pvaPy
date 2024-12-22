#!/usr/bin/env python

'''
Monitor data receiver module.
'''

import pvaccess as pva
from .dataReceiver import DataReceiver

class MonitorDataReceiver(DataReceiver):
    ''' Monitor data receiver class. '''

    def __init__(self, inputChannel, processingFunction, pvObjectQueue=None, pvRequest='', providerType=pva.PVA):
        DataReceiver.__init__(self, inputChannel, processingFunction)
        self.logger.debug('Channel %s provider type: %s', inputChannel, providerType)
        self.channel = pva.Channel(inputChannel, providerType)
        self.pvRequest = pvRequest
        self.pvObjectQueue = pvObjectQueue
        if self.pvObjectQueue is not None:
            self.logger.debug('Using PvObjectQueue of length %s', self.pvObjectQueue.maxLength)
        else:
            self.logger.debug('Not using PvObjectQueue')
        self.logger.debug('Created monitor data receiver for input channel %s', inputChannel)

    def process(self, pv):
        return self.processingFunction(pv)

    def resetStats(self):
        self.channel.resetMonitorCounters()

    def getStats(self):
        return self.channel.getMonitorCounters()

    def start(self):
        self.logger.debug('Using request string: %s', self.pvRequest)
        if self.pvObjectQueue is not None:
            self.logger.debug('Starting queue monitor')
            self.channel.qMonitor(self.pvObjectQueue, self.pvRequest)
        else:
            self.logger.debug('Starting processing monitor')
            self.channel.monitor(self.process, self.pvRequest)

    def stop(self):
        self.channel.stopMonitor()
