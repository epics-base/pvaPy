#!/usr/bin/env python

import threading
from ..utility.loggingManager import LoggingManager

class HpcController:

    WAIT_TIME = 1.0
    MIN_STATUS_UPDATE_PERIOD = 10.0
    COMMAND_EXEC_DELAY = 0.1

    SUCCESS_RETURN_CODE = 0
    ERROR_RETURN_CODE = 1

    GET_STATS_COMMAND = 'get_stats'
    RESET_STATS_COMMAND = 'reset_stats'
    CONFIGURE_COMMAND = 'configure'
    STOP_COMMAND = 'stop'

    CONTROLLER_TYPE = 'hpc'

    def __init__(self, logLevel=None, logFile=None):
        self.logLevel = logLevel
        self.logFile = logFile
        self.logger = LoggingManager.getLogger(self.__class__.__name__, logLevel, logFile)

    def configure(self, configDict):
        pass

    def start(self):
        pass

    def reportStats(self, statsDict=None):
        pass

    def getStats(self):
        return {}

    def stop(self):
        pass

