#!/usr/bin/env python

import time
import pvaccess as pva
from ..utility.loggingManager import LoggingManager

# Base data processor class
class DataProcessor:

    def __init__(self, configDict={}):
        self.configDict = configDict
        # Use data processor id for logging
        self.processorId = configDict.get('processorId', 0)
        self.logger = LoggingManager.getLogger(f'processor-{self.processorId}')
        self.logger.debug(f'Config dict: {configDict}')

        # Assume NTND Arrays if object id field is not passed in
        self.objectIdField = configDict.get('objectIdField', 'uniqueId')
        # Do not process first object by default
        self.processFirstUpdate = configDict.get('processFirstUpdate', False)
        # Object id processing offset used for statistics calculation
        self.objectIdOffset = int(configDict.get('objectIdOffset', 1))
        # Output channel is used for publishing processed objects
        self.inputChannel = configDict.get('inputChannel', '')
        self.outputChannel = configDict.get('outputChannel', '')
        if self.outputChannel == '_':
            self.outputChannel = f'{self.inputChannel}:processor-{self.processorId}'
        self.outputRecordAdded = False
        self.pvaServer = None

        # Defines all counters and sets them to zero
        self.resetStats()

    # Interface method called at startup
    def start(self):
        pass

    def _start(self):
        if self.outputChannel:
            self.pvaServer = pva.PvaServer()
            self.pvaServer.start()
        self.startTime = time.time()
        self.start()

    # Interface method called at shutdown
    def stop(self):
        pass

    def _stop(self):
        now = time.time()
        self.endTime = now
        self.processorStats = self.updateStats(now)
        if self.outputChannel:
            self.pvaServer.stop()
        self.stop()

    # Interface method called at configuration
    def configure(self, kwargs):
        pass

    def _configure(self, kwargs):
        if type(kwargs) == dict:
            if 'processFirstUpdate' in kwargs: 
                self.processFirstUpdate = kwargs.get('processFirstUpdate')
                self.logger.debug(f'Resetting processing of first update to {self.processFirstUpdate}')
            if 'objectIdOffset' in kwargs: 
                self.objectIdOffset = int(configDict.get('objectIdOffset', 1))
                self.logger.debug(f'Resetting object id offset to {self.objectIdOffset}')
        self.configure(kwargs)

    # Interface method called at processing channel updates
    def process(self, pvObject):
        return pvObject

    def _process(self, pvObject):
        now = time.time()
        objectId = pvObject[self.objectIdField]
        if self.lastObjectId is None: 
            self.lastObjectId = objectId
            if self.outputChannel and not self.outputRecordAdded:
                self.outputRecordAdded = True
                self.pvaServer.addRecord(self.outputChannel, pvObject.copy())
                self.logger.debug(f'Added output channel {self.outputChannel}')
            if not self.processFirstUpdate:
                return None
        if self.firstObjectId is None:
            self.firstObjectId = objectId
            self.firstObjectTime = now
            self.lastObjectId = objectId
        nMissed = objectId-self.lastObjectId-self.objectIdOffset
        if nMissed > 0:
            self.nMissed += nMissed
        self.lastObjectId = objectId
        self.lastObjectTime = now
        self.statsNeedsUpdate = True
        try:
            pvObject2 = self.process(pvObject)
            self.nProcessed += 1
            return pvObject2
        except Exception as ex:
            self.nErrors += 1
            raise

    def resetStats(self):
        self.nProcessed = 0
        self.nMissed = 0
        self.nErrors = 0
        self.firstObjectId = None
        self.lastObjectId = None
        self.startTime = time.time()
        self.firstObjectTime = 0
        self.lastObjectTime = 0
        self.endTime = 0
        self.processorStats = {}
        self.statsNeedsUpdate = True

    def getStats(self):
        if self.statsNeedsUpdate:
            self.processorStats = self.updateStats()
        else:
            runtime = time.time()-self.startTime
            self.processorStats['runtime'] = runtime
        return self.processorStats

    def updateStats(self, t=0):
        self.statsNeedsUpdate = False
        if not t:
            t = time.time()
        runtime = t-self.startTime
        receivingTime = self.lastObjectTime-self.firstObjectTime
        processedRate = 0
        missedRate = 0
        errorRate = 0
        if receivingTime > 0:
            processedRate = self.nProcessed/receivingTime
            missedRate = self.nMissed/receivingTime
            errorRate = self.nErrors/receivingTime
        processorStats = {
            'runtime' : runtime, 
            'startTime' : self.startTime, 
            'endTime' : self.endTime, 
            'receivingTime' : receivingTime,
            'firstObjectTime' : self.firstObjectTime, 
            'lastObjectTime' : self.lastObjectTime, 
            'firstObjectId' : self.firstObjectId or 0, 
            'lastObjectId' : self.lastObjectId or 0, 
            'nProcessed' : self.nProcessed, 
            'processedRate' : processedRate,
            'nMissed' : self.nMissed, 
            'missedRate' : missedRate,
            'nErrors' : self.nErrors, 
            'errorRate' : errorRate,
        }
        return processorStats

    def updateOutputChannel(self, pvObject):
        if not self.pvaServer:
            return 
        self.pvaServer.update(pvObject)

