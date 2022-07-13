#!/usr/bin/env python

import time
from ..utility.loggingManager import LoggingManager

# Example for a data processor
class DataProcessor:

    def __init__(self, configDict={}):
        self.configDict = configDict
        # Use data processor id for logging
        self.processorId = configDict.get('processorId', self.__class__.__name__)
        self.logger = LoggingManager.getLogger(f'processor-{self.processorId}')
        self.logger.debug(f'Config dict: {configDict}')

        # Assume NTND Arrays if object id field is not passed in
        self.objectIdField = configDict.get('objectIdField', 'uniqueId')
        # Do not process first object by default
        self.processFirstUpdate = configDict.get('processFirstUpdate', False)
        # Object id processing offset used for statistics calculation
        self.objectIdOffset =int(configDict.get('objectIdOffset', 1))
        self.nProcessed = 0
        self.nMissed = 0
        self.firstObjectId = None
        self.lastObjectId = None
        self.startTime = time.time()
        self.firstObjectTime = 0
        self.lastObjectTime = 0
        self.endTime = 0
        self.processorStats = {}
        self.statsNeedsUpdate = True

    def start(self):
        self.startTime = time.time()

    def stop(self):
        now = time.time()
        self.endTime = now
        self.processorStats = self.updateStats(now)

    def getStats(self):
        if self.statsNeedsUpdate:
            self.processorStats = self.updateStats()
        return self.processorStats

    def updateStats(self, t=0):
        self.statsNeedsUpdate = False
        if not t:
            t = time.time()
        runtime = t-self.startTime
        receivingTime = self.lastObjectTime-self.firstObjectTime
        processedRate = 0
        missedRate = 0
        if receivingTime > 0:
            processedRate = self.nProcessed/receivingTime
            missedRate = self.nMissed/receivingTime
        processorStats = {
            'runtime' : runtime, 
            'startTime' : self.startTime, 
            'endTime' : self.endTime, 
            'receivingTime' : receivingTime,
            'firstObjectTime' : self.firstObjectTime, 
            'lastObjectTime' : self.lastObjectTime, 
            'firstObjectId' : self.firstObjectId, 
            'lastObjectId' : self.lastObjectId, 
            'nProcessed' : self.nProcessed, 
            'processedRate' : processedRate,
            'nMissed' : self.nMissed, 
            'missedRate' : missedRate,
        }
        return processorStats

    def process(self, pvObject):
        now = time.time()
        objectId = pvObject[self.objectIdField]
        if self.lastObjectId is None: 
            self.lastObjectId = objectId
            if not self.processFirstUpdate:
                return None
        if self.firstObjectId is None:
            self.firstObjectId = objectId
            self.firstObjectTime = now
            self.lastObjectId = objectId
        self.nProcessed += 1
        nMissed = objectId-self.lastObjectId-self.objectIdOffset
        if nMissed > 0:
            self.nMissed += nMissed
        self.lastObjectId = objectId
        self.lastObjectTime = now
        self.statsNeedsUpdate = True
        return pvObject
