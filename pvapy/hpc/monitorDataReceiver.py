#!/usr/bin/env python

'''
Monitor data receiver module.
'''

import copy
import pvaccess as pva
from ..utility.statsUtility import StatsUtility
from .dataReceiver import DataReceiver

class MonitorDataReceiver(DataReceiver):
    ''' Monitor data receiver class. '''

    def __init__(self, inputChannel, processingFunction, pvObjectQueue=None, pvRequest='', providerType=pva.PVA):
        DataReceiver.__init__(self, inputChannel, processingFunction)
        self.logger.debug('Channel %s provider type: %s', inputChannel, providerType)
        self.inputChannel = inputChannel
        self.providerType = providerType
        self.channel = pva.Channel(self.inputChannel, self.providerType)
        self.zeroStats = self.channel.getMonitorCounters()
        self.savedStats = copy.copy(self.zeroStats)
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
        if self.channel:
            self.channel.resetMonitorCounters()
        self.savedStats = copy.copy(self.zeroStats)

    def getStats(self):
        currentStats = {}
        if self.channel:
            currentStats = self.channel.getMonitorCounters()
        return StatsUtility.addKeyValues(self.savedStats, currentStats)

    def start(self):
        if not DataReceiver.start(self):
            self.logger.debug('Monitor already running')
            return False
        if not self.channel:
            self.channel = pva.Channel(self.inputChannel, self.providerType)
        self.logger.debug('Using request string: %s', self.pvRequest)
        if self.pvObjectQueue is not None:
            self.logger.debug('Starting queue monitor')
            self.channel.qMonitor(self.pvObjectQueue, self.pvRequest)
        else:
            self.logger.debug('Starting processing monitor')
            self.channel.monitor(self.process, self.pvRequest)
        return True

    def stop(self):
        if not DataReceiver.stop(self):
            self.logger.debug('Monitor already stopped')
            return False
        self.logger.debug('Stopping monitor')
        self.channel.stopMonitor()
        self.savedStats = self.getStats()
        # Force disconnect
        self.channel = None
        return True
