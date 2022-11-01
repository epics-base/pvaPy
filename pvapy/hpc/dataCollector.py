#!/usr/bin/env python

import time
import threading
import pvaccess as pva
from .sourceChannel import SourceChannel
from .metadataChannelFactory import MetadataChannelFactory
from ..utility.loggingManager import LoggingManager
from ..utility.floatWithUnits import FloatWithUnits

class ProducerChannel(SourceChannel):
    def __init__(self, producerId, channelName, serverQueueSize, monitorQueueSize, objectIdField, fieldRequest, dataCollector):
        loggerName = f'producer-{producerId}'
        SourceChannel.__init__(self, producerId, channelName, pva.PVA, serverQueueSize, monitorQueueSize, loggerName, dataCollector)
        self.producerId = producerId
        self.objectIdField = objectIdField
        self.fieldRequest = fieldRequest
        self.dataCollector = dataCollector

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
        self.dataCollector.addObjectToCache(self.producerId, objectId, pvObject.copy())

    # Return true if object was processed, False otherwise
    def processFromQueue(self, waitTime=0):
        if self.pvObjectQueue is not None:
            try:
                pvObject = self.pvObjectQueue.get(waitTime)
                objectId = pvObject[self.objectIdField]
                # We can manipulate object from the queue without having to copy it
                self.dataCollector.addObjectToCache(self.producerId, objectId, pvObject)
                return True
            except pva.QueueEmpty as ex:
                # Ignore empty queue
                pass
        return False

class ProcessingThread(threading.Thread):

    THREAD_EVENT_TIMEOUT_IN_SECONDS = 5.0

    def __init__ (self, name, dataCollector):
        threading.Thread.__init__(self)
        self.name = name
        self.isDone = False
        self.dataCollector = dataCollector
        self.logger = LoggingManager.getLogger(f'{name}')
        self.isRunning = False

    def processObjects(self):
        objectTuples = self.dataCollector.getObjectsFromCache()
        nObjects = len(objectTuples)
        if nObjects > 0:
            for objectId,pvObject in objectTuples:
                try:
                    self.dataCollector.process(pvObject)
                except Exception as ex:
                    self.logger.error(f'Error processing object id {objectId}: {ex}')
            self.logger.debug(f'Processed {nObjects} from cache')
        return nObjects

    def run(self):
        self.isRunning = True
        self.logger.debug(f'Starting thread: {self.name}')
        while True:
            self.dataCollector.clearEvent()
            while True:
                try:
                    cacheSize = len(self.dataCollector.collectorCacheMap)
                    timeSinceCacheUpdate = time.time()-self.dataCollector.cacheUpdateTime 
                    if not cacheSize or self.isDone:
                        # Cache empty or we are done
                        break
                    elif cacheSize < self.dataCollector.collectorCacheSize and timeSinceCacheUpdate < self.THREAD_EVENT_TIMEOUT_IN_SECONDS:
                        # Cache is being filled 
                        break
                    nObjects = self.processObjects()
                except Exception as ex:
                    self.logger.exception(ex)

            if self.isDone:
                self.logger.debug('Exit flag is set')
                break
            if self.dataCollector.monitorQueueSize >= 0:
                # We are using client queues, need to push data into the cache
                nNewObjects = 0
                while True:
                    nNewObjects2 = self.dataCollector.pushObjectsToCacheFromProducerQueues()
                    nNewObjects += nNewObjects2
                    cacheSize = len(self.dataCollector.collectorCacheMap)
                    if cacheSize >= self.dataCollector.collectorCacheSize or self.isDone:
                        self.logger.debug(f'{nNewObjects} new objects cached, cache size: {cacheSize}')
                        break
                    # If we did not get any new objects, wait on event
                    if not nNewObjects2:
                        self.dataCollector.waitOnFirstProducerQueue(self.THREAD_EVENT_TIMEOUT_IN_SECONDS)
                        break
            else: 
                self.dataCollector.waitOnEvent(self.THREAD_EVENT_TIMEOUT_IN_SECONDS)
        # Finish up
        if self.dataCollector.monitorQueueSize >= 0:
            nNewObjects = 0
            nPushed = 1
            while nPushed:
                nPushed = self.dataCollector.pushObjectsToCacheFromProducerQueues()
                nNewObjects += nPushed
                
            self.logger.debug(f'Pushed remaining {nNewObjects} objects into the cache')
        cacheSize = len(self.dataCollector.collectorCacheMap)
        self.logger.debug(f'Processing all remaining {cacheSize} cached objects')
        while True:
            nObjects = self.processObjects()
            if not nObjects:
                break
        # Wait after processing so that any clients can pick up
        # last set of updates
        self.dataCollector.waitOnEvent(self.THREAD_EVENT_TIMEOUT_IN_SECONDS)
        self.isRunning = False
        self.logger.debug(f'{self.name} is done')

    def stop(self):
        self.isDone = True

