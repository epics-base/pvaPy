#!/usr/bin/env python

'''
EJFAT data receiver (server) module.
'''

import threading
import time
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

from .dataReceiver import DataReceiver
from ..utility.configUtility import ConfigUtility
from ..utility.loggingManager import LoggingManager

class EjfatDataReceiver(DataReceiver, EjfatSystemBase, threading.Thread):
    ''' EJFAT data receiver (server) class. '''

    DEFAULT_PORT = 35328 # EJFAT
    DEFAULT_N_RECEIVING_THREADS = 1
    DEFAULT_WEIGHT = 1.0
    DEFAULT_SOURCE_COUNT = 1
    DEFAULT_MIN_FACTOR = 1.0
    DEFAULT_MAX_FACTOR = 1.0

    # It looks like workers must send state every 100ms, or else
    # they will be automatically deregistered
    # This does seem a bit aggressive
    DEFAULT_STATE_UPDATE_PERIOD_IN_SECONDS = 0.1

    # Valid Reassembler attributes: 'Kd', 'Ki', 'Kp', 'epoch_ms', 'eventTimeout_ms', 'getFromINI', 'max_factor', 'min_factor', 'period_ms', 'portRange', 'rcvSocketBufSize', 'setPoint', 'useCP', 'useHostAddress', 'validateCert', 'weight', 'withLBHeader'

    def __init__(self, inputChannel, processingFunction, configDict={}, pvObjectQueue=None):
        DataReceiver.__init__(self, inputChannel, processingFunction)
        EjfatSystemBase.__init__(self, inputChannel, configDict)
        threading.Thread.__init__(self)
        self.event = threading.Event()
        self.isDone = False

        self.logger.debug('Initializing reassembler flags using configuration: %s', configDict)
        rflags = e2sar_py.DataPlane.Reassembler.ReassemblerFlags()
        rflags.portRange = 0
        ConfigUtility.configureObject(rflags, configDict, setOnlyExistingKeys=True)

        self.port = int(configDict.get(self.PORT_KEY, self.DEFAULT_PORT))
        self.logger.debug('Using port number: %s', self.port)

        self.nReceivingThreads = int(configDict.get(self.N_RECEIVING_THREADS_KEY, self.DEFAULT_N_RECEIVING_THREADS))
        self.logger.debug('Number of receiving threads: %s', self.nReceivingThreads)
        self.weight = float(configDict.get(self.WEIGHT_KEY, self.DEFAULT_WEIGHT))
        self.logger.debug('Weight: %s', self.weight)
        self.sourceCount = int(configDict.get(self.SOURCE_COUNT_KEY, self.DEFAULT_SOURCE_COUNT))
        self.logger.debug('Source count: %s', self.sourceCount)
        self.minFactor = float(configDict.get(self.MIN_FACTOR_KEY, self.DEFAULT_MIN_FACTOR))
        self.logger.debug('Min. factor: %s', self.minFactor)
        self.maxFactor = float(configDict.get(self.MAX_FACTOR_KEY, self.DEFAULT_MAX_FACTOR))
        self.logger.debug('Max. factor: %s', self.maxFactor)
        self.stateUpdatePeriod = float(configDict.get(self.STATE_UPDATE_PERIOD_KEY, self.DEFAULT_STATE_UPDATE_PERIOD_IN_SECONDS))
        self.logger.debug('State update period: %s [s]', self.stateUpdatePeriod)

        try:
            self.logger.debug('Creating reassembler')
            self.reassembler = e2sar_py.DataPlane.Reassembler(self.ejfatUri, e2sar_py.IPAddress.from_string(self.ipAddress), self.port, self.nReceivingThreads, rflags)
        except Exception as ex:
            raise pva.ConfigurationError(f'Cannot initialize EJFAT data receiver: {ex}')

        self.pvObjectQueue = pvObjectQueue
        if self.pvObjectQueue is not None:
            self.logger.debug('Using PvObjectQueue of size %s', self.pvObjectQueue.maxLength)
        else:
            self.logger.debug('Not using PvObjectQueue')
        self.logger.debug('Created EJFAT data receiver for input channel %s', inputChannel)

    def process(self, buffer):
        try:
            self.nReceived += 1
            pv = pickle.loads(buffer)
            if self.pvObjectQueue is not None:
                try:
                    self.pvObjectQueue.put(pv)
                except pva.QueueFull:
                    errorMessage = 'Input PV object queue is full.'
                    self.logger.error(errorMessage)
                    self.nRejected += 1
            else:
                self.processingFunction(pv)
        except Exception as ex:
            self.nErrors += 1
            errorMessage = f'Error processing input PV object: {ex}'
            self.logger.error(errorMessage)

    def run(self):
        self.logger.debug('Starting EJFAT receiver for input channel %s', self.inputChannel)
        try:
            if self.useCp:
                self.logger.debug('Registering worker with hostname %s, IP address %s, port %s', self.hostname, self.ipAddress, self.port)
                self.lbManager.register_worker(self.hostname, (e2sar_py.IPAddress.from_string(self.ipAddress), self.port), self.weight, self.sourceCount, self.minFactor, self.maxFactor)
            else:
                self.logger.debug('Skipping worker registration')
            self.logger.debug('Starting reassembler')
            self.reassembler.OpenAndStart()
            self.logger.debug('Entering event loop')
            self.event.clear()
            fillPercent = 0
            controlSignal = 0
            isReady = True
            lastStateUpdateTime = 0
            while True:
                if self.isDone:
                    self.logger.debug('Exiting event loop')
                    break
                nBytes = 0
                while True:
                    now = time.time()
                    if self.useCp and now - lastStateUpdateTime > self.stateUpdatePeriod:
                        self.lbManager.send_state(fillPercent, controlSignal, isReady)
                        lastStateUpdateTime = now
                    nBytes, buffer, eventNumber, dataId = self.reassembler.getEventBytes()
                    if nBytes > 0:
                        self.logger.debug('Received %s bytes, data id %s, event number %s', nBytes, dataId, eventNumber)
                        self.process(buffer)
                    else:
                        break
                waitTime = self.stateUpdatePeriod
                if self.useCp:
                    waitTime = self.stateUpdatePeriod - (now - lastStateUpdateTime)
                if waitTime > 0:
                    self.event.wait(waitTime)
        except Exception as ex:
            self.logger.error('Caught unexpected error: %s', ex)
        self.logger.debug('EJFAT server for input channel %s is done', self.inputChannel)

    def start(self):
        threading.Thread.start(self)

    def stop(self):
        self.logger.debug('Shutting down receiver')
        self.isDone = True
        self.event.set()
