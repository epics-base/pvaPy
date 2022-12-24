#!/usr/bin/env python

import threading
import queue
import os
import multiprocessing as mp
from ..utility.loggingManager import LoggingManager
from .hpcController import HpcController

class UserMpWorker(mp.Process):

    ''' 
    User multiprocessing worker class.
  
    **UserMpWorker(workerId, userMpDataProcessor, commandRequestQueue, commandResponseQueue, inputDataQueue, logLevel=None, logFile=None)**

    :Parameter: *workerId* (str) - Worker id.
    :Parameter: *userMpDataProceessor* (UserMpDataProcessor) - Instance of the UserMpDataProcessor class that will be processing data.
    :Parameter: *commandRequestQueue* (multiprocessing.Queue) - Command request queue.
    :Parameter: *commandResponseQueue* (multiprocessing.Queue) - Command response queue.
    :Parameter: *inputDataQueue* (multiprocessing.Queue) - Input data queue.
    :Parameter: *logLevel* (str) - Log level; possible values: debug, info, warning, error, critical. If not provided, there will be no log output.
    :Parameter: *logFile* (str) - Log file.
    '''
    def __init__(self, workerId, userMpDataProcessor, commandRequestQueue, commandResponseQueue, inputDataQueue, logLevel=None, logFile=None):
 
        mp.Process.__init__(self) 
        self.logger = LoggingManager.getLogger(f'{self.__class__.__name__}.{workerId}', logLevel, logFile)
        self.workerId = workerId 
        self.userMpDataProcessor = userMpDataProcessor

        self.inputDataQueue = inputDataQueue
        self.commandRequestQueue = commandRequestQueue
        self.commandResponseQueue = commandResponseQueue
        self.isStopped = True
        self.rpThread = RequestProcessingThread(self)

    def start(self):
        '''
        Method invoked at processing startup.
        '''
        if self.isStopped:
            self.isStopped = False
            self.userMpDataProcessor.start()
            mp.Process.start(self)

    def configure(configDict):
        '''
        Method invoked at user initiated runtime configuration changes.

        :Parameter: *configDict* (dict) - dictionary containing configuration parameters
        '''
        self.userMpDataProcessor.configure(configDict)

    def process(self, mpqObject):
        '''
        Data processing method.

        :Parameter: *mpqObject* (object) - object received from multiprocessing queue
        '''
        return self.userMpDataProcessor.process(mpqObject)

    def stop(self):
        '''
        Method invoked at processing shutdown.

        :Returns: Dictionary containing application statistics parameters
        '''
        if not self.isStopped:
            self.logger.debug(f'Stopping worker {self.workerId}, PID: {os.getpid()}')
            self.isStopped = True
            self.userMpDataProcessor.stop()
            try:
                self.logger.debug(f'Emptying input data queue for worker {self.workerId}, queue size is {self.inputDataQueue.qsize()}')
                while not self.inputDataQueue.empty():
                    self.inputDataQueue.get(block=True, timeout=HpcController.WAIT_TIME)
            except Exception as ex:
                self.logger.warn(f'Error emptying input data queue for worker {self.workerId}: {ex}')
        return self.getStats()

    def resetStats(self):
        '''
        Method invoked at user initiated application statistics reset.
        '''
        self.userMpDataProcessor.resetStats()

    def getStats(self):
        '''
        Method invoked periodically for generating application statistics.

        :Returns: Dictionary containing application statistics parameters
        '''
        return self.userMpDataProcessor.getStats()

    def run(self):
        '''
        Data processing thread. It retrieves objects from the input 
        queue and invokes user data processor process() method.
        '''
        self.logger.debug(f'Data processing thread for worker {self.workerId} starting, PID: {os.getpid()}')
        self.rpThread.start()
        while True:
            if self.isStopped:
                break
            try:
                inputData = self.inputDataQueue.get(block=True, timeout=HpcController.WAIT_TIME)
                self.process(inputData)
            except queue.Empty:
                pass
            except Exception as ex:
                self.logger.error(f'Data processing error: {ex}')
        self.logger.debug(f'Data processing thread for worker {self.workerId} is exiting')

class RequestProcessingThread(threading.Thread):
    ''' 
    Request processing thread for the user multiprocessing worker class.
    '''

    def __init__(self, userWorkProcess):
        threading.Thread.__init__(self)
        self.userWorkProcess = userWorkProcess
        self.logger = LoggingManager.getLogger(f'rpThread.{self.userWorkProcess.workerId}')

    def run(self):
        self.logger.debug(f'Request processing thread for worker {self.userWorkProcess.workerId} starting')
        while True:
            if self.userWorkProcess.isStopped:
                break

            # Check for new request
            try:
                response = {}
                returnValue = None
                request = self.userWorkProcess.commandRequestQueue.get(block=True, timeout=HpcController.WAIT_TIME)
                self.logger.debug(f'Received request: {request}')
                command = request.get('command')
                requestId = request.get('requestId')
                response['requestId'] = requestId
                if command == HpcController.STOP_COMMAND:
                    returnValue = self.userWorkProcess.stop()
                elif command == HpcController.CONFIGURE_COMMAND:
                    configDict = request.get('configDict')
                    self.userWorkProcess.configure(configDict)
                elif command == HpcController.RESET_STATS_COMMAND:
                    self.userWorkProcess.resetStats()
                elif command == HpcController.GET_STATS_COMMAND:
                    returnValue = self.userWorkProcess.getStats()
                response['returnCode'] = HpcController.SUCCESS_RETURN_CODE
                if returnValue is not None:
                    response['returnValue'] = returnValue
            except queue.Empty:
                pass
            except Exception as ex:
                self.logger.error(f'Request processing error for worker {self.userWorkProcess.workerId}: {ex}')
                response['returnCode'] = HpcController.ERROR_RETURN_CODE
                response['error'] = str(ex)
            try:
                if len(response):
                    self.userWorkProcess.commandResponseQueue.put(response, block=True, timeout=HpcController.WAIT_TIME)
            except Exception as ex:
                self.logger.error(f'Response processing error for worker {self.userWorkProcess.workerId}: {ex}')

        self.logger.debug(f'Worker {self.userWorkProcess.workerId} is done, request processing thread is exiting')

