#!/usr/bin/env python

'''
EJFAT data publisher module.
'''

import time
import pickle
import pvaccess as pva

# Exception will be thrown in case this class is actually used and E2SAR
# module is not available
try:
    import e2sar_py
except ImportError:
    pass
from .ejfatSystemBase import EjfatSystemBase

from .dataPublisher import DataPublisher
from ..utility.configUtility import ConfigUtility

class EjfatDataPublisher(DataPublisher, EjfatSystemBase):
    ''' EJFAT data publisher class. '''

    DEFAULT_DATA_ID = 0
    DEFAULT_EVENT_SOURCE_ID = 1

    # Valid Segmenter attributes: 'connectedSocket', 'dpV6', 'getFromINI', 'mtu', 'numSendSockets', 'sndSocketBufSize', 'syncPeriodMs', 'syncPeriods', 'useCP', 'usecAsEventNum', 'zeroRate'

    def __init__(self, outputChannel, configDict={}):
        DataPublisher.__init__(self, outputChannel)
        EjfatSystemBase.__init__(self, outputChannel, configDict)
        self.bufferIndex = 0
        self.synchronousMode = False

        self.bufferIndexSet = set(list(range(0, self.bufferCacheSize)))
        if self.bufferCacheSize <= 1:
            self.logger.debug('Send buffer cache size is 1, using synchronous send mode')
            self.synchronousMode = True

        self.logger.debug('Initializing segmenter flags')
        sflags = e2sar_py.DataPlane.Segmenter.SegmenterFlags()
        ConfigUtility.configureObject(sflags, configDict, setOnlyExistingKeys=True)
        self.dataId = int(configDict.get(self.DATA_ID_KEY, self.DEFAULT_DATA_ID))
        self.logger.debug('Data id: %s', self.dataId)
        self.eventSourceId = int(configDict.get(self.EVENT_SOURCE_ID_KEY, self.DEFAULT_EVENT_SOURCE_ID))
        self.logger.debug('Event source id: %s', self.eventSourceId)
        self.eventNumber = 0
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

    def __serialize(self, pvObject):
        t0 = time.time()
        buffer = pickle.dumps(pvObject)
        t1 = time.time()
        self.totalPickleTime += t1-t0
        self.nPickled += 1
        self.averagePickleTime = self.totalPickleTime/self.nPickled
        return buffer

    def __prepareSyncSendBuffer(self, pvObject):
        buffer = self.__serialize(pvObject)
        bufferLength = len(buffer)
        return buffer, bufferLength

    def __getSyncSendStats(self):
        try:
            stats = self.segmenter.getSendStats()
            if stats.lastErrno != 0:
                self.nErrors += 1
                self.logger.warning('Last segmenter error: %s', stats.lastErrno)
            else:
                self.nPublished += 1
        except Exception as ex:
            self.logger.error('Error getting segmenter stats: %s', ex)

    def __prepareAsyncSendBuffer(self, pvObject):
        with self.bufferCacheLock:
            usedBufferIndexSet = set(list(self.bufferCache.keys()))
            availableBuffers = sorted(self.bufferIndexSet-usedBufferIndexSet)
            if not availableBuffers:
                raise Exception('No free buffers available')
            bufferIndex = availableBuffers[0]
            self.bufferCache[bufferIndex] = bufferIndex
        self.bufferCache[bufferIndex] = self.__serialize(pvObject)
        bufferLength = len(self.bufferCache[bufferIndex])
        return bufferIndex, bufferLength

    def __getAsyncSendStats(self):
        try:
            stats = self.segmenter.getSendStats()
            if stats.lastErrno != 0:
                self.nErrors += 1
                self.logger.warning('Last segmenter error: %s', stats.lastErrno)
        except Exception as ex:
            self.logger.error('Error getting segmenter stats: %s', ex)

    def __asyncSendBufferCallback(self, bufferIndex):
        with self.bufferCacheLock:
            self.logger.debug('Freeing buffer index %s', bufferIndex)
            del self.bufferCache[bufferIndex]
            self.nPublished += 1

    def updateOutputChannel(self, pvObject):
        try:
            if self.synchronousMode:
                buffer, bufferLength = self.__prepareSyncSendBuffer(pvObject)
                self.logger.debug('Sending sync buffer with object of size %s, event number %s, average pickle time %.6f (%s pickled)', bufferLength, self.eventNumber, self.averagePickleTime, self.nPickled)
                self.segmenter.sendEvent(buffer, bufferLength, self.eventNumber)
                self.__getSyncSendStats()
            else:
                bufferIndex, bufferLength = self.__prepareAsyncSendBuffer(pvObject)
                self.logger.debug('Adding to send queue buffer with object of size %s, event number %s, average pickle time %.6f (%s pickled)', bufferLength, self.eventNumber, self.averagePickleTime, self.nPickled)
                self.segmenter.addToSendQueue(self.bufferCache[bufferIndex], bufferLength, self.eventNumber, callback=self.__asyncSendBufferCallback, cbArg=bufferIndex)
                self.__getAsyncSendStats()
        except Exception as ex:
            self.nErrors += 1
            self.logger.error('Error publishing object with event number %s: %s', self.eventNumber, ex)
            stats = self.segmenter.getSendStats()
            if stats.lastErrno != 0:
                self.logger.warning('Last segmenter error: %s', stats.lastErrno)
        self.eventNumber += 1
