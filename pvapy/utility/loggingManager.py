#!/usr/bin/env python

'''
Basic logging utility.
'''

import sys
import logging
from logging import handlers

class LoggingManager:
    '''
    Class that simplifies application logging by providing default
    configuration.

    Usage example:
    ::\n\n
    \tLoggingManager.setLogLevel('WARN')
    \tlogger = LoggingManager.getLogger('myLogger')
    \tlogger.debug('My first log message')
    \n\n
    '''

    _loggingFormatter = logging.Formatter('%(asctime)s %(name)s %(levelname)s: %(message)s')
    _logLevel = 'ERROR'
    _handlerMap = {}
    _loggerMap = {}

    @classmethod
    def addStreamHandler(cls, stream=sys.stdout):
        '''
        Add stream log handler.

        :Parameter: *stream* (stream object) - stream object; if not set, system standard output will be used
        '''
        handler = logging.StreamHandler(stream)
        cls.addHandler(f'stream://{stream.name}', handler)

    @classmethod
    def addFileHandler(cls, filename):
        '''
        Add file log handler (timed rotating on a daily basis).

        :Parameter: *filename* (str) - log file path
        '''
        handler = handlers.TimedRotatingFileHandler(filename, when='D', interval=1, backupCount=0, encoding=None)
        handler.setLevel(cls._logLevel)
        cls.addHandler(f'file://{filename}', handler)

    @classmethod
    def addHandler(cls, name, handler):
        '''
        Add log handler.

        :Parameter: *name* (str) - handler name
        :Parameter: *handler* (logging handler) - handler object from the logging module
        '''
        if name in cls._handlerMap:
            return
        handler.setFormatter(cls._loggingFormatter)
        handler.setLevel(cls._logLevel)
        cls._handlerMap[name] = handler
        for logger in cls._loggerMap.values():
            logger.addHandler(handler)

    @classmethod
    def getLogger(cls, name, logLevel=None, logFile=None):
        '''
        Get logger object.

        :Parameter: *name* (str) - logger name
        :Parameter: *logLevel* (str) - log level, can be one of the standard logging levels (ERROR, WARN, INFO, DEBUG); if not set, current global log level will be used
        :Parameter: *logFile* (str) - log file; if not set, standard output stream handler will be used
        :Returns: logging.Logger object
        '''
        if logLevel:
            cls.setLogLevel(logLevel)
        if logFile:
            cls.addFileHandler(logFile)
        else:
            cls.addStreamHandler(sys.stdout)
        logger = cls._loggerMap.get(name)
        if logger:
            return logger
        logger = logging.getLogger(name)
        logger.setLevel(cls._logLevel)
        for handler in cls._handlerMap.values():
            logger.addHandler(handler)
        cls._loggerMap[name] = logger
        return logger

    @classmethod
    def setLogLevel(cls, logLevel):
        '''
        Set global log level. This method will update all known logger
        instances.

        :Parameter: *logLevel* (str) - log level, can be one of the standard logging levels (ERROR, WARN, INFO, DEBUG)
        '''
        cls._logLevel = logLevel.upper()
        for logger in cls._loggerMap.values():
            logger.setLevel(cls._logLevel)
        for handler in cls._handlerMap.values():
            handler.setLevel(cls._logLevel)
