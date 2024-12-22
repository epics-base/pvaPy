#!/usr/bin/env python

'''
PVAS data receiver module.
'''

import pvaccess as pva
from .dataReceiver import DataReceiver

class PvasDataReceiver(DataReceiver):
    ''' PVAS data receiver class. '''

    def __init__(self, inputChannel, processingFunction, pvaServer, inputPvObject, pvObjectQueue=None):
        DataReceiver.__init__(self, inputChannel, processingFunction)
        self.pvaServer = pvaServer
        self.inputPvObject = inputPvObject.copy()
        self.pvaServer.addRecord(self.inputChannel, self.inputPvObject, self.process)
        self.pvObjectQueue = pvObjectQueue
        if self.pvObjectQueue is not None:
            self.logger.debug('Using PvObjectQueue of length %s', self.pvObjectQueue.maxLength)
        else:
            self.logger.debug('Not using PvObjectQueue')
        self.logger.debug('Created PVAS data receiver for input channel %s', inputChannel)

    def process(self, pv):
        try:
            self.nReceived += 1
            if self.pvObjectQueue is not None:
                try:
                    self.pvObjectQueue.put(pv)
                except pva.QueueFull:           
                    self.logger.error('Input PV object queue is full.')
                    self.nRejected += 1
                return
            else:
                self.processingFunction(pv)
        except Exception as ex:           
            self.nErrors += 1
            self.logger.error('Error processing input PV object: %s', str(ex))
