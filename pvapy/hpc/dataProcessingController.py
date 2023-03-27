#!/usr/bin/env python
'''
Data processing controller module.
'''

import time
import pvaccess as pva
from ..utility.loggingManager import LoggingManager
from ..utility.floatWithUnits import FloatWithUnits

class DataProcessingController:
    ''' Data processor controller class. '''

    def __init__(self, configDict={}, userDataProcessor=None):
        self.configDict = configDict

        # Use data processor id for logging
        self.processorId = configDict.get('processorId', 0)
        self.logger = LoggingManager.getLogger(f'processor-{self.processorId}')
        self.logger.debug('Config dict: %s', configDict)

        self.userDataProcessor = userDataProcessor
        self.logger.debug('User data processor: %s', userDataProcessor)

        # Assume NTND Arrays if object id field is not passed in
        self.objectIdField = configDict.get('objectIdField', 'uniqueId')
        # Do not process first object by default
        self.skipInitialUpdates = configDict.get('skipInitialUpdates', 1)
        # Object id processing offset used for statistics calculation
        self.objectIdOffset = int(configDict.get('objectIdOffset', 1))
        # Number of sequential updates is used for statistics calculation
        self.nSequentialUpdates = int(configDict.get('nSequentialUpdates', 1))
        # Output channel is used for publishing processed objects
        self.inputChannel = configDict.get('inputChannel', '')
        self.outputChannel = configDict.get('outputChannel', '')
        if self.outputChannel == '_':
            self.outputChannel = f'{self.inputChannel}:processor-{self.processorId}'
        self.outputRecordAdded = False
        self.pvaServerStarted = False
        self.pvaServer = None

        # Defines all counters and sets them to zero
        self.resetStats()

    def setPvaServer(self, pvaServer):
        self.pvaServer = pvaServer

    def addUserDefinedOutputRecord(self, pvObject):
        # Create output channel if user processing class defines it
        # Input pvObject will come from the first channel update
        if self.userDataProcessor:
            outputPvObject = self.userDataProcessor.getOutputPvObjectType(pvObject)
            if outputPvObject:
                self.logger.debug('User data processor defined output channel as: %s', outputPvObject.getStructureDict())
                self.addOutputRecordIfItDoesNotExist(outputPvObject)
            else:
                self.logger.debug('User data processor did not define output channel')

    def addOutputRecordIfItDoesNotExist(self, pvObject):
        if self.outputChannel and self.pvaServer and not self.outputRecordAdded:
            self.outputRecordAdded = True
            self.pvaServer.addRecord(self.outputChannel, pvObject.copy(), None)
            self.logger.debug('Added output channel %s', self.outputChannel)

    def start(self):
        if self.outputChannel and not self.pvaServer:
            self.logger.debug('Starting pva server')
            self.pvaServerStarted = True
            self.pvaServer = pva.PvaServer()
            self.pvaServer.start()
        self.startTime = time.time()
        # Call user interface method for startup
        if self.userDataProcessor:
            self.userDataProcessor.pvaServer = self.pvaServer
            self.userDataProcessor.outputChannel = self.outputChannel
            self.userDataProcessor.inputChannel = self.inputChannel
            self.userDataProcessor.start()

    def stop(self):
        now = time.time()
        self.endTime = now
        self.processorStats = self.updateStats(now)
        if self.pvaServerStarted:
            self.pvaServer.stop()
        # Call user interface method for shutdown
        if self.userDataProcessor:
            self.userDataProcessor.stop()

    def configure(self, configDict):
        if isinstance(configDict, dict):
            if 'skipInitialUpdates' in configDict:
                self.skipInitialUpdates = int(configDict.get('skipInitialUpdates'))
                self.logger.debug('Resetting processing of first update to %s', self.skipInitialUpdates)
            if 'objectIdOffset' in configDict:
                self.objectIdOffset = int(configDict.get('objectIdOffset', 1))
                self.logger.debug('Resetting object id offset to %s', self.objectIdOffset)
            if 'nSequentialUpdates' in configDict:
                self.nSequentialUpdates = int(configDict.get('nSequentialUpdates', 1))
                self.logger.debug('Resetting number of sequential updates to %s', self.nSequentialUpdates)
        # Call user interface method for configuration
        if self.userDataProcessor:
            self.userDataProcessor.configure(configDict)

    def process(self, pvObject):
        now = time.time()
        objectId = pvObject[self.objectIdField]
        if self.lastObjectId is None:
            self.lastObjectId = objectId

            # First try to create user defined output record, and
            # if that does not succeed, create output record based
            # on input type
            self.addUserDefinedOutputRecord(pvObject)
            self.addOutputRecordIfItDoesNotExist(pvObject)
        if self.skipInitialUpdates > 0:
            self.skipInitialUpdates -= 1
            self.logger.debug('Skipping initial update, %s remain to be skipped', self.skipInitialUpdates)
            return None
        if self.firstObjectId is None:
            self.firstObjectId = objectId
            self.firstObjectTime = now
            self.lastObjectId = objectId
            self.lastExpectedGroupUpdateId = objectId + self.nSequentialUpdates - 1

        # Calculate number of missed objects
        nMissed = 0
        if self.nSequentialUpdates > 1:
            # More than one sequential update
            # Example:
            #   4 consumers, 3 sequential updates: id offset 10
            #   Consumer 1: ( 1, 2, 3) (13,14,15) (25,26,27) (37,38,39)
            #   Consumer 2: ( 4, 5, 6) (16,17,18) (28,29,30) (40,41,42)
            #   Consumer 3: ( 7, 8, 9) (19,20,21) (31,32,33) (43,44,45)
            #   Consumer 4: (10,11,12) (22,23,24) (34,35,36) (46,47,48)
            idDiff = objectId-self.lastObjectId
            nOffsets = (idDiff+self.nSequentialUpdates-1) // (self.objectIdOffset+self.nSequentialUpdates-1)
            if idDiff > 1:
                # Potential miss
                lastExpectedCurrentGroupUpdateId = self.lastExpectedGroupUpdateId
                nMissedInCurrentGroup = min(objectId,lastExpectedCurrentGroupUpdateId) - self.lastObjectId - 1
                nGroupsMissed = 0
                nMissedInNewGroup = 0
                firstExpectedNewGroupUpdateId = 0
                lastExpectedNewGroupUpdateId = 0
                if nOffsets > 0:
                    nGroupsMissed = nOffsets - 1
                    nMissedInCurrentGroup = lastExpectedCurrentGroupUpdateId - self.lastObjectId
                    firstExpectedNewGroupUpdateId = lastExpectedCurrentGroupUpdateId + nOffsets*(self.objectIdOffset-1) + nGroupsMissed*self.nSequentialUpdates + 1
                    self.lastExpectedGroupUpdateId = firstExpectedNewGroupUpdateId + self.nSequentialUpdates - 1
                    lastExpectedNewGroupUpdateId = self.lastExpectedGroupUpdateId
                    nMissedInNewGroup = objectId - firstExpectedNewGroupUpdateId
                nMissed = nMissedInCurrentGroup + nGroupsMissed*self.nSequentialUpdates + nMissedInNewGroup
                if nMissed > 0:
                    self.logger.debug('Missed %s objects at id %s (nOffsets: %s, nGroupsMissed: %s, nMissedInCurrentGroup: %s, nMissedInNewGroup: %s, lastExpectedCurrentGroupUpdateId: %s, firstExpectedNewGroupUpdateId: %s, lastExpectedNewGroupUpdateId: %s', nMissed, objectId, nOffsets, nGroupsMissed, nMissedInCurrentGroup, nMissedInNewGroup, lastExpectedCurrentGroupUpdateId, firstExpectedNewGroupUpdateId, lastExpectedNewGroupUpdateId)
        else:
            # Single sequential update
            # Example:
            #   4 consumers, 1 update: id offset is 4
            #   Consumer 1: (1) (5) ( 9)
            #   Consumer 2: (2) (6) (10)
            #   Consumer 3: (3) (7) (11)
            #   Consumer 4: (4) (8) (12)
            nMissed = (objectId-self.lastObjectId-self.objectIdOffset) // self.objectIdOffset
        if nMissed > 0:
            self.nMissed += nMissed
        self.lastObjectId = objectId
        self.lastObjectTime = now
        self.statsNeedsUpdate = True
        try:
            # Call user interface method for processing
            if self.userDataProcessor:
                pvObject2 = self.userDataProcessor.process(pvObject)
            else:
                pvObject2 = pvObject
            self.nProcessed += 1
            return pvObject2
        except Exception:
            self.nErrors += 1
            raise

    def resetStats(self):
        self.nProcessed = 0
        self.nMissed = 0
        self.nErrors = 0
        self.firstObjectId = None
        self.lastObjectId = None
        self.lastExpectedGroupUpdateId = None
        self.startTime = time.time()
        self.firstObjectTime = 0
        self.lastObjectTime = 0
        self.endTime = 0
        self.processorStats = {}
        self.statsNeedsUpdate = True
        # Call user interface method for resetting stats
        if self.userDataProcessor:
            self.userDataProcessor.resetStats()

    def getUserStats(self):
        # Call user interface for retrieving stats
        if self.userDataProcessor:
            return self.userDataProcessor.getStats()
        return {}

    def getUserStatsPvaTypes(self):
        # Call user interface for retrieving stats PVA types
        if self.userDataProcessor:
            return self.userDataProcessor.getStatsPvaTypes()
        return {}

    def getProcessorStats(self):
        if self.statsNeedsUpdate:
            self.processorStats = self.updateStats()
        else:
            runtime = time.time()-self.startTime
            self.processorStats['runtime'] = FloatWithUnits(runtime, 's')
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
            'runtime' : FloatWithUnits(runtime, 's'),
            'startTime' : FloatWithUnits(self.startTime, 's'),
            'endTime' : FloatWithUnits(self.endTime, 's'),
            'receivingTime' : FloatWithUnits(receivingTime, 's'),
            'firstObjectTime' : FloatWithUnits(self.firstObjectTime, 's'),
            'lastObjectTime' : FloatWithUnits(self.lastObjectTime, 's'),
            'firstObjectId' : self.firstObjectId or 0,
            'lastObjectId' : self.lastObjectId or 0,
            'nProcessed' : self.nProcessed,
            'processedRate' : FloatWithUnits(processedRate, 'Hz'),
            'nMissed' : self.nMissed,
            'missedRate' : FloatWithUnits(missedRate, 'Hz'),
            'nErrors' : self.nErrors,
            'errorRate' : FloatWithUnits(errorRate, 'Hz')
        }
        return processorStats

    def updateOutputChannel(self, pvObject):
        if not self.outputChannel:
            return
        self.pvaServer.update(self.outputChannel, pvObject)
