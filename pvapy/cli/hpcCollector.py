#!/usr/bin/env python

import curses
import json
import pprint
import argparse
import threading
import time
import json
import queue
import pvaccess as pva
import multiprocessing as mp
from ..utility.loggingManager import LoggingManager
from ..utility.objectUtility import ObjectUtility
from ..utility.pvapyPrettyPrinter import PvaPyPrettyPrinter
from ..hpc.dataCollector import DataCollector
from ..hpc.dataProcessingController import DataProcessingController

__version__ = pva.__version__

WAIT_TIME = 1.0
MIN_STATUS_UPDATE_PERIOD = 10.0
COMMAND_EXEC_DELAY = 0.1

GET_STATS_COMMAND = 'get_stats'
RESET_STATS_COMMAND = 'reset_stats'
CONFIGURE_COMMAND = 'configure'
STOP_COMMAND = 'stop'

class CollectorController:

    COLLECTOR_CONTROL_TYPE_DICT = {
        'collectorId' : pva.UINT,
        'objectTime' : pva.DOUBLE,
        'objectTimestamp' : pva.PvTimeStamp(),
        'command' : pva.STRING,
        'args' : pva.STRING,
        'statusMessage' : pva.STRING
    }

    PRODUCER_STATUS_TYPE_DICT = {
        'producerId' : pva.UINT,
        'monitorStats' : {
            'nReceived' : pva.UINT,
            'receivedRate' : pva.DOUBLE,
            'nOverruns' : pva.UINT, 
            'overrunRate' : pva.DOUBLE
        },
        'queueStats' : {
            'nReceived' : pva.UINT,
            'nRejected' : pva.UINT,
            'nDelivered' : pva.UINT,
            'nQueued' : pva.UINT
        },
    }

    COLLECTOR_STATUS_TYPE_DICT = {
        'collectorId' : pva.UINT,
        'objectId' : pva.UINT,
        'objectTime' : pva.DOUBLE,
        'objectTimestamp' : pva.PvTimeStamp(),
        'collectorStats' : {
            'nCollected' : pva.UINT, 
            'collectedRate' : pva.DOUBLE,
            'nRejected' : pva.UINT, 
            'rejectedRate' : pva.DOUBLE,
            'nMissed' : pva.UINT, 
            'missedRate' : pva.DOUBLE
        },
        'processorStats' : {
            'runtime' : pva.DOUBLE,
            'startTime' : pva.DOUBLE,
            'endTime' : pva.DOUBLE,
            'receivingTime' : pva.DOUBLE,
            'firstObjectTime' : pva.DOUBLE, 
            'lastObjectTime' : pva.DOUBLE,
            'firstObjectId' : pva.UINT,
            'lastObjectId' : pva.UINT,
            'nProcessed' : pva.UINT,
            'processedRate' : pva.DOUBLE,
            'nErrors' : pva.UINT,
            'errorRate' : pva.DOUBLE,
            'nMissed' : pva.UINT, 
            'missedRate' : pva.DOUBLE
        }
    }

    def __init__(self, args):
        self.screen = None
        if args.log_level:
            LoggingManager.setLogLevel(args.log_level)
            if args.log_file:
                LoggingManager.addFileHandler(args.log_file)
                if not args.disable_curses:
                    self.screen = curses.initscr()
            else:
                LoggingManager.addStreamHandler()
        else:
            if not args.disable_curses:
                self.screen = curses.initscr()

        self.logger = LoggingManager.getLogger(self.__class__.__name__)
        self.args = args
        self.isDone = False
        self.statsObjectId = 0
        self.statsEnabled = {}
        for statsType in ['monitor','queue','processor','user']:
            self.statsEnabled[f'{statsType}Stats'] = 'all' in args.report_stats or statsType in args.report_stats

    def controlCallback(self, pv):
        t = time.time()
        if 'command' not in pv:
            statusMessage = f'Ignoring invalid request (no command specified): {pv}'
            self.logger.warning(statusMessage)
            self.controlPvObject.set({'statusMessage' : statusMessage, 'objectTime' : t, 'objectTimestamp' : pva.PvTimeStamp(t)})
            return
        command = pv['command']
        self.logger.debug(f'Got command: {command}')
        if command == RESET_STATS_COMMAND:
            self.logger.info('Control channel: resetting collector statistics')
            cTimer = threading.Timer(COMMAND_EXEC_DELAY, self.controlResetStats)
        elif command == GET_STATS_COMMAND:
            self.logger.info('Control channel: getting collector statistics')
            cTimer = threading.Timer(COMMAND_EXEC_DELAY, self.controlGetStats)
        elif command == CONFIGURE_COMMAND:
            args = ''
            if 'args' not in pv:
                self.logger.debug('Empty keyword arguments string for the configure request')
            else:
                args = pv['args']
            self.logger.info(f'Control channel: configuring collector with args: {args}')
            cTimer = threading.Timer(COMMAND_EXEC_DELAY, self.controlConfigure, args=[args])
        elif command == STOP_COMMAND:
            self.logger.info(f'Control channel: stopping collector')
            cTimer = threading.Timer(COMMAND_EXEC_DELAY, self.controlStop)
        else: 
            statusMessage = f'Ignoring invalid request (unrecognized command specified): {pv}'
            self.logger.warning(statusMessage)
            self.controlPvObject.set({'statusMessage' : statusMessage, 'objectTime' : t, 'objectTimestamp' : pva.PvTimeStamp(t)})
            return
        statusMessage = 'Command successful'
        self.controlPvObject.set({'statusMessage' : statusMessage, 'objectTime' : t, 'objectTimestamp' : pva.PvTimeStamp(t)})
        cTimer.start()

    def controlConfigure(self, configDict):
        self.logger.debug(f'Configuring collector {self.dataCollector.collectorId} with: {configDict}')
        try:
            configDict = json.loads(configDict)
            self.logger.debug(f'Converted configuration args string from JSON: {configDict}')
        except Exception as ex:
            self.logger.debug(f'Cannot convert string {configDict} from JSON: {ex}')
        try:
            self.dataCollector.configure(configDict)
            statusMessage = 'Configuration successful'
            self.logger.debug(statusMessage)
        except Exception as ex:
            self.stopScreen()
            statusMessage = f'Configuration failed: {ex}'
            self.logger.warning(statusMessage)
        self.controlPvObject['statusMessage'] = statusMessage

    def controlResetStats(self):
        self.logger.debug(f'Resetting stats for collector {self.dataCollector.collectorId}')
        self.dataCollector.resetStats()
        statusMessage = 'Stats reset successful'
        self.controlPvObject['statusMessage'] = statusMessage

    def controlGetStats(self):
        self.logger.debug(f'Getting stats for collector {self.dataCollector.collectorId}')
        self.reportCollectorStats()
        statusMessage = 'Stats update successful'
        self.controlPvObject['statusMessage'] = statusMessage

    def controlStop(self):
        self.logger.debug(f'Stopping collector {self.dataCollector.collectorId}')
        self.isDone = True
        statusMessage = 'Stop flag set'
        self.controlPvObject['statusMessage'] = statusMessage

    def createProcessorConfig(self, collectorId, args):
        inputChannel = args.input_channel
        self.logger.debug(f'Input channel: {inputChannel}')

        outputChannel = args.output_channel
        if outputChannel == '_':
            outputChannel = f'pvapy:collector:{collectorId}:output'
        if outputChannel:
            outputChannel = outputChannel.replace('*', f'{collectorId}')
            self.logger.debug(f'Processor output channel name: {outputChannel}')

        processorConfig = {}
        if args.processor_args:
            processorConfig = json.loads(args.processor_args)
        processorConfig['inputChannel'] = inputChannel
        if not 'processorId' in processorConfig:
            processorConfig['processorId'] = collectorId
        if not 'skipInitialUpdates' in processorConfig:
            processorConfig['skipInitialUpdates'] = args.skip_initial_updates
        if not 'objectIdField' in processorConfig:
            processorConfig['objectIdField'] = args.oid_field
        if not 'objectIdOffset' in processorConfig:
            processorConfig['objectIdOffset'] = args.oid_offset
        if not 'fieldRequest' in processorConfig:
            processorConfig['fieldRequest'] = args.field_request
        if not 'outputChannel' in processorConfig:
            processorConfig['outputChannel'] = outputChannel
        self.processorConfig = processorConfig
        return processorConfig

    def createProcessor(self, collectorId, args):
        # Create config dict
        processorConfig = self.createProcessorConfig(collectorId, args)
    
        self.logger.debug(f'Using processor configuration: {processorConfig}')
        userDataProcessor = None
        if args.processor_file and args.processor_class:
            userDataProcessor = ObjectUtility.createObjectInstanceFromFile(args.processor_file, 'userDataProcessorModule', args.processor_class, processorConfig)
        elif args.processor_class:
            userDataProcessor = ObjectUtility.createObjectInstanceFromClassPath(args.processor_class, processorConfig)

        if userDataProcessor is not None:
            self.logger.debug(f'Created data processor {collectorId}: {userDataProcessor}')
            userDataProcessor.processorId = collectorId
            userDataProcessor.collectorId = collectorId
            userDataProcessor.objectIdField = processorConfig['objectIdField']
        processingController = DataProcessingController(processorConfig, userDataProcessor)
        return processingController
            
    def getCollectorStatusTypeDict(self, processingController):
        statusTypeDict = self.COLLECTOR_STATUS_TYPE_DICT
        if processingController:
            userStatsTypeDict = processingController.getUserStatsPvaTypes()
            if userStatsTypeDict:
                statusTypeDict['userStats'] = processingController.getUserStatsPvaTypes()
        for producerId in self.producerIdList:
            statusTypeDict[f'producerStats_{producerId}'] = self.PRODUCER_STATUS_TYPE_DICT
        return statusTypeDict

    def getProducerIdList(self, args):
        # Evaluate producer id list; it should be given either as range() spec
        # or as comma-separated list.
        producerIdList = args.producer_id_list
        if not producerIdList.startswith('range') and not producerIdList.startswith('['):
            producerIdList = f'[{producerIdList}]'
        producerIdList = list(eval(producerIdList))
        return producerIdList

    def createCollector(self, collectorId, args):
        processingController = self.createProcessor(collectorId, args)
        inputChannel = args.input_channel
        self.logger.debug(f'Input channel name: {inputChannel}')

        self.producerIdList = self.getProducerIdList(args)
        self.logger.debug(f'Producer id list: {self.producerIdList}')

        self.pvaServer = pva.PvaServer()
        self.statusChannel = args.status_channel
        if self.statusChannel == '_':
            self.statusChannel = f'pvapy:collector:{collectorId}:status'
        if self.statusChannel:
            self.statusChannel = self.statusChannel.replace('*', f'{collectorId}')
            self.logger.debug(f'Collector status channel name: {self.statusChannel}')
        if self.statusChannel:
            self.statusTypeDict = self.getCollectorStatusTypeDict(processingController)
            statusPvObject = pva.PvObject(self.statusTypeDict, {'collectorId' : collectorId})
            self.pvaServer.addRecord(self.statusChannel, statusPvObject)
            self.logger.debug(f'Created collector status channel: {self.statusChannel}')

        self.controlChannel = args.control_channel
        if self.controlChannel == '_':
            self.controlChannel = f'pvapy:collector:{collectorId}:control'
        if self.controlChannel:
            self.controlChannel = self.controlChannel.replace('*', f'{collectorId}')
            self.logger.debug(f'Collector control channel name: {self.controlChannel}')
        if self.controlChannel:
            # Keep reference to the control object so we can
            # update it
            self.controlPvObject = pva.PvObject(self.COLLECTOR_CONTROL_TYPE_DICT, {'collectorId' : collectorId})
            self.pvaServer.addRecord(self.controlChannel, self.controlPvObject, self.controlCallback)
            self.logger.debug(f'Created collector control channel: {self.controlChannel}')

        # Share PVA server
        if processingController:
            processingController.pvaServer = self.pvaServer
            processingController.createUserDefinedOutputChannel()

        objectIdField = self.processorConfig['objectIdField']
        objectIdOffset = self.processorConfig['objectIdOffset']
        fieldRequest = self.processorConfig['fieldRequest']
        self.dataCollector = DataCollector(collectorId, inputChannel, producerIdList=self.producerIdList, objectIdField=objectIdField, objectIdOffset=objectIdOffset, fieldRequest=fieldRequest, serverQueueSize=args.server_queue_size, monitorQueueSize=args.monitor_queue_size, collectorCacheSize=args.collector_cache_size, processingController=processingController)
        return self.dataCollector

    def startCollectors(self):
        self.createCollector(self.args.collector_id, args=self.args)
        self.dataCollector.start()
        if self.pvaServer:
            self.pvaServer.start()
        self.logger.info(f'Started collector {self.dataCollector.collectorId}')

    def reportCollectorStats(self, statsDict=None):
        if not statsDict:
            statsDict = self.getCollectorStats()
        collectorId = self.dataCollector.collectorId
        statsDict['collectorId'] = collectorId
        pp = PvaPyPrettyPrinter()
        report = pp.pformat(statsDict)

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

    def getCollectorStats(self):
        statsDict = self.dataCollector.getStats()
        self.statsObjectId += 1
        statsDict['objectId'] = self.statsObjectId
        t = time.time()
        if self.pvaServer:
            collectorId = self.dataCollector.collectorId
            statusObject = pva.PvObject(self.statusTypeDict, {'collectorId' : collectorId, 'objectId' : self.statsObjectId, 'objectTime' : t, 'objectTimestamp' : pva.PvTimeStamp(t)})
            statusObject['processorStats'] = statsDict.get('processorStats', {})
            userStatsPvaTypes = self.statusTypeDict.get('userStats', {})
            if userStatsPvaTypes: 
                userStats = statsDict.get('userStats', {})
                filteredUserStats = {}
                for k,v in userStats.items():
                    if k in userStatsPvaTypes:
                        filteredUserStats[k] = v
                statusObject['userStats'] = filteredUserStats
            statusObject['collectorStats'] = statsDict.get('collectorStats', {})
            for producerId in self.producerIdList:
                producerStatsDict = statsDict.get(f'producerStats', {})
                producerStatsDict = producerStatsDict.get(f'producer-{producerId}', {})
                producerStatusObject = {'producerId' : producerId}
                producerStatusObject['monitorStats'] = producerStatsDict.get('monitorStats', {})
                producerStatusObject['queueStats'] = producerStatsDict.get('queueStats', {})
                statusObject[f'producerStats_{producerId}'] = producerStatusObject
            self.pvaServer.update(self.statusChannel, statusObject)
        return statsDict 

    def stopScreen(self):
        if self.screen:
            curses.endwin()
        self.screen = None

    def stopCollectors(self):
        self.logger.debug('Controller exiting')
        try: 
            self.dataCollector.stop()
            self.logger.info(f'Stopping collector {self.dataCollector.collectorId}')
        except Exception as ex:
            self.logger.warn(f'Could not stop collector {self.dataCollector.collectorId}')

        statsDict = self.dataCollector.getStats()
        self.stopScreen()
        return statsDict

