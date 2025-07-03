#!/usr/bin/env python

'''
EJFAT data publisher module.
'''

import threading
import socket
import pickle
import pvaccess as pva

# Exception will be thrown in case this class is actually used and E2SAR
# module is not available
try:
    import e2sar_py
except:
    pass
from .ejfatSystemBase import EjfatSystemBase

from .dataPublisher import DataPublisher
from ..utility.configUtility import ConfigUtility
from ..utility.loggingManager import LoggingManager

class EjfatDataPublisher(DataPublisher, EjfatSystemBase):
    ''' EJFAT data publisher class. '''

    BUFFER_CACHE_SIZE_KEY = 'bufferCacheSize'

    DEFAULT_DATA_ID = 0
    DEFAULT_EVENT_SOURCE_ID = 1
    DEFAULT_BUFFER_CACHE_SIZE = 100

    # Valid Segmenter attributes: 'connectedSocket', 'dpV6', 'getFromINI', 'mtu', 'numSendSockets', 'sndSocketBufSize', 'syncPeriodMs', 'syncPeriods', 'useCP', 'usecAsEventNum', 'zeroRate'

    def __init__(self, outputChannel, configDict={}):
        DataPublisher.__init__(self, outputChannel)
        EjfatSystemBase.__init__(self, outputChannel, configDict)
        self.bufferMap = {}
        self.bufferIndex = 0
        self.bufferMapLock = threading.Lock()

        self.bufferCacheSize = int(configDict.get(self.BUFFER_CACHE_SIZE_KEY, self.DEFAULT_BUFFER_CACHE_SIZE))
        if self.bufferCacheSize < 1:
            self.bufferCacheSize = self.DEFAULT_BUFFER_CACHE_SIZE
        self.logger.debug('Using send buffer cache size: %s', self.bufferCacheSize)
        self.bufferIndexSet = set(list(range(0, self.bufferCacheSize)))

        self.logger.debug('Initializing segmenter flags')
        sflags = e2sar_py.DataPlane.Segmenter.SegmenterFlags()
        ConfigUtility.configureObject(sflags, configDict, setOnlyExistingKeys=True)
        self.dataId = int(configDict.get(self.DATA_ID_KEY, self.DEFAULT_DATA_ID))
        self.logger.debug('Data id: %s', self.dataId)
        self.eventSourceId = int(configDict.get(self.EVENT_SOURCE_ID_KEY, self.DEFAULT_EVENT_SOURCE_ID))
        self.logger.debug('Event source id: %s', self.eventSourceId)
        try:
            if self.useCp:
                self.logger.debug('Adding sender with hostname %s, IP address %s', self.hostname, self.ipAddress)
                self.lbManager.add_senders([self.ipAddress])
            else:
                self.logger.debug('Control plane is not used, not adding sender')

            self.logger.debug('Creating segmenter')
            self.segmenter = e2sar_py.DataPlane.Segmenter(self.ejfatUri, self.dataId, self.eventSourceId, sflags)
        except Exception as ex:
            raise pva.ConfigurationError(f'Cannot initialize EJFAT data publisher: {ex}')
        self.logger.debug('Created EJFAT data publisher')

    def start(self):
        try:
            self.logger.debug('Starting segmenter')
            self.segmenter.OpenAndStart()
        except Exception as ex:
            raise pva.ConfigurationError(f'Cannot start EJFAT data publisher: {ex}')

    def __prepareSendBuffer(self, pvObject):
        with self.bufferMapLock:
            usedBufferIndexSet = set(list(self.bufferMap.keys()))
            availableBuffers = sorted(self.bufferIndexSet-usedBufferIndexSet)
            if not availableBuffers:
                raise Exception('No free buffers available')
            bufferIndex = availableBuffers[0]
            self.bufferMap[bufferIndex] = bufferIndex
        self.bufferMap[bufferIndex] = pickle.dumps(pvObject)
        bufferLength = len(self.bufferMap[bufferIndex])
        return bufferIndex, bufferLength

    def __sendBufferCallback(self, bufferIndex):
        with self.bufferMapLock:
            self.logger.debug('Freeing buffer index %s', bufferIndex)
            del self.bufferMap[bufferIndex]
            self.nPublished += 1

    def updateOutputChannel(self, pvObject):
        try:
            bufferIndex, bufferLength = self.__prepareSendBuffer(pvObject)
            self.logger.debug('Using buffer index %s to add to queue object of size %s', bufferIndex, bufferLength)
            #self.segmenter.addToSendQueue(self.bufferMap[bufferIndex], bufferLength, callback=self.__sendBufferCallback, cbArg=bufferIndex)
            self.segmenter.sendEvent(self.bufferMap[bufferIndex], bufferLength)
            stats = self.segmenter.getSendStats()
            if stats.lastErrno != 0:
                self.nErrors += 1
                self.logger.warning('Last segmenter error: %s', stats.lastErrno)
            else:
                self.nPublished += 1
            del self.bufferMap[bufferIndex]
        except Exception as ex:
            self.nErrors += 1
            self.logger.error('Error publishing object: %s', ex)
            stats = self.segmenter.getSendStats()
            if stats.lastErrno != 0:
                self.logger.warning('Last segmenter error: %s', stats.lastErrno)
