#!/usr/bin/env python

import time
import pvaccess as pva
from ..utility.loggingManager import LoggingManager
from ..utility.floatWithUnits import FloatWithUnits

class DataConsumer:

    PROVIDER_TYPE_MAP = { 'pva' : pva.PVA, 'ca' : pva.CA }

    def __init__(self, consumerId, inputChannel, providerType=pva.PVA, objectIdField='uniqueId', fieldRequest='', serverQueueSize=-1, monitorQueueSize=-1, distributorPluginName='pydistributor', distributorGroupId=None, distributorSetId=None, distributorTriggerFieldName=None, distributorUpdates=None, distributorUpdateMode=None, processingController=None):
        self.logger = LoggingManager.getLogger(f'consumer-{consumerId}')
        self.consumerId = consumerId
        providerType = self.PROVIDER_TYPE_MAP.get(providerType.lower(), pva.PVA)
        self.logger.debug(f'Channel {inputChannel} provider type: {providerType}')
        self.channel = pva.Channel(inputChannel, providerType)
        self.serverQueueSize = serverQueueSize
        self.logger.debug(f'Server queue size: {serverQueueSize}')
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
            self.logger.debug(f'Using PvObjectQueue of size: {monitorQueueSize}')
        self.processingController = processingController
        self.startTime = None
        self.endTime = None

        # If first object is ignored, stats will be adjusted
        self.nReceivedOffset = 0
        if self.processingController and hasattr(self.processingController, 'ignoreFirstObject') and self.processingController.ignoreFirstObject:
            self.nReceivedOffset = 1

        self.logger.debug(f'Created data consumer {consumerId}')

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
        if type(configDict) == dict:
            if 'monitorQueueSize' in configDict:
                monitorQueueSize = int(configDict.get('monitorQueueSize'))
                if self.pvObjectQueue is not None:
                    self.logger.debug(f'Resetting PvObjectQueue size from {self.pvObjectQueue.maxLength} to {monitorQueueSize}')
                    self.pvObjectQueue.maxLength = monitorQueueSize
                    self.monitorQueueSize = monitorQueueSize
        if self.processingController:
            self.processingController.configure(configDict)

    def process(self, pv):
        if self.processingController:
            self.processingController.process(pv)

    # Return true if object was processed, False otherwise
    def processFromQueue(self, waitTime):
        if self.pvObjectQueue is not None:
            try:
                pvObject = self.pvObjectQueue.get(waitTime)
                self.process(pvObject)
                return True
            except pva.QueueEmpty as ex:
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
        return {'monitorStats' : monitorStats, 'queueStats' : queueStats, 'processorStats' : processorStats, 'userStats' : userStats}

    def getConsumerId(self):
        return self.consumerId
 
    def start(self):
        self.startTime = time.time()
        request = self.getPvMonitorRequest()
        self.logger.debug(f'Using request string {request}')
        if self.pvObjectQueue is not None:
            self.logger.debug('Starting queue monitor')
            self.channel.qMonitor(self.pvObjectQueue, request)
        else:
            self.logger.debug('Starting process monitor')
            self.channel.monitor(self.process, request)

        if self.processingController:
            self.processingController.start()

    def stop(self):
        self.endTime = time.time()
        self.channel.stopMonitor()
        if self.processingController:
            self.processingController.stop()

