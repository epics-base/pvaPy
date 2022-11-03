#!/usr/bin/env python

import time
import pvaccess as pva
from ..utility.loggingManager import LoggingManager
from ..utility.floatWithUnits import FloatWithUnits

# Data processor contrller class
class DataProcessingController:

    def __init__(self, configDict={}, userDataProcessor=None):
        self.configDict = configDict

        # Use data processor id for logging
        self.processorId = configDict.get('processorId', 0)
        self.logger = LoggingManager.getLogger(f'processor-{self.processorId}')
        self.logger.debug(f'Config dict: {configDict}')

        self.userDataProcessor = userDataProcessor
        self.logger.debug(f'User data processor: {userDataProcessor}')

        # Assume NTND Arrays if object id field is not passed in
        self.objectIdField = configDict.get('objectIdField', 'uniqueId')
        # Do not process first object by default
        self.skipInitialUpdates = configDict.get('skipInitialUpdates', 1)
        # Object id processing offset used for statistics calculation
        self.objectIdOffset = int(configDict.get('objectIdOffset', 1))
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
                self.logger.debug(f'User data processor defined output channel as: {outputPvObject.getStructureDict()}')
                self.addOutputRecordIfItDoesNotExist(outputPvObject)
            else:
                self.logger.debug('User data processor did not define output channel')
                
    def addOutputRecordIfItDoesNotExist(self, pvObject):
        if self.outputChannel and self.pvaServer and not self.outputRecordAdded:
            self.outputRecordAdded = True
            self.pvaServer.addRecord(self.outputChannel, pvObject.copy(), None)
            self.logger.debug(f'Added output channel {self.outputChannel}')

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
        if type(configDict) == dict:
            if 'skipInitialUpdates' in configDict: 
                self.skipInitialUpdates = int(configDict.get('skipInitialUpdates'))
                self.logger.debug(f'Resetting processing of first update to {self.skipInitialUpdates}')
            if 'objectIdOffset' in configDict: 
                self.objectIdOffset = int(configDict.get('objectIdOffset', 1))
                self.logger.debug(f'Resetting object id offset to {self.objectIdOffset}')
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
            self.logger.debug(f'Skipping initial update, {self.skipInitialUpdates} remain to be skipped')
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
            # Call user interface method for processing
            if self.userDataProcessor:
                pvObject2 = self.userDataProcessor.process(pvObject)
            else:
                pvObject2 = pvObject
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

