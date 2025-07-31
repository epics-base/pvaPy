#!/usr/bin/env python

'''
EJFAT system base, contains functionality shared between higher level
EJFAT classes.
'''

import sys
import socket
import threading
import pvaccess as pva

# Exception will be thrown in case this class is actually used and E2SAR
# module is not available
try:
    import e2sar_py
except ImportError:
    pass

from ..utility.loggingManager import LoggingManager

class EjfatSystemBase:
    ''' EJFAT system base class. '''

    EJFAT_KEY = 'ejfat'
    URI_KEY = 'uri'
    HOSTNAME_KEY = 'hostname'
    PORT_KEY = 'port'
    IP_ADDRESS_KEY = 'ipAddress'
    N_RECEIVING_THREADS_KEY = 'nReceivingThreads'
    VALIDATE_SERVER_KEY = 'validateServer'
    WEIGHT_KEY = 'weight'
    SOURCE_COUNT_KEY = 'sourceCount'
    MIN_FACTOR_KEY = 'minFactor'
    MAX_FACTOR_KEY = 'maxFactor'
    DATA_ID_KEY = 'dataId'
    EVENT_SOURCE_ID_KEY = 'eventSourceId'
    STATE_UPDATE_PERIOD_KEY = 'stateUpdatePeriod'
    USE_CP_KEY = 'useCP'
    BUFFER_CACHE_SIZE_KEY = 'bufferCacheSize'
    PROCESSING_DELAY_KEY = 'processingDelay'
    PROCESSOR_ID_KEY = 'processorId'
    ADD_PROCESSOR_ID_TO_PORT_KEY = 'addProcessorIdToPort'

    DEFAULT_PORT = 10000 # default UDP data port
    DEFAULT_USE_CP = 1
    DEFAULT_VALIDATE_SERVER = 0
    DEFAULT_BUFFER_CACHE_SIZE = 1000
    DEFAULT_PROCESSOR_ID = 0
    DEFAULT_ADD_PROCESSOR_ID_TO_PORT = 0

    def __init__(self, uri, configDict={}):
        if 'e2sar_py' not in sys.modules:
            raise pva.ConfigurationError('The e2sar_py module was not imported')
        self.logger = LoggingManager.getLogger(self.__class__.__name__)
        self.logger.debug('Initializing EJFAT uri from %s', uri)
        self.uri = uri
        if not self.uri.startswith(self.EJFAT_KEY):
            self.logger.debug('Provided uri does not start with %s, looking for uri in the configuration dictionary', self.EJFAT_KEY)
            if self.URI_KEY in configDict:
                self.uri = configDict.get(self.URI_KEY)
        if not self.uri.startswith(self.EJFAT_KEY):
            raise pva.ConfigurationError(f'URI string should start with "{self.EJFAT_KEY}"')
        self.processorId = int(configDict.get(self.PROCESSOR_ID_KEY, self.DEFAULT_PROCESSOR_ID))
        self.logger.debug('Processor id: %s', self.processorId)
        self.addProcessorIdToPort = int(configDict.get(self.ADD_PROCESSOR_ID_TO_PORT_KEY, self.DEFAULT_ADD_PROCESSOR_ID_TO_PORT))
        self.logger.debug('Add processor id to port: %s', self.addProcessorIdToPort)
        self.port = int(configDict.get(self.PORT_KEY, self.DEFAULT_PORT))
        self.logger.debug('Specified port number: %s', self.port)
        if self.addProcessorIdToPort:
            self.port += self.processorId
            self.logger.debug('Port number after adding processor id: %s', self.port)
        try:
            self.ejfatUri = e2sar_py.EjfatURI(uri=self.uri, tt=e2sar_py.EjfatURI.TokenType.instance)
            if self.addProcessorIdToPort:
                # Modify assigned port; EjfatURI.set_data_addr_v4() does
                # not seem to work, so replace port in the URI string and
                # reinitialize EjfatURI instance
                originalPort = self.ejfatUri.get_data_addr_v4().value()[1]
                modifiedPort = originalPort + self.processorId
                self.uri = self.uri.replace(f'{originalPort}', f'{modifiedPort}')
                self.logger.debug('Reassigned original URI port %s to %s', originalPort, modifiedPort)
                self.ejfatUri = e2sar_py.EjfatURI(uri=self.uri, tt=e2sar_py.EjfatURI.TokenType.instance)
            self.logger.debug('Data plane address: %s:%s', self.ejfatUri.get_data_addr_v4().value()[0], self.ejfatUri.get_data_addr_v4().value()[1])
            self.logger.debug('Instance token: %s', self.ejfatUri.get_instance_token().value())
        except Exception as ex:
            raise pva.ConfigurationError(f'Cannot create EJFAT URI from {self.uriString}: {ex}')
        self.logger.debug('Initialized EJFAT uri')

        self.hostname = configDict.get(self.HOSTNAME_KEY) or socket.gethostname()
        self.logger.debug('Using hostname: %s', self.hostname)
        self.ipAddress = configDict.get(self.IP_ADDRESS_KEY) or socket.gethostbyname(self.hostname)
        self.logger.debug('Using ip address: %s', self.ipAddress)

        self.useCp = bool(configDict.get(self.USE_CP_KEY, self.DEFAULT_USE_CP))
        self.logger.debug('Use control plane: %s', self.useCp)
        self.validateServer = bool(configDict.get(self.VALIDATE_SERVER_KEY, self.DEFAULT_VALIDATE_SERVER))
        self.logger.debug('Validate control plane server: %s', self.validateServer)

        self.bufferCacheSize = int(configDict.get(self.BUFFER_CACHE_SIZE_KEY, self.DEFAULT_BUFFER_CACHE_SIZE))
        if self.bufferCacheSize < 1:
            self.bufferCacheSize = 1
        self.logger.debug('Using send buffer cache size: %s', self.bufferCacheSize)
        self.bufferCache = {}
        self.bufferCacheLock = threading.Lock()

        self.lbManager = None
        if self.useCp:
            try:
                self.logger.debug('Initializing LB manager')
                self.lbManager = e2sar_py.ControlPlane.LBManager(self.ejfatUri, self.validateServer)
                self.logger.debug('Initialized LB manager')
            except Exception as ex:
                raise pva.ConfigurationError(f'Cannot initialize EJFAT LB manager: {ex}')
        else:
            self.logger.debug('Control plane is not used, LB manager is not initialized')

        self.nPickled = 0
        self.totalPickleTime = 0
        self.averagePickleTime = 0
