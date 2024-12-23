#!/usr/bin/env python

import json
import threading
import time
import pvaccess as pva
from ..utility.loggingManager import LoggingManager
from ..utility.objectUtility import ObjectUtility
from ..utility.operationMode import OperationMode
from ..utility.pvapyPrettyPrinter import PvaPyPrettyPrinter
from .sourceChannel import SourceChannel
from .dataProcessingController import DataProcessingController
from .hpcController import HpcController

class SystemController(HpcController):

    CONTROLLER_TYPE = 'system'

    CONTROL_TYPE_DICT = {
        'objectTime' : pva.DOUBLE,
        'objectTimestamp' : pva.PvTimeStamp(),
        'command' : pva.STRING,
        'args' : pva.STRING,
        'statusMessage' : pva.STRING
    }

    ALL_STATS_TYPES = [
        'receiver',
        'publisher',
        'queue',
        'metadata',
        'processor',
        'user'
    ]

    ALLOWED_INPUT_MODES = [
        OperationMode.PVA,
        OperationMode.PVAS,
        OperationMode.RPCS,
        OperationMode.CA
    ]

    ALLOWED_OUTPUT_MODES = [
        OperationMode.PVAS,
        OperationMode.PVA,
        OperationMode.RPC
    ]

    @classmethod
    def getControllerIdField(cls):
        return f'{cls.CONTROLLER_TYPE}Id'

    @classmethod
    def getControlTypeDict(cls):
        d = {cls.getControllerIdField() : pva.UINT}
        d.update(cls.CONTROL_TYPE_DICT)
        return d

    @classmethod
    def generateIdList(cls, listSpec):
        # List spec should be given either as range() spec
        # or as comma-separated list. 
        idList = listSpec
        if type(listSpec) == str:
            if not listSpec.startswith('range') and not listSpec.startswith('['):
                idList = f'[{listSpec}]'
            idList = eval(idList)
        return list(idList)

    def __init__(self, inputChannel, inputMode='pva', outputChannel=None, outputMode='pvas', statusChannel=None, controlChannel=None, processorFile=None, processorClass=None, processorArgs=None, idFormatSpec=None, objectIdField='uniqueId', objectIdOffset=0, fieldRequest='', skipInitialUpdates=1, reportStatsList='all', logLevel=None, logFile=None, disableCurses=False):
        HpcController.__init__(self, logLevel=logLevel, logFile=logFile)
        self.lock = threading.Lock()
        self.screen = None
        self.inputChannel = inputChannel
        self.inputMode = OperationMode.fromString(inputMode)
        if self.inputMode not in self.ALLOWED_INPUT_MODES:
            raise pva.InvalidArgument(f'Invalid input mode specified: {inputMode}.')
        self.logger.debug('Using input channel %s, input mode %s', inputChannel, inputMode)
        self.outputChannel = outputChannel
        self.outputMode = OperationMode.fromString(outputMode)
        if self.outputMode not in self.ALLOWED_OUTPUT_MODES:
            raise pva.InvalidArgument(f'Invalid output mode specified: {outputMode}.')
        self.logger.debug('Using output channel %s, output mode %s', outputChannel, outputMode)
        self.statusChannel = statusChannel
        self.controlChannel = controlChannel
        self.idFormatSpec = idFormatSpec
        self.processorFile = processorFile
        self.processorClass = processorClass
        self.processorArgs = processorArgs
        self.objectIdField = objectIdField
        self.objectIdOffset = objectIdOffset
        self.nSequentialUpdates = 1
        self.fieldRequest = fieldRequest
        self.skipInitialUpdates = skipInitialUpdates
        self.reportStatsList = reportStatsList
        self.disableCurses = disableCurses

        self.isStopped = True
        self.shouldBeStopped = False
        self.isRunning = False
        self.statsObjectId = 0
        self.statsEnabled = {}
        self.allStatsEnabled = 'all' in reportStatsList
        if not self.allStatsEnabled:
            for statsType in self.ALL_STATS_TYPES:
                self.statsEnabled[f'{statsType}Stats'] = statsType in reportStatsList
        self.prettyPrinter = PvaPyPrettyPrinter()
        self.hpcObject = None
        self.hpcObjectId = None

        self.screen = self.setupCurses(self.disableCurses, self.logLevel)

    def setupCurses(self, disableCurses, logLevel):
        screen = None
        if not disableCurses and not logLevel:
            try:
                import curses
                screen = curses.initscr()
                self.curses = curses
            except ImportError as ex:
                self.logger.warning('Disabling curses library: %s', str(ex))
        return screen

    def formatIdString(self, idValue):
        if self.idFormatSpec:
            return f'{idValue:{self.idFormatSpec}}'
        return f'{idValue}'

    def controlCallback(self, pv):
        t = time.time()
        if 'command' not in pv:
            statusMessage = f'Ignoring invalid request (no command specified): {pv}'
            self.logger.warning(statusMessage)
            self.controlPvObject.set({'statusMessage' : statusMessage, 'objectTime' : t, 'objectTimestamp' : pva.PvTimeStamp(t)})
            return
        command = pv['command']
        self.logger.debug('Got command: %s', command)
        if command == self.RESET_STATS_COMMAND:
            self.logger.info('Control channel: resetting %s statistics', self.CONTROLLER_TYPE)
            cTimer = threading.Timer(self.COMMAND_EXEC_DELAY, self.controlResetStats)
        elif command == self.GET_STATS_COMMAND:
            self.logger.info('Control channel: getting %s statistics', self.CONTROLLER_TYPE)
            cTimer = threading.Timer(self.COMMAND_EXEC_DELAY, self.controlGetStats)
        elif command == self.CONFIGURE_COMMAND:
            args = ''
            if 'args' not in pv:
                self.logger.debug('Empty keyword arguments string for the configure request')
            else:
                args = pv['args']
            self.logger.info('Control channel: configuring %s with args: %s', self.CONTROLLER_TYPE, args)
            cTimer = threading.Timer(self.COMMAND_EXEC_DELAY, self.controlConfigure, args=[args])
        elif command == self.STOP_COMMAND:
            self.logger.info('Control channel: stopping %s', self.CONTROLLER_TYPE)
            cTimer = threading.Timer(self.COMMAND_EXEC_DELAY, self.controlStop)
        else: 
            statusMessage = f'Ignoring invalid request (unrecognized command specified): {pv}'
            self.logger.warning(statusMessage)
            self.controlPvObject.set({'statusMessage' : statusMessage, 'objectTime' : t, 'objectTimestamp' : pva.PvTimeStamp(t)})
            return
        statusMessage = 'Command successful'
        self.controlPvObject.set({'statusMessage' : statusMessage, 'objectTime' : t, 'objectTimestamp' : pva.PvTimeStamp(t)})
        cTimer.start()

    def controlConfigure(self, configDict):
        self.logger.debug('Configuring %s %s with: %s', self.CONTROLLER_TYPE, self.hpcObjectId, configDict)
        try:
            configDict = json.loads(configDict)
            self.logger.debug('Converted configuration args string from JSON: %s', configDict)
        except Exception as ex:
            self.logger.debug('Cannot convert string %s from JSON: %s', configDict, str(ex))
        try:
            self.hpcObject.configure(configDict)
            statusMessage = 'Configuration successful'
            self.logger.debug(statusMessage)
        except Exception as ex:
            self.stopScreen()
            statusMessage = f'Configuration failed: {ex}'
            self.logger.warning(statusMessage)
        self.controlPvObject['statusMessage'] = statusMessage

    def controlResetStats(self):
        self.logger.debug('Resetting stats for %s %s', self.CONTROLLER_TYPE, self.hpcObjectId)
        self.hpcObject.resetStats()
        statusMessage = 'Stats reset successful'
        self.controlPvObject['statusMessage'] = statusMessage

    def controlGetStats(self):
        self.logger.debug('Getting stats for %s %s', self.CONTROLLER_TYPE, self.hpcObjectId)
        self.reportStats()
        statusMessage = 'Stats update successful'
        self.controlPvObject['statusMessage'] = statusMessage

    def controlStop(self):
        self.logger.debug('Stopping %s %s', self.CONTROLLER_TYPE, self.hpcObjectId)
        self.shouldBeStopped = True
        statusMessage = 'Stop flag set'
        self.controlPvObject['statusMessage'] = statusMessage

    def getStatusTypeDict(self):
        return {}

    def createStatusAndControlChannels(self, hpcObjectId):
        self.pvaServer = pva.PvaServer()
        hpcObjectIdString = self.formatIdString(hpcObjectId)
        if self.statusChannel == '_':
            self.statusChannel = f'pvapy:{self.CONTROLLER_TYPE}:{hpcObjectIdString}:status'
        if self.statusChannel:
            self.statusChannel = self.statusChannel.replace('*', hpcObjectIdString)
            self.logger.debug('Status channel name: %s', self.statusChannel)
        self.statusTypeDict = self.getStatusTypeDict()
        if self.statusChannel:
            statusPvObject = pva.PvObject(self.statusTypeDict, {f'{self.getControllerIdField()}' : hpcObjectId})
            self.pvaServer.addRecord(self.statusChannel, statusPvObject, None)
            self.logger.debug('Created %s status channel: %s', self.CONTROLLER_TYPE, self.statusChannel)

        if self.controlChannel == '_':
            self.controlChannel = f'pvapy:{self.CONTROLLER_TYPE}:{hpcObjectIdString}:control'
        if self.controlChannel:
            self.controlChannel = self.controlChannel.replace('*', hpcObjectIdString)
            self.logger.debug('Control channel name: %s', self.controlChannel)
        if self.controlChannel:
            # Keep reference to the control object so we can
            # update it
            self.controlPvObject = pva.PvObject(self.getControlTypeDict(), {f'{self.getControllerIdField()}' : hpcObjectId})
            self.pvaServer.addRecord(self.controlChannel, self.controlPvObject, self.controlCallback)
            self.logger.debug('Created %s control channel: %s', self.CONTROLLER_TYPE, self.controlChannel)

    def createDataProcessorConfig(self, processorId):
        processorConfig = {}
        if self.processorArgs:
            processorConfig = json.loads(self.processorArgs)
        processorConfig['processorId'] = processorId
        processorConfig['inputChannel'] = self.inputChannel
        processorConfig['inputMode'] = self.inputMode
        processorConfig['outputChannel'] = self.outputChannel
        processorConfig['outputMode'] = self.outputMode
        processorConfig['objectIdField'] = self.objectIdField
        processorConfig['skipInitialUpdates'] = self.skipInitialUpdates
        processorConfig['objectIdOffset'] = self.objectIdOffset
        processorConfig['nSequentialUpdates'] = self.nSequentialUpdates
        processorConfig['fieldRequest'] = self.fieldRequest
        return processorConfig

    def createDataProcessor(self, processorId):
        self.processorConfig = self.createDataProcessorConfig(processorId)
        self.logger.debug('Using processor configuration: %s', self.processorConfig)
        userDataProcessor = None
        if self.processorFile and self.processorClass:
            userDataProcessor = ObjectUtility.createObjectInstanceFromFile(self.processorFile, 'userDataProcessorModule', self.processorClass, self.processorConfig)
        elif self.processorClass:
            userDataProcessor = ObjectUtility.createObjectInstanceFromClassPath(self.processorClass, self.processorConfig)

        if userDataProcessor is not None:
            self.logger.debug('Created data processor %s: %s', processorId, userDataProcessor)
            userDataProcessor.processorId = processorId
            userDataProcessor.objectIdField = self.processorConfig['objectIdField']
        self.processingController = DataProcessingController(self.processorConfig, userDataProcessor)
        return self.processingController
            
    def getStatusTypeDict(self, processingController):
        return {}

    def start(self):
        self.lock.acquire()
        try:
            if not self.isStopped:
                self.logger.warning('Controller for hpc %s %s is already started', self.CONTROLLER_TYPE, self.hpcObjectId)
                return
            self.isStopped = False
            self.shouldBeStopped = False
            self.logger.debug('Controller for hpc %s %s is starting', self.CONTROLLER_TYPE, self.hpcObjectId)

            try: 
                self.logger.info('Starting hpc %s %s', self.CONTROLLER_TYPE, self.hpcObjectId)
                self.hpcObject.start()
                self.logger.info('Started hpc %s %s', self.CONTROLLER_TYPE, self.hpcObjectId)
            except Exception as ex:
                self.logger.warning('Could not start hpc %s %s: %s', self.CONTROLLER_TYPE, self.hpcObjectId, str(ex))
                raise

            if self.pvaServer:
                self.pvaServer.start()
        finally:
            self.lock.release()

    def filterStats(self, statsDict):
        if not self.allStatsEnabled:
            for statsType in self.ALL_STATS_TYPES:
                statsKey = f'{statsType}Stats'
                if statsKey in statsDict and not self.statsEnabled.get(statsKey):
                    del statsDict[statsKey]

    def reportStats(self, statsDict=None):
        if not statsDict:
            statsDict = self.getStats()
        statsDict[f'{self.getControllerIdField()}'] = self.hpcObjectId
        self.filterStats(statsDict)
        report = self.prettyPrinter.pformat(statsDict)

        if self.screen:
            try:
                self.screen.erase()
                self.screen.addstr(report)
                self.screen.refresh()
                return
            except Exception as ex:
                # Turn screen off on errors
                self.stopScreen()
        print(report)

    def reportCombinedSystemStats(self, statsDict):
        combinedSystemStatsDict = self.getCombinedSystemStats(statsDict)
        if not combinedSystemStatsDict:
            return
        report = self.prettyPrinter.pformat(combinedSystemStatsDict)
        print('\nCombined System Stats:\n')
        print(report)

    def getStats(self):
        return {}

    def getCombinedSystemStats(self, statsDict):
        return {}

    def processPvUpdate(self, updateWaitTime):
        return False

    def stopScreen(self):
        if self.screen:
            self.curses.endwin()
        self.screen = None

    def stop(self):
        self.lock.acquire()
        try:
            if self.isStopped:
                self.logger.warning('Controller for hpc %s %s is already stopped', self.CONTROLLER_TYPE, self.hpcObjectId)
                return
            if self.isRunning:
                # Stop running thread
                self.shouldBeStopped = True
            self.isStopped = True
            self.logger.debug('Controller for hpc %s %s is stopping', self.CONTROLLER_TYPE, self.hpcObjectId)
            try: 
                self.logger.info('Stopping hpc %s %s', self.CONTROLLER_TYPE, self.hpcObjectId)
                self.hpcObject.stop()
            except Exception as ex:
                self.logger.warning('Could not stop hpc %s %s: %s', self.CONTROLLER_TYPE, self.hpcObjectId, str(ex))
            statsDict = self.hpcObject.getStats()
            self.stopScreen()
            return statsDict
        finally:
            self.lock.release()

    def run(self, runtime=0, reportPeriod=0):
        self.lock.acquire()
        try:
            if self.isRunning:
                self.logger.warning('Controller for %s %s is already running', self.CONTROLLER_TYPE, self.hpcObjectId)
                return
            self.isRunning = True
            self.shouldBeStopped = False
        finally:
            self.lock.release()
        self.start()
        startTime = time.time()
        lastReportTime = startTime
        lastStatusUpdateTime = startTime
        waitTime = self.WAIT_TIME
        minStatusUpdatePeriod = self.MIN_STATUS_UPDATE_PERIOD
        while True:
            try:
                now = time.time()
                wakeTime = now+waitTime
                if self.shouldBeStopped:
                    break
                if runtime > 0:
                    rt = now - startTime
                    if rt > runtime:
                        break
                if reportPeriod > 0 and now-lastReportTime > reportPeriod:
                    lastReportTime = now
                    lastStatusUpdateTime = now
                    self.reportStats()

                if now-lastStatusUpdateTime > minStatusUpdatePeriod:
                    lastStatusUpdateTime = now
                    self.getStats()

                try:
                    hasProcessedObject = self.processPvUpdate(waitTime)
                    if not hasProcessedObject:
                        # Check if we need to sleep
                        delay = wakeTime-time.time()
                        if delay > 0:
                            time.sleep(delay)
                except Exception as ex:
                    self.stopScreen()
                    self.logger.error('Processing error: %s', str(ex))

            except KeyboardInterrupt as ex:
                break

        statsDict = self.stop()
        # Allow clients monitoring various channels to get last update
        time.sleep(waitTime)
        self.reportStats(statsDict)
        self.reportCombinedSystemStats(statsDict)
        self.isRunning = False
