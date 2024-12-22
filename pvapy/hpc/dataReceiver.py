#!/usr/bin/env python

'''
Data receiver module.
'''

from ..utility.loggingManager import LoggingManager

class DataReceiver:
    ''' Data receiver class. '''

    def __init__(self, inputChannel, processingFunction):
        self.logger = LoggingManager.getLogger(f'{self.__class__.__name__}-{inputChannel}')
        self.inputChannel = inputChannel
        self.processingFunction = processingFunction
        self.nReceived = 0
        self.nRejected = 0
        self.nErrors = 0

    def process(self, pv):
        return self.processingFunction(pv)

    def resetStats(self):
        self.nReceived = 0
        self.nRejected = 0
        self.nErrors = 0

    def getStats(self):
        return {'nReceived' : self.nReceived, 'nRejected' : self.nRejected, 'nErrors' : self.nErrors}

    def start(self):
        pass

    def stop(self):
        pass
