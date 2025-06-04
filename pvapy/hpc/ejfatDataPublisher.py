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

    DEFAULT_DATA_ID = 0
    DEFAULT_EVENT_SOURCE_ID = 1

    # Valid Segmenter attributes: 'connectedSocket', 'dpV6', 'getFromINI', 'mtu', 'numSendSockets', 'sndSocketBufSize', 'syncPeriodMs', 'syncPeriods', 'useCP', 'usecAsEventNum', 'zeroRate'

    def __init__(self, outputChannel, configDict={}):
        DataPublisher.__init__(self, outputChannel)
        EjfatSystemBase.__init__(self, outputChannel, configDict)

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

    def updateOutputChannel(self, pvObject):
        try:
            buffer = pickle.dumps(pvObject)
            bLen = len(buffer)
            self.logger.debug('Buffering object of size %s', bLen)
            self.segmenter.addToSendQueue(buffer, bLen)
            self.nPublished += 1
        except Exception as ex:
            self.nErrors += 1
            self.logger.error('Error publishing object: %s', ex)
