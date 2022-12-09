#!/usr/bin/env python

import threading
import queue
import os
import multiprocessing as mp
from ..utility.loggingManager import LoggingManager
from .baseController import BaseController

class UserWorkProcess(mp.Process):

    ''' 
    User work process class.
  
    **UserWorkProcess(workerId, userDataProcessor, commandRequestQueue, commandResponseQueue, inputDataQueue, outputDataQueue, logLevel=None, logFile=None)**

    :Parameter: *workerId* (str) - Worker id.
    :Parameter: *userDataProceessor* (UserDataProcessor) - Instance of the UserDataProcessor class that will be processing data.
    :Parameter: *commandRequestQueue* (multiprocessing.Queue) - Command request queue.
    :Parameter: *commandResponseQueue* (multiprocessing.Queue) - Command response queue.
    :Parameter: *inputDataQueue* (multiprocessing.Queue) - Input data queue.
    :Parameter: *outputDataQueue* (multiprocessing.Queue) - Output data queue.
    :Parameter: *logLevel* (str) - Log level; possible values: debug, info, warning, error, critical. If not provided, there will be no log output.
    :Parameter: *logFile* (str) - Log file.
    '''
    def __init__(self, workerId, userDataProcessor, commandRequestQueue, commandResponseQueue, inputDataQueue, outputDataQueue, logLevel=None, logFile=None):
 
        mp.Process.__init__(self) 
        self.logger = LoggingManager.getLogger(self.__class__.__name__, logLevel, logFile)
        self.workerId = workerId 
        self.userDataProcessor = userDataProcessor

        self.inputDataQueue = inputDataQueue
        self.outputDataQueue = outputDataQueue
        self.commandRequestQueue = commandRequestQueue
        self.commandResponseQueue = commandResponseQueue
        self.isStopped = True
        self.rpThread = RequestProcessingThread(self)

    def start(self):
        if self.isStopped:
            self.isStopped = False
            self.userDataProcessor.start()
            mp.Process.start(self)

    def getStats(self):
        return self.userDataProcessor.getStats()

    def resetStats(self):
        self.userDataProcessor.resetStats()

    def configure(configDict):
        self.userDataProcessor.configure(configDict)

    def process(self, data):
        return self.userDataProcessor.process(data)

    def stop(self):
        if not self.isStopped:
            self.isStopped = True
            self.userDataProcessor.stop()
        return self.getStats()

    def run(self):
        self.logger.debug(f'Data processing thread for worker {self.workerId} starting, PID: {os.getpid()}')
        self.rpThread.start()
        while True:
            if self.isStopped:
                break
            try:
                inputData = self.inputDataQueue.get(block=True, timeout=BaseController.WAIT_TIME)
                outputData = self.process(inputData)
                if outputData is not None:
                    self.outputDataQueue.put(outputData, block=False)
            except queue.Empty:
                pass
            except Exception as ex:
                self.logger.error(f'Data processing error: {ex}')
        self.logger.debug(f'Data processing thread for worker {self.workerId} is exiting')

class RequestProcessingThread(threading.Thread):

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
                request = self.userWorkProcess.commandRequestQueue.get(block=True, timeout=BaseController.WAIT_TIME)
                self.logger.debug(f'Received request: {request}')
                command = request.get('command')
                requestId = request.get('requestId')
                response['requestId'] = requestId
                if command == BaseController.STOP_COMMAND:
                    returnValue = self.userWorkProcess.stop()
                elif command == BaseController.CONFIGURE_COMMAND:
                    configDict = request.get('configDict')
                    self.userWorkProcess.configure(configDict)
                elif command == BaseController.RESET_STATS_COMMAND:
                    self.userWorkProcess.resetStats()
                elif command == BaseController.GET_STATS_COMMAND:
                    returnValue = self.userWorkProcess.getStats()
                response['returnCode'] = BaseController.SUCCESS_RETURN_CODE
                if returnValue is not None:
                    response['returnValue'] = returnValue
            except queue.Empty:
                pass
            except Exception as ex:
                self.logger.error(f'Request processing error for worker {self.userWorkProcess.workerId}: {ex}')
                response['returnCode'] = BaseController.ERROR_RETURN_CODE
                response['error'] = str(ex)
            try:
                if len(response):
                    self.userWorkProcess.commandResponseQueue.put(response, block=True, timeout=BaseController.WAIT_TIME)
            except Exception as ex:
                self.logger.error(f'Response processing error for worker {self.userWorkProcess.workerId}: {ex}')

        self.logger.debug(f'Worker {self.userWorkProcess.workerId} is done, request processing thread is exiting')

