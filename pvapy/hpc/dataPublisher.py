#!/usr/bin/env python

'''
Data publisher module.
'''

from ..utility.loggingManager import LoggingManager

class DataPublisher:
    ''' Data publisher class. '''

    def __init__(self, outputChannel):
        self.logger = LoggingManager.getLogger(f'{self.__class__.__name__}-{outputChannel}')
        self.outputChannel = outputChannel
        self.nPublished = 0
        self.nErrors = 0

    def resetStats(self):
        self.nPublished = 0
        self.nErrors = 0

    def getStats(self):
        return {'nPublished' : self.nPublished, 'nErrors' : self.nErrors}

    def start(self):
        pass

    def updateOutputChannel(self, pvObject):
        pass

    def stop(self):
        pass

