#!/usr/bin/env python

import json
import threading
import time
import pvaccess as pva
from ..utility.loggingManager import LoggingManager
from ..utility.objectUtility import ObjectUtility
from ..utility.pvapyPrettyPrinter import PvaPyPrettyPrinter
from ..hpc.sourceChannel import SourceChannel
from ..hpc.dataProcessingController import DataProcessingController

class HpcController:

    WAIT_TIME = 1.0
    MIN_STATUS_UPDATE_PERIOD = 10.0
    COMMAND_EXEC_DELAY = 0.1

    GET_STATS_COMMAND = 'get_stats'
    RESET_STATS_COMMAND = 'reset_stats'
    CONFIGURE_COMMAND = 'configure'
    STOP_COMMAND = 'stop'

    CONTROLLER_TYPE = 'controller'

    CONTROL_TYPE_DICT = {
        'objectTime' : pva.DOUBLE,
        'objectTimestamp' : pva.PvTimeStamp(),
        'command' : pva.STRING,
        'args' : pva.STRING,
        'statusMessage' : pva.STRING
    }

    @classmethod
    def getControllerIdField(cls):
        return f'{cls.CONTROLLER_TYPE}Id'

    @classmethod
    def getControlTypeDict(cls):
        d = {cls.getControllerIdField() : pva.UINT}
        d.update(cls.CONTROL_TYPE_DICT)
        return d

    def __init__(self, args):
        self.lock = threading.Lock()
        self.screen = None
        if args.log_level:
            LoggingManager.setLogLevel(args.log_level)
            if args.log_file:
                LoggingManager.addFileHandler(args.log_file)
            else:
                LoggingManager.addStreamHandler()
        self.logger = LoggingManager.getLogger(self.__class__.__name__)
        self.screen = self.setupCurses(args)
        self.args = args
        self.isStopped = True
        self.shouldBeStopped = False
        self.isRunning = False
        self.statsObjectId = 0
        self.statsEnabled = {}
        for statsType in ['monitor','queue','processor','user']:
            self.statsEnabled[f'{statsType}Stats'] = 'all' in args.report_stats or statsType in args.report_stats
        self.prettyPrinter = PvaPyPrettyPrinter()
        self.hpcObject = None
        self.hpcObjectId = None

    def setupCurses(self, args):
        screen = None
        if not args.disable_curses and not args.log_level:
            try:
                import curses
                screen = curses.initscr()
                self.curses = curses
            except ImportError as ex:
                self.logger.warning(f'Disabling curses library: {ex}')
        return screen

    def controlCallback(self, pv):
        t = time.time()
        if 'command' not in pv:
            statusMessage = f'Ignoring invalid request (no command specified): {pv}'
            self.logger.warning(statusMessage)
            self.controlPvObject.set({'statusMessage' : statusMessage, 'objectTime' : t, 'objectTimestamp' : pva.PvTimeStamp(t)})
            return
        command = pv['command']
        self.logger.debug(f'Got command: {command}')
        if command == self.RESET_STATS_COMMAND:
            self.logger.info(f'Control channel: resetting {self.CONTROLLER_TYPE} statistics')
            cTimer = threading.Timer(self.COMMAND_EXEC_DELAY, self.controlResetStats)
        elif command == self.GET_STATS_COMMAND:
            self.logger.info(f'Control channel: getting {self.CONTROLLER_TYPE} statistics')
            cTimer = threading.Timer(self.COMMAND_EXEC_DELAY, self.controlGetStats)
        elif command == self.CONFIGURE_COMMAND:
            args = ''
            if 'args' not in pv:
                self.logger.debug('Empty keyword arguments string for the configure request')
            else:
                args = pv['args']
            self.logger.info(f'Control channel: configuring {self.CONTROLLER_TYPE} with args: {args}')
            cTimer = threading.Timer(self.COMMAND_EXEC_DELAY, self.controlConfigure, args=[args])
        elif command == self.STOP_COMMAND:
            self.logger.info(f'Control channel: stopping {self.CONTROLLER_TYPE}')
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
        self.logger.debug(f'Configuring {self.CONTROLLER_TYPE} {self.hpcObjectId} with: {configDict}')
        try:
            configDict = json.loads(configDict)
            self.logger.debug(f'Converted configuration args string from JSON: {configDict}')
        except Exception as ex:
            self.logger.debug(f'Cannot convert string {configDict} from JSON: {ex}')
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
        self.logger.debug(f'Resetting stats for {self.CONTROLLER_TYPE} {self.hpcObjectId}')
        self.hpcObject.resetStats()
        statusMessage = 'Stats reset successful'
        self.controlPvObject['statusMessage'] = statusMessage

    def controlGetStats(self):
        self.logger.debug(f'Getting stats for {self.CONTROLLER_TYPE} {self.hpcObjectId}')
        self.reportStats()
        statusMessage = 'Stats update successful'
        self.controlPvObject['statusMessage'] = statusMessage

    def controlStop(self):
        self.logger.debug(f'Stopping {self.CONTROLLER_TYPE} {self.hpcObjectId}')
        self.shouldBeStopped = True
        statusMessage = 'Stop flag set'
        self.controlPvObject['statusMessage'] = statusMessage

    def getStatusTypeDict(self):
        return {}

    def createOutputChannels(self, hpcObjectId, args):
        self.pvaServer = pva.PvaServer()
        self.statusChannel = args.status_channel
        if self.statusChannel == '_':
            self.statusChannel = f'pvapy:{self.CONTROLLER_TYPE}:{hpcObjectId}:status'
        if self.statusChannel:
            self.statusChannel = self.statusChannel.replace('*', f'{hpcObjectId}')
            self.logger.debug(f'Status channel name: {self.statusChannel}')
        if self.statusChannel:
            self.statusTypeDict = self.getStatusTypeDict()
            statusPvObject = pva.PvObject(self.statusTypeDict, {f'{self.getControllerIdField()}' : hpcObjectId})
            self.pvaServer.addRecord(self.statusChannel, statusPvObject)
            self.logger.debug(f'Created {self.CONTROLLER_TYPE} status channel: {self.statusChannel}')

        self.controlChannel = args.control_channel
        if self.controlChannel == '_':
            self.controlChannel = f'pvapy:{self.CONTROLLER_TYPE}:{hpcObjectId}:control'
        if self.controlChannel:
            self.controlChannel = self.controlChannel.replace('*', f'{hpcObjectId}')
            self.logger.debug(f'Control channel name: {self.controlChannel}')
        if self.controlChannel:
            # Keep reference to the control object so we can
            # update it
            self.controlPvObject = pva.PvObject(self.getControlTypeDict(), {f'{self.getControllerIdField()}' : hpcObjectId})
            self.pvaServer.addRecord(self.controlChannel, self.controlPvObject, self.controlCallback)
            self.logger.debug(f'Created {self.CONTROLLER_TYPE} control channel: {self.controlChannel}')

    def createDataProcessorConfig(self, processorId, args):
        processorConfig = {}
        if args.processor_args:
            processorConfig = json.loads(args.processor_args)
        processorConfig['inputChannel'] = inputChannel
        if not 'processorId' in processorConfig:
            processorConfig['processorId'] = processorId
        self.processorConfig = processorConfig
        return processorConfig

    def createDataProcessor(self, processorId, args):
        processorConfig = self.createDataProcessorConfig(processorId, args)
        self.logger.debug(f'Using processor configuration: {processorConfig}')
        userDataProcessor = None
        if args.processor_file and args.processor_class:
            userDataProcessor = ObjectUtility.createObjectInstanceFromFile(args.processor_file, 'userDataProcessorModule', args.processor_class, processorConfig)
        elif args.processor_class:
            userDataProcessor = ObjectUtility.createObjectInstanceFromClassPath(args.processor_class, processorConfig)

        if userDataProcessor is not None:
            self.logger.debug(f'Created data processor {processorId}: {userDataProcessor}')
            userDataProcessor.processorId = processorId
            userDataProcessor.objectIdField = processorConfig['objectIdField']
        processingController = DataProcessingController(processorConfig, userDataProcessor)
        if processingController:
            processingController.pvaServer = self.pvaServer
            processingController.createUserDefinedOutputChannel()
        return processingController
            
    def getStatusTypeDict(self, processingController):
        return {}

    def start(self):
        self.lock.acquire()
        try:
            if not self.isStopped:
                self.logger.warn(f'Controller for hpc {self.CONTROLLER_TYPE} {self.hpcObjectId} is already started')
                return
            self.isStopped = False
            self.shouldBeStopped = False
            self.logger.debug(f'Controller for hpc {self.CONTROLLER_TYPE} {self.hpcObjectId} is starting')

            try: 
                self.logger.info(f'Starting hpc {self.CONTROLLER_TYPE} {self.hpcObjectId}')
                self.hpcObject.start()
                self.logger.info(f'Started hpc {self.CONTROLLER_TYPE} {self.hpcObjectId}')
            except Exception as ex:
                self.logger.warn(f'Could not start hpc {self.CONTROLLER_TYPE} {self.hpcObjectId}: {ex}')
                raise

            if self.pvaServer:
                self.pvaServer.start()
        finally:
            self.lock.release()

    def reportStats(self, statsDict=None):
        if not statsDict:
            statsDict = self.getStats()
        statsDict[f'{self.getControllerIdField()}'] = self.hpcObjectId
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

    def getStats(self):
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
                self.logger.warn(f'Controller for hpc {self.CONTROLLER_TYPE} {self.hpcObjectId} is already stopped')
                return
            if self.isRunning:
                # Stop running thread
                self.shouldBeStopped = True
            self.isStopped = True
            self.logger.debug(f'Controller for hpc {self.CONTROLLER_TYPE} {self.hpcObjectId} is stopping')
            try: 
                self.logger.info(f'Stopping hpc {self.CONTROLLER_TYPE} {self.hpcObjectId}')
                self.hpcObject.stop()
            except Exception as ex:
                self.logger.warn(f'Could not stop hpc {self.CONTROLLER_TYPE} {self.hpcObjectId}')
            statsDict = self.hpcObject.getStats()
            self.stopScreen()
            return statsDict
        finally:
            self.lock.release()

    def run(self, runtime=0, reportPeriod=0):
        self.lock.acquire()
        try:
            if self.isRunning:
                self.logger.warn(f'Controller for {self.CONTROLLER_TYPE} {self.hpcObjectId} is already running')
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
                    self.logger.error(f'Processing error: {ex}')

            except KeyboardInterrupt as ex:
                break

        print()
        statsDict = self.stop()
        # Allow clients monitoring various channels to get last update
        time.sleep(waitTime)
        self.reportStats(statsDict)
        self.isRunning = False
