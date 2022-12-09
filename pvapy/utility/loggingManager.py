#!/usr/bin/env python

import sys
import logging

# Basic logging utility
class LoggingManager:

    formatter = logging.Formatter('%(asctime)s %(name)s %(levelname)s: %(message)s')
    logLevel = logging.DEBUG
    handlerMap = {}
    loggerMap = {}

    @classmethod
    def addStreamHandler(cls, stream=sys.stdout):
        handler = logging.StreamHandler(stream)
        cls.addHandler('stream://%s' % stream.name, handler)

    @classmethod
    def addFileHandler(cls, filename):
        from logging import handlers
        handler = handlers.TimedRotatingFileHandler(filename, when='D', interval=1, backupCount=0, encoding=None)
        handler.setLevel(cls.logLevel)
        cls.addHandler('file://%s' % filename, handler)

    @classmethod
    def addHandler(cls, name, handler):
        if name in cls.handlerMap:
            return
        handler.setFormatter(cls.formatter)
        handler.setLevel(cls.logLevel)
        cls.handlerMap[name] = handler
        for (name,logger) in cls.loggerMap.items():
            logger.addHandler(handler)

    @classmethod
    def getLogger(cls, name, logLevel=None, logFile=None):
        if logLevel:
            cls.setLogLevel(logLevel)
            if logFile:
                cls.addFileHandler(logFile)
            else:
                cls.addStreamHandler()
        logger = cls.loggerMap.get(name)
        if logger:
            return logger
        logger = logging.getLogger(name)
        logger.setLevel(cls.logLevel)
        for (hName,handler) in cls.handlerMap.items():
            logger.addHandler(handler)
        cls.loggerMap[name] = logger
        return logger

    @classmethod
    def setLogLevel(cls, logLevel):
        cls.logLevel = logLevel.upper()
        for (name,logger) in cls.loggerMap.items():
            logger.setLevel(cls.logLevel)
        for (name,handler) in cls.handlerMap.items():
            handler.setLevel(cls.logLevel)