def main():
    parser = argparse.ArgumentParser(description='PvaPy HPC Collector utility. It can be used for receiving data from a set of producer processes, and processing this data using a specified implementation of the data processor interface.')
    parser.add_argument('-v', '--version', action='version', version='%(prog)s {version}'.format(version=__version__))
    parser.add_argument('-id', '--collector-id', dest='collector_id', type=int, default=1, help='Collector id (default: 1). This may be used for naming various PVA channels, so care must be taken when multiple collector processes are running independently of each other.')
    parser.add_argument('-pir', '--producer-id-list', dest='producer_id_list', default='1,2', help='Comma-separated list of producer IDs (default: 1,2). This option can also be specified as "range(<firstId>,<lastId+1>[,<idStep>)."')
    parser.add_argument('-ic', '--input-channel', dest='input_channel', required=True, help='Input PV channel name. It shuld contain the "*" character which will be replaced with <producerId>.')
    parser.add_argument('-oc', '--output-channel', dest='output_channel', default=None, help='Output PVA channel name (default: None). If specified, this channel can be used for publishing processing results. The value of "_" indicates that the output channel name will be set to "pvapy:collector:<collectorId>:output", while the "*" character will be replaced with <collectorId>. Note that this parameter is ignored if processor arguments dictionary contains "outputChannel" key.')
    parser.add_argument('-sc', '--status-channel', dest='status_channel', default=None, help='Status PVA channel name (default: None). If specified, this channel will provide collector status. The value of "_" indicates that the status channel name will be set to "pvapy:collector:<collectorId>:status", while the "*" character will be replaced with <collectorId>.')
    parser.add_argument('-cc', '--control-channel', dest='control_channel', default=None, help='Control channel name (default: None). If specified, this channel can be used to control collector configuration and processing. The value of "_" indicates that the control channel name will be set to "pvapy:collector:<collectorId>:control", while the "*" character will be replaced with <collectorId>. The control channel object has two strings: command and args. The only allowed values for the command string are: "configure", "reset_stats", "get_stats" and "stop". The configure command is used to allow for runtime configuration changes; in this case the keyword arguments string should be in json format to allow data collector to convert it into python dictionary that contains new configuration. For example, sending configuration dictionary via pvput command might look like this: pvput pvapy:collector:1:control \'{"command" : "configure", "args" : "{\\"x\\":100}"}\'. Note that system parameters that can be modified at runtime are the following: "collectorCacheSize", "monitorQueueSize" (only if client monitor queues have been configured at the start), "skipInitialUpdates" (affects processing behavior after resetting stats), and "objectIdOffset". The reset_stats command will cause collector to reset its statistics data, the get_stats will force statistics data update, and the stop command will result in collector process exiting; for all of these commands args string is not needed.')
    parser.add_argument('-sqs', '--server-queue-size', type=int, dest='server_queue_size', default=0, help='Server queue size (default: 0); this setting will increase memory usage on the server side, but may help prevent missed PV updates.')
    parser.add_argument('-mqs', '--monitor-queue-size', type=int, dest='monitor_queue_size', default=-1, help='PVA channel monitor (client) queue size (default: -1); if < 0, PV updates will be processed immediately without copying them into PvObjectQueue; if >= 0, PvObjectQueue will be used for receving PV updates (value of zero indicates infinite queue size).')
    parser.add_argument('-ccs', '--collector-cache-size', type=int, dest='collector_cache_size', default=-1, help='Collector cache size (default: -1). Collector puts all received PV updates into its cache; once the cache is full, PV updates are sorted by the objectIdField value, removed from the cache and further processed. If specified cache size is negative, or smaller than the minimum allowed value (nProducers*10), this option will be ignored.')
    parser.add_argument('-pf', '--processor-file', dest='processor_file', default=None, help='Full path to the python file containing user processor class. If this option is not used, the processor class should be specified using "<modulePath>.<className>" notation.')
    parser.add_argument('-pc', '--processor-class', dest='processor_class', default=None, help='Name of the class located in the user processor file that will be processing PV updates. Alternatively, if processor file is not given, the processor class should be specified using the "<modulePath>.<className>" notation. The class should be initialized with a dictionary and must implement the "process(self, pv)" method.')
    parser.add_argument('-pa', '--processor-args', dest='processor_args', default=None, help='JSON-formatted string that can be converted into dictionary and used for initializing user processor object.')
    parser.add_argument('-of', '--oid-field', dest='oid_field', default='uniqueId', help='PV update id field used for calculating data processor statistics (default: uniqueId). This parameter is ignored if processor arguments dictionary contains "objectIdField" key.')
    parser.add_argument('-oo', '--oid-offset', type=int, dest='oid_offset', default=1, help='This parameter determines by how much object id should change between the two PV updates, and is used for determining the number of missed PV updates (default: 1). This parameter is ignored if processor arguments dictionary contains "objectIdOffset" key.')
    parser.add_argument('-fr', '--field-request', dest='field_request', default='', help='PV field request string (default: None). This parameter can be used to request only a subset of the data available in the input channel. The system will automatically append object id field to the specified request string. Note that this parameter is ignored when data distributor is used.')
    parser.add_argument('-siu', '--skip-initial-updates', type=int, dest='skip_initial_updates', default=1, help='Number of initial PV updates that should not be processed (default: 1). This parameter is ignored if processor arguments dictionary contains "skipInitialUpdates" key.')
    parser.add_argument('-rt', '--runtime', type=float, dest='runtime', default=0, help='Server runtime in seconds; values <=0 indicate infinite runtime (default: infinite).')
    parser.add_argument('-rp', '--report-period', type=float, dest='report_period', default=0, help='Statistics report period for the collector in seconds; values <=0 indicate no reporting (default: 0).')
    parser.add_argument('-rs', '--report-stats', dest='report_stats', default='all', help='Comma-separated list of statistics subsets that should be reported (default: all); possible values: monitor, queue, processor, user, all.')
    parser.add_argument('-ll', '--log-level', dest='log_level', help='Log level; possible values: debug, info, warning, error, critical. If not provided, there will be no log output.')
    parser.add_argument('-lf', '--log-file', dest='log_file', help='Log file.')
    parser.add_argument('-dc', '--disable-curses', dest='disable_curses', default=False, action='store_true', help='Disable curses library screen handling. This is enabled by default, except when logging into standard output is turned on.')

    args, unparsed = parser.parse_known_args()
    if len(unparsed) > 0:
        print('Unrecognized argument(s): {}'.format(' '.join(unparsed)))
        exit(1)

    logger = LoggingManager.getLogger('collectorMain')
    controller = CollectorController(args)
    controller.startCollectors()
    startTime = time.time()
    lastReportTime = startTime
    lastStatusUpdateTime = startTime
    waitTime = WAIT_TIME
    while True:
        try:
            now = time.time()
            wakeTime = now+waitTime
            if controller.isDone:
                break
            if args.runtime > 0:
                runtime = now - startTime
                if runtime > args.runtime:
                    break
            if args.report_period > 0 and now-lastReportTime > args.report_period:
                lastReportTime = now
                lastStatusUpdateTime = now
                controller.reportCollectorStats()

            if args.status_channel and now-lastStatusUpdateTime > MIN_STATUS_UPDATE_PERIOD:
                lastStatusUpdateTime = now
                controller.getCollectorStats()

            # Check if we need to sleep
            delay = wakeTime-time.time()
            if delay > 0:
                time.sleep(delay)

        except KeyboardInterrupt as ex:
            break

    print()
    statsDict = controller.stopCollectors()
    controller.reportCollectorStats(statsDict)
    # Allow clients monitoring various channels to get last update
    time.sleep(WAIT_TIME)

if __name__ == '__main__':
    main()
