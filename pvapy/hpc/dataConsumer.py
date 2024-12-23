#!/usr/bin/env python

'''
Data consumer module.
'''

import time
import pvaccess as pva
from .monitorDataReceiver import MonitorDataReceiver
from .pvasDataReceiver import PvasDataReceiver
from .rpcsDataReceiver import RpcsDataReceiver
from .metadataChannelFactory import MetadataChannelFactory
from ..utility.loggingManager import LoggingManager
from ..utility.floatWithUnits import FloatWithUnits
from ..utility.operationMode import OperationMode

class DataConsumer:
    ''' Data consumer class. '''

    PROVIDER_TYPE_MAP = { 'pva' : pva.PVA, 'ca' : pva.CA }

    STATUS_TYPE_DICT = {
        'consumerId' : pva.UINT,
        'inputChannel' : pva.STRING,
        'objectId' : pva.UINT,
        'objectTime' : pva.DOUBLE,
        'objectTimestamp' : pva.PvTimeStamp(),
        'receiverStats' : {
            'nReceived' : pva.UINT,
            'receivedRate' : pva.DOUBLE,
            'nRejected' : pva.UINT,
            'rejectedRate' : pva.DOUBLE,
            'nErrors' : pva.UINT,
            'errorRate' : pva.DOUBLE,
            'nOverruns' : pva.UINT,
            'overrunRate' : pva.DOUBLE
        },
        'publisherStats' : {
            'nPublished' : pva.UINT,
            'publishedRate' : pva.DOUBLE,
            'nErrors' : pva.UINT,
            'errorRate' : pva.DOUBLE
        },
        'queueStats' : {
            'nReceived' : pva.UINT,
            'nRejected' : pva.UINT,
            'nDelivered' : pva.UINT,
            'nQueued' : pva.UINT
        },
        'processorStats' : {
            'runtime' : pva.DOUBLE,
            'startTime' : pva.DOUBLE,
            'endTime' : pva.DOUBLE,
            'receivingTime' : pva.DOUBLE,
            'firstObjectTime' : pva.DOUBLE,
            'lastObjectTime' : pva.DOUBLE,
            'firstObjectId' : pva.UINT,
            'lastObjectId' : pva.UINT,
            'nProcessed' : pva.UINT,
            'processedRate' : pva.DOUBLE,
            'nErrors' : pva.UINT,
            'errorRate' : pva.DOUBLE,
            'nMissed' : pva.UINT,
            'missedRate' : pva.DOUBLE
        }
    }

    def __init__(self, consumerId, inputChannel, inputMode=OperationMode.PVA, objectIdField='uniqueId', fieldRequest='', serverQueueSize=-1, receiverQueueSize=-1, accumulateObjects=-1, accumulationTimeout=-1, distributorPluginName='pydistributor', distributorGroupId=None, distributorSetId=None, distributorTriggerFieldName=None, distributorUpdates=None, distributorUpdateMode=None, metadataChannels=None, processingController=None):
        self.logger = LoggingManager.getLogger(f'consumer-{consumerId}')
        self.consumerId = consumerId
        self.inputChannel = inputChannel
        self.inputMode = inputMode
        self.serverQueueSize = serverQueueSize
        self.logger.debug('Server queue size: %s', serverQueueSize)
        self.distributorPluginName = distributorPluginName
        self.distributorGroupId = distributorGroupId
        self.distributorSetId = distributorSetId
        self.distributorTriggerFieldName = distributorTriggerFieldName
        self.distributorUpdates = distributorUpdates
        self.distributorUpdateMode = distributorUpdateMode
        self.objectIdField = objectIdField
        self.fieldRequest = fieldRequest
        self.pvObjectQueue = None
        self.receiverQueueSize = receiverQueueSize
        if receiverQueueSize >= 0:
            self.pvObjectQueue = pva.PvObjectQueue(receiverQueueSize)
            self.logger.debug('Using PvObjectQueue of size: %s', receiverQueueSize)
        self.accumulateObjects = accumulateObjects
        self.accumulationTimeout = accumulationTimeout
        self.logger.debug('Will accumulate %s objects before processing, with accumulation timeout of %s', self.accumulateObjects, self.accumulationTimeout)

        self.processingController = processingController
        self.startTime = None
        self.endTime = None

        # If first object is ignored, stats will be adjusted
        self.nReceivedOffset = 0
        if self.processingController and hasattr(self.processingController, 'ignoreFirstObject') and self.processingController.ignoreFirstObject:
            self.nReceivedOffset = 1

        # Metadata channels
        self.metadataChannelMap, self.metadataQueueMap = MetadataChannelFactory.createMetadataChannels(metadataChannels, serverQueueSize, receiverQueueSize, self)

        if self.processingController and self.processingController.userDataProcessor:
            self.processingController.userDataProcessor.metadataQueueMap = self.metadataQueueMap

        self.dataReceiver = self.createDataReceiver()
        self.logger.debug('Created data consumer %s', consumerId)

    def createDataReceiver(self):
        self.logger.debug('Creating data source, input mode %s', self.inputMode)
        if self.inputMode == OperationMode.PVA:
            providerType = pva.PVA
            return MonitorDataReceiver(inputChannel=self.inputChannel, processingFunction=self.process, pvObjectQueue=self.pvObjectQueue, pvRequest=self.getPvMonitorRequest(), providerType=providerType)
        elif self.inputMode == OperationMode.CA:
            providerType = pva.CA
            return MonitorDataReceiver(inputChannel=self.inputChannel, processingFunction=self.process, pvObjectQueue=self.pvObjectQueue, pvRequest=self.getPvMonitorRequest(), providerType=providerType)
        elif self.inputMode == OperationMode.PVAS:
            inputPvObject = self.processingController.getUserInputPvObjectType()
            if not inputPvObject:
                raise pva.InvalidState('User input PvObject type is not defined.')
            pvaServer = self.processingController.pvaServer
            pvObjectQueue=self.pvObjectQueue
            return PvasDataReceiver(inputChannel=self.inputChannel, processingFunction=self.process, pvaServer=pvaServer, inputPvObject=inputPvObject, pvObjectQueue=pvObjectQueue)
        elif self.inputMode == OperationMode.RPCS:
            pvaServer = self.processingController.pvaServer
            pvObjectQueue=self.pvObjectQueue
            return RpcsDataReceiver(inputChannel=self.inputChannel, processingFunction=self.process, pvaServer=pvaServer, pvObjectQueue=pvObjectQueue)
        else:
            raise pva.InvalidState(f'Unsupported input mode: {self.inputMode}')

    def getPvMonitorRequest(self):
        recordStr = ''
        if self.serverQueueSize > 0:
            recordStr = f'record[queueSize={self.serverQueueSize}]'
        distributorStr = ''
        if self.distributorGroupId \
                or self.distributorSetId \
                or self.distributorTriggerFieldName \
                or self.distributorUpdates \
                or self.distributorUpdateMode:
            distributorStr = f'_[{self.distributorPluginName}='
        if self.distributorGroupId:
            distributorStr += f'group:{self.distributorGroupId};'
        if self.distributorSetId:
            distributorStr += f'set:{self.distributorSetId};'
        if self.distributorTriggerFieldName:
            distributorStr += f'trigger:{self.distributorTriggerFieldName};'
        if self.distributorUpdates:
            distributorStr += f'updates:{self.distributorUpdates};'
        if self.distributorUpdateMode:
            distributorStr += f'mode:{self.distributorUpdateMode};'

        fieldRequest = ''
        if self.fieldRequest:
            # Strip field() if present
            fieldRequest = self.fieldRequest.replace('field(', '').replace(')', '')
            if self.objectIdField and self.objectIdField not in fieldRequest:
                fieldRequest = f'{self.objectIdField},{fieldRequest}'

        request = f'{recordStr}field({fieldRequest})'
        if distributorStr:
            # Strip last ';' character
            distributorStr = distributorStr[0:-1] + ']'
            request = f'{recordStr}field({distributorStr})'
        return request

    def configure(self, configDict):
        if isinstance(configDict, dict):
            if 'receiverQueueSize' in configDict:
                receiverQueueSize = int(configDict.get('receiverQueueSize'))
                if self.pvObjectQueue is not None:
                    self.logger.debug('Resetting PvObjectQueue size from %s to %s', self.pvObjectQueue.maxLength, receiverQueueSize)
                    self.pvObjectQueue.maxLength = receiverQueueSize
                    self.receiverQueueSize = receiverQueueSize
        if self.processingController:
            self.processingController.configure(configDict)

    def process(self, pv):
        if self.processingController:
            self.processingController.process(pv)

    # Return true if object was processed, False otherwise
    def processFromQueue(self, waitTime):
        if self.pvObjectQueue is None:
            return False
        # If we are accumulating objects before processing,
        # we also have to make sure timout did not occur
        if self.accumulateObjects > 0:
            if len(self.pvObjectQueue) < self.accumulateObjects:
                timeSinceLastPut = self.pvObjectQueue.getTimeSinceLastPut()
                if self.accumulationTimeout > timeSinceLastPut:
                    self.logger.debug('Accumulation timeout did not occur yet (last put was %s seconds ago', timeSinceLastPut)
                    return False
        try:
            pvObject = self.pvObjectQueue.get(waitTime)
            self.process(pvObject)
            return True
        except pva.QueueEmpty:
            # Ignore empty queue
            pass
        return False

    def resetStats(self):
        self.dataReceiver.resetStats()
        if self.pvObjectQueue is not None:
            self.pvObjectQueue.resetCounters()
        if self.processingController:
            self.processingController.resetStats()

    def getDataReceiverStats(self, receivingTime):
        receiverStats = self.dataReceiver.getStats()
        nReceived = receiverStats.get('nReceived', 0)-self.nReceivedOffset
        nRejected = receiverStats.get('nRejected', 0)
        nErrors = receiverStats.get('nErrors', 0)
        nOverruns = receiverStats.get('nOverruns', 0)
        receivedRate = 0
        rejectedRate = 0
        errorRate = 0
        overrunRate = 0
        if receivingTime > 0:
            if nReceived > 0:
                receivedRate = nReceived/receivingTime
            if nRejected > 0:
                rejectedRate = nRejected/receivingTime
            if nErrors > 0:
                errorRate = nErrors/receivingTime
            if nOverruns > 0:
                overrunRate = nOverruns/receivingTime
        receiverStats['receivedRate'] = FloatWithUnits(receivedRate, 'Hz')
        receiverStats['rejectedRate'] = FloatWithUnits(rejectedRate, 'Hz')
        receiverStats['errorRate'] = FloatWithUnits(errorRate, 'Hz')
        receiverStats['overrunRate'] = FloatWithUnits(overrunRate, 'Hz')
        return receiverStats

    def getDataPublisherStats(self):
        return self.processingController.getPublisherStats()

    def getQueueStats(self):
        if self.pvObjectQueue is not None:
            return self.pvObjectQueue.getCounters()
        return {}

    def getProcessorStats(self):
        if self.processingController:
            return self.processingController.getProcessorStats()
        return {}

    def getUserStats(self):
        if self.processingController:
            return self.processingController.getUserStats()
        return {}

    def getStats(self):
        processorStats = self.getProcessorStats()
        receivingTime = processorStats.get('receivingTime', 0)
        receiverStats = self.getDataReceiverStats(receivingTime)
        publisherStats = self.getDataPublisherStats()
        queueStats = self.getQueueStats()
        userStats = self.getUserStats()
        metadataStats = {}
        for metadataChannelId,metadataChannel in self.metadataChannelMap.items():
            metadataStats[f'metadata-{metadataChannelId}'] = metadataChannel.getStats(receivingTime)

        return {'inputChannel' : self.inputChannel, 'receiverStats' : receiverStats, 'publisherStats' : publisherStats, 'queueStats' : queueStats, 'metadataStats' : metadataStats, 'processorStats' : processorStats, 'userStats' : userStats}

    def getConsumerId(self):
        return self.consumerId

    def start(self):
        self.startTime = time.time()
        self.dataReceiver.start()
        for metadataChannel in self.metadataChannelMap.values():
            metadataChannel.start()
        if self.processingController:
            self.processingController.start()

    def stop(self):
        self.endTime = time.time()
        self.dataReceiver.stop()
        for metadataChannel in self.metadataChannelMap.values():
            metadataChannel.stop()
        if self.processingController:
            self.processingController.stop()
