#!/usr/bin/env python

'''
Data receiver module.
'''

import time
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
        self.startTime = 0
        self.endTime = 0
        self.runtime = 0
        self.running = False

    def process(self, pv):
        return self.processingFunction(pv)

    def resetStats(self):
        self.nReceived = 0
        self.nRejected = 0
        self.nErrors = 0

    def getStats(self):
        return {'nReceived' : self.nReceived, 'nRejected' : self.nRejected, 'nErrors' : self.nErrors}

    def getStartTime(self):
        return self.startTime

    def isRunning(self):
        return self.running

    # Return false if receiver is running already
    def start(self):
        if self.running:
            return False
        self.running = True
        self.startTime = time.time()
        self.endTime = 0
        return True

    # Return false if receiver is stopped already
    def stop(self):
        if not self.running:
            return False
        self.running = False
        self.endTime = time.time()
        self.runtime += self.endTime - self.startTime
        return True
