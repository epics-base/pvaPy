#!/usr/bin/env python

import os
import queue
import multiprocessing as mp
from ..utility.loggingManager import LoggingManager
from .userMpWorker import UserMpWorker
from .hpcController import HpcController

class UserMpWorkerController(HpcController):

    CONTROLLER_TYPE = 'user'

    ''' 
    Controller class for user work process.
  
    **UserMpWorkerController(workerId, userMpDataProcessor, inputDataQueue, logLevel=None, logFile=None)**

    :Parameter: *workerId* (str) - Worker id.
    :Parameter: *userMpDataProcessor* (UserMpDataProcessor) - Instance of the UserMpDataProcessor class that will be processing data.
    :Parameter: *inputDataQueue* (multiprocessing.Queue) - Input data queue.
    :Parameter: *logLevel* (str) - Log level; possible values: debug, info, warning, error, critical. If not provided, there will be no log output.
    :Parameter: *logFile* (str) - Log file.
    '''
    def __init__(self, workerId, userMpDataProcessor, inputDataQueue, logLevel=None, logFile=None):
        HpcController.__init__(self, logLevel, logFile) 
        self.workerId = workerId 
        self.inputDataQueue = inputDataQueue
        self.commandRequestQueue = mp.Queue()
        self.commandResponseQueue = mp.Queue()
        self.requestId = 0
        self.uwProcess = UserMpWorker(workerId, userMpDataProcessor, self.commandRequestQueue, self.commandResponseQueue, inputDataQueue, logLevel, logFile)
        self.pid = os.getpid()
        self.statsDict = {}
        self.isStopped = True

    class ProcessNotResponding(Exception):
        def __init__(self, args):
            Exception.__init__(self, args)

    def _invokeCommandRequest(self, command, args={}):
        returnCode = None
        try:
            requestId = self.requestId
            self.requestId += 1
            request = {'requestId' : requestId, 'command' : command}
            if args is not None and type(args) == dict:
                request.update(args)
            self.commandRequestQueue.put(request, block=True, timeout=self.WAIT_TIME)
            response = self.commandResponseQueue.get(block=True, timeout=self.WAIT_TIME)
            returnCode = response.get('returnCode', self.ERROR_RETURN_CODE)
            returnValue = response.get('returnValue')
            requestId2 = response.get('requestId')
        except queue.Empty:
            pass
        except Exception as ex:
            self.logger.error(f'Error invoking command request {request} for worker {self.workerId}: {ex}')
        if returnCode is None:
            raise self.ProcessNotResponding(f'No response from worker {self.workerId}')
        if returnCode != self.SUCCESS_RETURN_CODE:
            error = response.get('error', '')
            raise Exception(f'Error response from worker {self.workerId}: {error}')
        elif requestId2 != requestId:
            raise Exception(f'Invalid response from worker {self.workerId}: request id {requestId} != {requestId2}')
        return returnValue

    def start(self):
        # Replace interrupt handler for worker processes
        # so we can exit cleanly
        import signal
        originalSigintHandler = signal.signal(signal.SIGINT, signal.SIG_IGN)
        self.logger.debug(f'Starting worker {self.workerId}')
        self.uwProcess.start()
        self.logger.debug(f'Started user work process: {self.uwProcess}')
        signal.signal(signal.SIGINT, originalSigintHandler)
        self.isStopped = False

    def getStats(self, keyPrefix=None):
        statsDict = self.statsDict
        try:
            if not self.isStopped:
                statsDict = self._invokeCommandRequest(self.GET_STATS_COMMAND)
                self.statsDict = statsDict
        except Exception as ex:
            self.logger.warn(f'Cannot get stats for worker {self.workerId}: {ex}')
        if keyPrefix:
            statsDict2 = {}
            for key, value in statsDict.items():
                statsDict2[f'{keyPrefix}_{key}'] = value
            return statsDict2
        return statsDict

    def resetStats(self):
        try:
            self._invokeCommandRequest(self.RESET_STATS_COMMAND)
        except Exception as ex:
            self.logger.error(f'Cannot reset stats for worker {self.workerId}: {ex}')

    def configure(configDict):
        try:
            self._invokeCommandRequest(self.CONFIGURE_COMMAND, {'configDict' : configDict})
        except Exception as ex:
            self.logger.error(f'Cannot configure worker {self.workerId}: {ex}')

    def stop(self):
        self.logger.debug(f'Stopping user work process: {self.uwProcess}')
        statsDict = self.statsDict
        try:
            statsDict = self._invokeCommandRequest(self.STOP_COMMAND)
        except self.ProcessNotResponding as ex:
            pass
        except Exception as ex:
            self.logger.warn(f'Cannot stop worker {self.workerId}: {ex}')
        try:
            self.logger.debug(f'Waiting on child process pid {self.uwProcess.pid} (my pid: {self.pid})')
            self.uwProcess.join(self.WAIT_TIME)
        except:
            pass
        self.isStopped = True
        self.logger.debug(f'User work process {self.workerId} is done')
        return statsDict

