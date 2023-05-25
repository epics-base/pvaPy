#!/usr/bin/env python

'''
Data consumer module.
'''

import time
import pvaccess as pva
from .metadataChannelFactory import MetadataChannelFactory
from ..utility.loggingManager import LoggingManager
from ..utility.floatWithUnits import FloatWithUnits

class DataConsumer:
    ''' Data consumer class. '''

    PROVIDER_TYPE_MAP = { 'pva' : pva.PVA, 'ca' : pva.CA }

    STATUS_TYPE_DICT = {
        'consumerId' : pva.UINT,
        'inputChannel' : pva.STRING,
        'objectId' : pva.UINT,
        'objectTime' : pva.DOUBLE,
        'objectTimestamp' : pva.PvTimeStamp(),
        'monitorStats' : {
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

    def __init__(self, consumerId, inputChannel, providerType=pva.PVA, objectIdField='uniqueId', fieldRequest='', serverQueueSize=-1, monitorQueueSize=-1, accumulateObjects=-1, accumulationTimeout=-1, distributorPluginName='pydistributor', distributorGroupId=None, distributorSetId=None, distributorTriggerFieldName=None, distributorUpdates=None, distributorUpdateMode=None, metadataChannels=None, processingController=None):
        self.logger = LoggingManager.getLogger(f'consumer-{consumerId}')
        self.consumerId = consumerId
        providerType = self.PROVIDER_TYPE_MAP.get(providerType.lower(), pva.PVA)
        self.logger.debug('Channel %s provider type: %s', inputChannel, providerType)
        self.channel = pva.Channel(inputChannel, providerType)
        self.inputChannel = inputChannel
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
        self.monitorQueueSize = monitorQueueSize
        if monitorQueueSize >= 0:
            self.pvObjectQueue = pva.PvObjectQueue(monitorQueueSize)
            self.logger.debug('Using PvObjectQueue of size: %s', monitorQueueSize)
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
        self.metadataChannelMap, self.metadataQueueMap = MetadataChannelFactory.createMetadataChannels(metadataChannels, serverQueueSize, monitorQueueSize, self)

        if self.processingController and self.processingController.userDataProcessor:
            self.processingController.userDataProcessor.metadataQueueMap = self.metadataQueueMap

        self.logger.debug('Created data consumer %s', consumerId)

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
            if 'monitorQueueSize' in configDict:
                monitorQueueSize = int(configDict.get('monitorQueueSize'))
                if self.pvObjectQueue is not None:
                    self.logger.debug('Resetting PvObjectQueue size from %s to %s', self.pvObjectQueue.maxLength, monitorQueueSize)
                    self.pvObjectQueue.maxLength = monitorQueueSize
                    self.monitorQueueSize = monitorQueueSize
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
        self.channel.resetMonitorCounters()
        if self.pvObjectQueue is not None:
            self.pvObjectQueue.resetCounters()
        if self.processingController:
            self.processingController.resetStats()

    def getMonitorStats(self):
        return self.channel.getMonitorCounters()

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
        monitorStats = self.getMonitorStats()
        queueStats = self.getQueueStats()
        nOverruns = monitorStats.get('nOverruns', 0)
        processorStats = self.getProcessorStats()
        userStats = self.getUserStats()
        receivedRate = 0
        overrunRate = 0
        receivingTime = processorStats.get('receivingTime', 0)
        nReceived = monitorStats.get('nReceived', 0)-self.nReceivedOffset
        if receivingTime > 0 and nReceived >= 0:
            receivedRate = nReceived/receivingTime
            overrunRate = nOverruns/receivingTime
        monitorStats['receivedRate'] = FloatWithUnits(receivedRate, 'Hz')
        monitorStats['overrunRate'] = FloatWithUnits(overrunRate, 'Hz')
        metadataStats = {}
        for metadataChannelId,metadataChannel in self.metadataChannelMap.items():
            metadataStats[f'metadata-{metadataChannelId}'] = metadataChannel.getStats(receivingTime)

        return {'inputChannel' : self.inputChannel, 'monitorStats' : monitorStats, 'queueStats' : queueStats, 'metadataStats' : metadataStats, 'processorStats' : processorStats, 'userStats' : userStats}

    def getConsumerId(self):
        return self.consumerId

    def start(self):
        self.startTime = time.time()
        request = self.getPvMonitorRequest()
        self.logger.debug('Using request string: %s', request)
        if self.pvObjectQueue is not None:
            self.logger.debug('Starting queue monitor')
            self.channel.qMonitor(self.pvObjectQueue, request)
        else:
            self.logger.debug('Starting process monitor')
            self.channel.monitor(self.process, request)
        for metadataChannel in self.metadataChannelMap.values():
            metadataChannel.start()
        if self.processingController:
            self.processingController.start()

    def stop(self):
        self.endTime = time.time()
        self.channel.stopMonitor()
        for metadataChannel in self.metadataChannelMap.values():
            metadataChannel.stop()
        if self.processingController:
            self.processingController.stop()