class DataCollector:

    # Default queue sizing factor
    CACHE_SIZE_SCALING_FACTOR = 10

    STATUS_TYPE_DICT = {
        'collectorId' : pva.UINT,
        'objectId' : pva.UINT,
        'objectTime' : pva.DOUBLE,
        'objectTimestamp' : pva.PvTimeStamp(),
        'collectorStats' : {
            'nCollected' : pva.UINT,
            'collectedRate' : pva.DOUBLE,
            'nRejected' : pva.UINT,
            'rejectedRate' : pva.DOUBLE,
            'nMissed' : pva.UINT,
            'missedRate' : pva.DOUBLE
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

    def __init__(self, collectorId, inputChannel, producerIdList=[1], idFormatSpec=None, objectIdField='uniqueId', objectIdOffset=1, fieldRequest='', serverQueueSize=0, monitorQueueSize=-1, collectorCacheSize=-1, metadataChannels=None, processingController=None):
        self.logger = LoggingManager.getLogger(f'collector-{collectorId}')
        self.eventLock = threading.Lock()
        self.event = threading.Event()
        self.collectorId = collectorId
        self.inputChannel = inputChannel
        self.logger.debug(f'Input channel template: {inputChannel}')
        self.objectIdField = objectIdField 
        self.logger.debug(f'Object id field: {objectIdField}')
        self.objectIdOffset = objectIdOffset
        self.logger.debug(f'Object id offset: {objectIdOffset}')
        self.fieldRequest = fieldRequest
        self.logger.debug(f'Field request: {fieldRequest}')
        self.serverQueueSize = serverQueueSize
        self.logger.debug(f'Server queue size: {serverQueueSize}')
        self.nProducers = len(producerIdList)
        self.producerIdList = producerIdList
        if not self.nProducers:
            raise pva.InvalidArgument('Producer id list cannot be empty.')
        self.collectorCacheSize = self.getCollectorCacheSize(collectorCacheSize)
        self.logger.debug(f'Collector cache size is set to {self.collectorCacheSize}')
        self.monitorQueueSize = self.getClientQueueSize(monitorQueueSize)
        self.logger.debug(f'Client queue size is set to {self.monitorQueueSize}')
        self.cacheLock = threading.Lock()
        self.collectorCacheMap = {}

        # Producer channels
        self.logger.debug(f'Using producer id format spec: {idFormatSpec}')
        self.producerChannelMap = {}
        for producerId in producerIdList:
            producerIdString = str(producerId)
            if idFormatSpec: 
                producerIdString = f'{producerId:{idFormatSpec}}'
            cName = f'{inputChannel}'.replace('*', producerIdString)
            self.logger.debug(f'Creating channel {cName} for producer {producerId}')
            self.producerChannelMap[producerId] = ProducerChannel(producerId, cName, serverQueueSize, self.monitorQueueSize, objectIdField, fieldRequest, self)

        # Metadata channels
        self.metadataChannelMap, self.metadataQueueMap = MetadataChannelFactory.createMetadataChannels(metadataChannels, serverQueueSize, monitorQueueSize, self)

        self.processingController = processingController
        if self.processingController.userDataProcessor:
            self.processingController.userDataProcessor.metadataQueueMap = self.metadataQueueMap

        self.processingThread = ProcessingThread(f'ProcessingThread-{self.collectorId}', self)
        self.startTime = None
        self.endTime = None
        self.minCachedObjectId = None
        self.cacheUpdateTime = time.time()
        self.lastObjectId = None

        self.nRejected = 0
        self.nCollected = 0
        self.nMissed = 0

        # If first object is ignored, stats will be adjusted
        self.nReceivedOffset = 0
        if self.processingController and hasattr(self.processingController, 'ignoreFirstObject') and self.processingController.ignoreFirstObject:
            self.nReceivedOffset = 1

        self.logger.debug(f'Created data collector {collectorId}')

    def getCollectorCacheSize(self, collectorCacheSize):
        minCollectorCacheSize = self.nProducers*self.CACHE_SIZE_SCALING_FACTOR 
        if collectorCacheSize > minCollectorCacheSize: 
            return collectorCacheSize
        return minCollectorCacheSize 

    def getClientQueueSize(self, monitorQueueSize):
        if monitorQueueSize <= 0:
            # Either client queue is not used, or it is set to infinity
            return monitorQueueSize
        # Make sure client queue size is sufficiently large
        minClientQueueSize = self.collectorCacheSize*self.CACHE_SIZE_SCALING_FACTOR 
        if monitorQueueSize > minClientQueueSize: 
            return monitorQueueSize
        return minClientQueueSize 

    def configure(self, configDict):
        if type(configDict) == dict:
            if 'collectorCacheSize' in configDict:
                collectorCacheSize = int(configDict.get('collectorCacheSize'))
                self.collectorCacheSize = self.getCollectorCacheSize(collectorCacheSize)
                self.logger.debug(f'Collector cache size is set to {self.collectorCacheSize}')
            for producerId,producerChannel in self.producerChannelMap.items():
                producerChannel.configure(configDict)
        if self.processingController:
            self.processingController.configure(configDict)

    def process(self, pv):
        if self.processingController:
            self.processingController.process(pv)

    def waitOnFirstProducerQueue(self, waitTime):
        producerId = self.producerIdList[0]
        producerChannel = self.producerChannelMap[producerId]
        producerChannel.waitOnQueue(waitTime)
       
    def pushObjectsToCacheFromProducerQueues(self):
        nNewObjects = 0
        for producerId,producerChannel in self.producerChannelMap.items():
            if producerChannel.processFromQueue():
                nNewObjects += 1
        return nNewObjects

    def addObjectToCache(self, producerId, objectId, pvObject):
        self.cacheLock.acquire()
        try:
            if self.minCachedObjectId is None:
                self.minCachedObjectId = objectId
            if self.lastObjectId is not None and objectId <= self.lastObjectId:
                self.nRejected += 1
                self.logger.debug(f'Rejecting object id {objectId} from producer {producerId} (last processed object id: {self.lastObjectId}; total number of rejected objects: {self.nRejected}')
                return 

            self.collectorCacheMap[objectId] = pvObject
            if objectId < self.minCachedObjectId:
                self.minCachedObjectId = objectId
            self.cacheUpdateTime = time.time()
            self.setEvent()
        finally:
            self.cacheLock.release()

    def getObjectsFromCache(self):
        self.cacheLock.acquire()
        try:
            # Get as many sequential objects as possible
            objectTuples = []
            if not len(self.collectorCacheMap):
                return objectTuples 
            nMissed = 0
            if self.lastObjectId is not None:
                nMissed = self.minCachedObjectId-self.lastObjectId-self.objectIdOffset
                if nMissed > 0:
                    self.logger.debug(f'Missed {nMissed} objects since last retrieval from cache; total number of missed objects: {self.nMissed}')
                    self.nMissed += nMissed
            objectId = self.minCachedObjectId
            objectTuples = []
            while True:
                if objectId in self.collectorCacheMap:
                    objectTuples.append((objectId, self.collectorCacheMap[objectId]))
                    del self.collectorCacheMap[objectId]
                    self.lastObjectId = objectId
                    objectId += 1
                else:
                    break
            nCollected = len(objectTuples)
            self.minCachedObjectId = None
            if len(self.collectorCacheMap):
                self.minCachedObjectId = min(self.collectorCacheMap)
            self.nCollected += nCollected
            self.logger.debug(f'Found {nCollected} objects ready for processing, remaining cache size is {len(self.collectorCacheMap)}')
            self.cacheUpdateTime = time.time()
            return objectTuples 
        finally:
            self.cacheLock.release()

    def resetStats(self):
        for producerId,producerChannel in self.producerChannelMap.items():
            producerChannel.resetStats()
        self.nRejected = 0
        self.nCollected = 0
        self.nMissed = 0
        self.lastObjectId = None
       
    def getCollectorStats(self, receivingTime):
        collectorStats = {
            'nCollected' : self.nCollected, 
            'nRejected' : self.nRejected,
            'nMissed' : self.nMissed
        }
        collectedRate = 0
        rejectedRate = 0
        missedRate = 0
        if receivingTime > 0:
            collectedRate = self.nCollected/receivingTime
            rejectedRate = self.nRejected/receivingTime
            missedRate = self.nMissed/receivingTime
        collectorStats['collectedRate'] = FloatWithUnits(collectedRate, 'Hz')
        collectorStats['rejectedRate'] = FloatWithUnits(rejectedRate, 'Hz')
        collectorStats['missedRate'] = FloatWithUnits(missedRate, 'Hz')
        return collectorStats

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
        userStats = self.getUserStats()
        receivingTime = processorStats.get('receivingTime', 0)
        collectorStats = self.getCollectorStats(receivingTime)
        producerStats = {}
        for producerId,producerChannel in self.producerChannelMap.items():
            producerStats[f'producer-{producerId}'] = producerChannel.getStats(receivingTime, self.nReceivedOffset)
        metadataStats = {}
        for metadataChannelId,metadataChannel in self.metadataChannelMap.items():
            metadataStats[f'metadata-{metadataChannelId}'] = metadataChannel.getStats(receivingTime)
        return {'producerStats' : producerStats, 'metadataStats' : metadataStats, 'processorStats' : processorStats, 'userStats' : userStats, 'collectorStats' : collectorStats}

    def start(self):
        self.startTime = time.time()
        self.processingThread.start()
        for producerId,producerChannel in self.producerChannelMap.items():
            producerChannel.start()
        for metadataChannelId,metadataChannel in self.metadataChannelMap.items():
            metadataChannel.start()
        if self.processingController:
            self.processingController.start()

    def stop(self):
        self.endTime = time.time()
        for producerId,producerChannel in self.producerChannelMap.items():
            producerChannel.stop()
        for metadataChannelId,metadataChannel in self.metadataChannelMap.items():
            metadataChannel.stop()
        self.processingThread.stop()
        self.processingThread.join(ProcessingThread.THREAD_EVENT_TIMEOUT_IN_SECONDS)
        if self.processingController:
            self.processingController.stop()
        self.logger.debug(f'Collected objects {self.nCollected}; missed objects: {self.nMissed}; rejected objects: {self.nRejected}')

    def setEvent(self):
        self.eventLock.acquire()
        try:
            self.event.set()
        finally:
            self.eventLock.release()

    def clearEvent(self):
        self.eventLock.acquire()
        try:
            self.event.clear()
        finally:
            self.eventLock.release()

    def waitOnEvent(self, timeout=None):
        self.event.wait(timeout)

