#!/usr/bin/env python

import time
import pvaccess as pva
from ..utility.loggingManager import LoggingManager
from ..utility.floatWithUnits import FloatWithUnits

class SourceChannel(pva.Channel):

    STATUS_TYPE_DICT = {
        'producerId' : pva.UINT,
        'channel' : pva.STRING,
        'receiverStats' : {
            'nReceived' : pva.UINT,
            'receivedRate' : pva.DOUBLE,
            'nOverruns' : pva.UINT,
            'overrunRate' : pva.DOUBLE
        },
        'queueStats' : {
            'nReceived' : pva.UINT,
            'nRejected' : pva.UINT,
            'nDelivered' : pva.UINT,
            'nQueued' : pva.UINT
        },
    }

    def __init__(self, channelId, channelName, channelProtocol, serverQueueSize, receiverQueueSize, loggerName, parentObject):
        if not loggerName:
            loggerName = self.__class__.__name__
        self.logger = LoggingManager.getLogger(loggerName)
        self.channelName = channelName
        self.parentObject = parentObject
        self.channelId = channelId
        self.serverQueueSize = serverQueueSize
        self.receiverQueueSize = receiverQueueSize
        self.pvObjectQueue = None
        if receiverQueueSize >= 0:
            self.logger.debug('Source channel %s using receiver queue size %s', self.channelName, receiverQueueSize)
            self.pvObjectQueue = pva.PvObjectQueue(receiverQueueSize)
        pva.Channel.__init__(self, channelName, channelProtocol)
        self.logger.debug('Created source channel %s, protocol %s', self.channelName, channelProtocol)

    def configure(self, configDict):
        if type(configDict) == dict:
            if 'receiverQueueSize' in configDict and self.pvObjectQueue is not None:
                receiverQueueSize = int(configDict.get('receiverQueueSize'))
                if receiverQueueSize >= 0:
                    self.receiverQueueSize = receiverQueueSize
                    self.pvObjectQueue.maxLength = receiverQueueSize
                    self.logger.debug('Source channel receiver queue size is set to %s', receiverQueueSize)

    def getPvMonitorRequest(self):
        recordStr = ''
        if self.serverQueueSize > 0:
            recordStr = f'record[queueSize={self.serverQueueSize}]'
        request = f'{recordStr}field()'
        return request

    def resetStats(self):
        self.resetMonitorCounters()
        if self.pvObjectQueue is not None:
            self.pvObjectQueue.resetCounters()

    def getMonitorStats(self):
        return self.getMonitorCounters()
        
    def getQueueStats(self):
        if self.pvObjectQueue is not None:
            return self.pvObjectQueue.getCounters()
        return {}

    def getStats(self, receivingTime, nReceivedOffset=0):
        receiverStats = self.getMonitorStats()
        queueStats = self.getQueueStats()
        receivedRate = 0
        overrunRate = 0
        nOverruns = receiverStats.get('nOverruns', 0)
        nReceived = receiverStats.get('nReceived', 0)-nReceivedOffset
        if receivingTime > 0 and nReceived >= 0:
            receivedRate = nReceived/receivingTime
            overrunRate = nOverruns/receivingTime
        receiverStats['receivedRate'] = FloatWithUnits(receivedRate, 'Hz')
        receiverStats['overrunRate'] = FloatWithUnits(overrunRate, 'Hz')
        return {'channel' : self.channelName, 'receiverStats' : receiverStats, 'queueStats' : queueStats}

    def process(self, pvObject):
        pass

    # Return true if object was processed, False otherwise
    def processFromQueue(self, waitTime=0):
        return False
        
    def waitOnQueue(self, waitTime):
        if self.pvObjectQueue is not None:
            self.pvObjectQueue.waitForPut(waitTime)
            self.parentObject.setEvent()

    def start(self):
        self.startTime = time.time()
        request = self.getPvMonitorRequest()
        self.logger.debug('Source channel %s using request string %s', self.channelName, request)
        if self.pvObjectQueue is not None:
            self.logger.debug('Starting queue monitor')
            self.qMonitor(self.pvObjectQueue, request)
        else:
            self.logger.debug('Starting processing monitor')
            self.monitor(self.process, request)

    def stop(self):
        self.endTime = time.time()
        self.stopMonitor()
        self.logger.debug('Source channel %s stopped monitor', self.channelName)

class PvaMetadataChannel(SourceChannel):
    def __init__(self, channelId, channelName, serverQueueSize, receiverQueueSize, parentObject):
        loggerName = f'pvaMetadata-{channelId}'
        SourceChannel.__init__(self, channelId, channelName, pva.PVA, serverQueueSize, receiverQueueSize, loggerName, parentObject)

class CaMetadataChannel(SourceChannel):
    def __init__(self, channelId, channelName, serverQueueSize, receiverQueueSize, parentObject):
        loggerName = f'caMetadata-{channelId}'
        SourceChannel.__init__(self, channelId, channelName, pva.CA, serverQueueSize, receiverQueueSize, loggerName, parentObject)

    def getPvMonitorRequest(self):
        recordStr = ''
        if self.serverQueueSize > 0:
            recordStr = f'record[queueSize={self.serverQueueSize}]'
        request = f'{recordStr}field(value,timeStamp)'
        return request

class ProducerChannel(SourceChannel):
    def __init__(self, channelId, channelName, serverQueueSize, receiverQueueSize, objectIdField, fieldRequest, parentObject):
        loggerName = f'producer-{channelId}'
        SourceChannel.__init__(self, channelId, channelName, pva.PVA, serverQueueSize, receiverQueueSize, loggerName, parentObject)
        self.objectIdField = objectIdField
        self.fieldRequest = fieldRequest

    def getPvMonitorRequest(self):
        fieldRequest = ''
        if self.fieldRequest:
            # Strip field() if present
            fieldRequest = self.fieldRequest.replace('field(', '').replace(')', '')
            if self.objectIdField and self.objectIdField not in fieldRequest:
                fieldRequest = f'{self.objectIdField},{fieldRequest}'

        recordStr = ''
        if self.serverQueueSize > 0:
            recordStr = f'record[queueSize={self.serverQueueSize}]'
        request = f'{recordStr}field({fieldRequest})'
        return request

    def process(self, pvObject):
        # We need to copy object coming directly from PVA monitor before we cache it
        objectId = pvObject[self.objectIdField]
        self.parentObject.addObjectToCache(self.channelId, objectId, pvObject.copy())

    # Return true if object was processed, False otherwise
    def processFromQueue(self, waitTime=0):
        if self.pvObjectQueue is not None:
            try:
                pvObject = self.pvObjectQueue.get(waitTime)
                objectId = pvObject[self.objectIdField]
                # We can manipulate object from the queue without having to copy it
                self.parentObject.addObjectToCache(self.channelId, objectId, pvObject)
                return True
            except pva.QueueEmpty as ex:
                # Ignore empty queue
                pass
        return False

