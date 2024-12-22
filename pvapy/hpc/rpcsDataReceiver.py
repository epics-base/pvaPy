#!/usr/bin/env python

'''
RPCS data receiver module.
'''

import threading
import pvaccess as pva
from .dataReceiver import DataReceiver

class RpcsDataReceiver(DataReceiver, threading.Thread):
    ''' RPCS data receiver class. '''

    STATUS_OK = pva.PvObject({'status': pva.INT}, {'status' : 0})
    STATUS_ERROR = pva.PvObject({'status': pva.INT, 'errorMessage' : pva.STRING}, {'status' : -1})

    def __init__(self, inputChannel, processingFunction, pvaServer, pvObjectQueue=None):
        DataReceiver.__init__(self, inputChannel, processingFunction)
        threading.Thread.__init__(self)
        self.rpcServer = pva.RpcServer()
        self.rpcServer.registerService(self.inputChannel, self.process)
        self.pvObjectQueue = pvObjectQueue
        if self.pvObjectQueue is not None:
            self.logger.debug('Using PvObjectQueue of size %s', self.pvObjectQueue.maxLength)
        else:
            self.logger.debug('Not using PvObjectQueue')
        self.logger.debug('Created RPCS data receiver for input channel %s', inputChannel)

    def process(self, pv):
        status = self.STATUS_OK
        try:
            self.nReceived += 1
            if self.pvObjectQueue is not None:
                try:
                    self.pvObjectQueue.put(pv)
                except pva.QueueFull:           
                    status = self.STATUS_ERROR
                    errorMessage = 'Input PV object queue is full.'
                    status['errorMessage'] = errorMessage
                    self.logger.error(errorMessage)
                    self.nRejected += 1
            else:
                self.processingFunction(pv)
        except Exception as ex:           
            self.nErrors += 1
            errorMessage = f'Error processing input PV object: {ex}'
            status['errorMessage'] = errorMessage
            self.logger.error(errorMessage)
        return status

    def run(self):
        self.logger.debug('Starting RPC server for input channel %s', self.inputChannel)
        self.rpcServer.start()
        self.logger.debug('RPC server for input channel %s is done', self.inputChannel)

    def start(self):
        threading.Thread.start(self)

    def stop(self):
        self.rpcServer.stop()
