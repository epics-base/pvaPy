#!/usr/bin/env python

import curses
import argparse
import time
import json
import queue
import pvaccess as pva
import multiprocessing as mp
from ..utility.loggingManager import LoggingManager
from ..utility.objectUtility import ObjectUtility
from ..hpc.dataConsumer import DataConsumer

__version__ = pva.__version__

WAIT_TIME = 1.0
GET_STATS_COMMAND = 'get_stats'
STOP_COMMAND = 'stop'

class ConsumerController:

    def __init__(self, args):
        self.screen = None
        if args.log_level:
            LoggingManager.setLogLevel(args.log_level)
            if args.log_file:
                LoggingManager.addFileHandler(args.log_file)
                self.screen = curses.initscr()
            else:
                LoggingManager.addStreamHandler()
        else:
            self.screen = curses.initscr()

        self.logger = LoggingManager.getLogger(self.__class__.__name__)
        self.args = args

    def createProcessor(self, processorId, args):
        dataProcessor = None
        if args.processor_file and args.processor_class:
            # Create config dict
            processorConfig = {}
            if args.processor_args:
                processorConfig = json.loads(args.processor_args)
            if not 'processorId' in processorConfig:
                processorConfig['processorId'] = processorId
            if not 'processFirstUpdate' in processorConfig:
                processorConfig['processFirstUpdate'] = args.process_first_update
            if not 'objectIdField' in processorConfig:
                processorConfig['objectIdField'] = args.processor_oid_field
            if not 'objectIdOffset' in processorConfig:
                processorConfig['objectIdOffset'] = args.processor_oid_offset

            self.logger.debug(f'Using processor configuration: {processorConfig}')
            dataProcessor = ObjectUtility.createObjectInstanceFromFile(args.processor_file, 'dataProcessor', args.processor_class, processorConfig)
            self.logger.debug(f'Created data processor {processorId}')
        return dataProcessor
            
    def createConsumer(self, consumerId, processorId, args):
        dataProcessor = self.createProcessor(processorId, args)
            
        pvObjectQueue = None
        self.usingPvObjectQueue = False
        if args.consumer_queue_size >= 0:
            pvObjectQueue = pva.PvObjectQueue(args.consumer_queue_size)
            self.usingPvObjectQueue = True

        self.dataConsumer = DataConsumer(consumerId, args.channel_name, providerType=args.channel_provider_type, serverQueueSize=args.server_queue_size, distributorGroupId=args.distributor_group, distributorSetId=args.distributor_set, distributorTriggerFieldName=args.distributor_trigger, distributorNumUpdates=args.distributor_n_updates, distributorUpdateMode=None, pvObjectQueue=pvObjectQueue, dataProcessor=dataProcessor)
        return self.dataConsumer

    def startConsumers(self):
        self.createConsumer(consumerId=1, processorId=1, args=self.args)
        self.dataConsumer.start()
        self.logger.info(f'Started consumer {self.dataConsumer.getConsumerId()}')

    def reportConsumerStats(self, statsDict=None):
        if not statsDict:
            statsDict = self.getConsumerStats()
        consumerId = self.dataConsumer.getConsumerId()
        report = self.formatConsumerStats(consumerId, statsDict)
        if self.screen:
            self.screen.erase()
            self.screen.addstr(f'{report}\n')
            self.screen.refresh()
        else:
            print(report)

    def formatConsumerStats(self, consumerId, statsDict):
        now = time.time()
        report = 'consumer-{} @ {:.3f}s :\n'.format(consumerId, now)
        for k,v in statsDict.items():
            if not v:
                continue
            report += '  {:15s}:'.format(k)
            if type(v) == dict:
                for (k2,v2) in v.items():
                    report += ' {}'.format(self._formatDictEntry(k2,v2))
            else:
                report += '  {}'.format(self._formatDictEntry(k,v))
            report += '\n'
        return report

    def _formatDictEntry(self, k, v):
        if k.endswith('ime'):
            # anything ending with time or Time
            return '{}={:.3f}s'.format(k,v)
        elif k.endswith('ate'):
            # anything ending with rate or Rate
            return '{}={:.4f}Hz'.format(k,v)
        else:
            return '{}={}'.format(k,v)


    def getConsumerStats(self):
        return self.dataConsumer.getStats()

    def processPvUpdate(self, updateWaitTime):
        if self.usingPvObjectQueue:
            # This should be only done for a single consumer using a queue
            return self.dataConsumer.processFromQueue(updateWaitTime)
        return False

    def stopConsumers(self):
        self.logger.debug('Controller exiting')
        self.dataConsumer.stop()
        statsDict = self.dataConsumer.getStats()
        self.logger.info(f'Stopped consumer {self.dataConsumer.getConsumerId()}')
        if self.screen:
            curses.endwin()
        self.screen = None
        return statsDict

class MultiprocessConsumerController(ConsumerController):

    def __init__(self, args):
        ConsumerController.__init__(self, args)
        self.mpProcessMap = {}
        self.requestQueueMap = {}
        self.responseQueueMap = {}

    def startConsumers(self):
        for i in range(1, self.args.n_processors+1):
            consumerId = i
            processorId = i
            requestQueue = mp.Queue()
            self.requestQueueMap[i] = requestQueue
            responseQueue = mp.Queue()
            self.responseQueueMap[i] = responseQueue
            mpProcess = mp.Process(target=mpController, args=(consumerId, processorId, requestQueue, responseQueue, self.args,))
            self.mpProcessMap[i] = mpProcess
            self.logger.debug(f'Starting consumer process {i}')
            mpProcess.start()

    def reportConsumerStats(self, combinedStatsDict=None):
        if not combinedStatsDict:
            combinedStatsDict = self.getConsumerStats()
        report = ''
        for consumerId,statsDict in combinedStatsDict.items():
            report += self.formatConsumerStats(consumerId, statsDict)
        if self.screen:
            self.screen.erase()
            self.screen.addstr('{}\n'.format(report))
            self.screen.refresh()
        else:
            print(report)

    def getConsumerStats(self):
        for i in range(1, self.args.n_processors+1):
            requestQueue = self.requestQueueMap[i]
            requestQueue.put(GET_STATS_COMMAND)
        statsDict = {}
        for i in range(1, self.args.n_processors+1):
            statsDict[i] = {}
            try:
                responseQueue = self.responseQueueMap[i]
                statsDict[i] = responseQueue.get(WAIT_TIME)
            except queue.Empty:
                self.logger.warn(f'No stats received from consumer process {i}')
        return statsDict

    def processPvUpdate(self, updateWaitTime):
        return False

    def stopConsumers(self):
        for i in range(1, self.args.n_processors+1):
            requestQueue = self.requestQueueMap[i]
            requestQueue.put(STOP_COMMAND)
        statsDict = {}
        for i in range(1, self.args.n_processors+1):
            statsDict[i] = {}
            try:
                responseQueue = self.responseQueueMap[i]
                statsDict[i] = responseQueue.get(WAIT_TIME)
            except queue.Empty:
                self.logger.warn(f'No stats received from consumer process {i}')
        for i in range(1, self.args.n_processors+1):
            mpProcess = self.mpProcessMap[i]
            mpProcess.join()
            self.logger.info(f'Stopped consumer process {i}')
        if self.screen:
            curses.endwin()
            self.screen = None
        self.logger.debug('Controller exiting')
        return statsDict

def mpController(consumerId, processId, requestQueue, responseQueue, args):
    controller = ConsumerController(args)
    logger = LoggingManager.getLogger(f'mpController-{consumerId}')
    dataConsumer = controller.createConsumer(consumerId, processId, args)
    dataConsumer.start()
    waitTime = WAIT_TIME
    while True:
        try:
            now = time.time()
            wakeTime = now+waitTime
            try:
                request = requestQueue.get()
                logger.debug(f'Received request: {request}')
                if request == STOP_COMMAND:
                    break
                elif request == GET_STATS_COMMAND:
                    statsDict = controller.getConsumerStats()
                    responseQueue.put(statsDict)
            except queue.Empty:
                pass

            hasProcessedObject = controller.processPvUpdate(waitTime)
            if not hasProcessedObject:
                # Check if we need to sleep
                delay = wakeTime-time.time()
                if delay > 0:
                    time.sleep(delay)
        except Exception as ex:
            break

    dataConsumer.stop()
    statsDict = controller.getConsumerStats()
    responseQueue.put(statsDict)
    time.sleep(WAIT_TIME)

def main():
    parser = argparse.ArgumentParser(description='PvaPy HPC Consumer utility. It can be used for receiving and processing data using specified implementation of the data processor interface.')
    parser.add_argument('-v', '--version', action='version', version='%(prog)s {version}'.format(version=__version__))
    parser.add_argument('-cn', '--channel-name', dest='channel_name', required=True, help='PV channel name.')
    parser.add_argument('-cpt', '--channel-provider-type', dest='channel_provider_type', default='pva', help='PV channel provider type, it must be either "pva" or "ca" (default: pva).')
    parser.add_argument('-sqs', '--server-queue-size', type=int, dest='server_queue_size', default=0, help='Server queue size (default: 0); this setting will increase memory usage on the server side, but may help prevent missed PV updates.')
    parser.add_argument('-cqs', '--consumer-queue-size', type=int, dest='consumer_queue_size', default=-1, help='Consumer queue size (default: -1); if >= 0, PvObjectQueue will be used for receving PV updates (value of zero indicates infinite queue size).')
    parser.add_argument('-pf', '--processor-file', dest='processor_file', default=None, help='Full path to the python file containing processor class.')
    parser.add_argument('-pc', '--processor-class', dest='processor_class', default=None, help='Name of the class located in the processor file that will be processing PV updates; it should be initialized with a dictionary and must implement the "process(self, pv)" method.')
    parser.add_argument('-pa', '--processor-args', dest='processor_args', default=None, help='JSON-formatted string that can be converted into dictionary and used for initializing processor object.')
    parser.add_argument('-poo', '--processor-oid-offset', type=int, dest='processor_oid_offset', default=1, help='This parameter determines by how much object id should change between the two PV updates, and is used for determining the number of missed PV updates (default: 1). This parameter is ignored if processor args dictionary contains "objectIdOffset" key, and should be modified only if data distributor plugin will be distributing data between multiple clients, and should be set to "(<nConsumers>-1)*<nUpdates>" for a single client set, or to "(<nSets>-1)*<nUpdates>" for multiple client sets.')
    parser.add_argument('-pof', '--processor-oid-field', dest='processor_oid_field', default='uniqueId', help='PV update id field used for calculating data processor statistics (default: uniqueId). This parameter is ignored if processor args dictionary contains "objectIdField" key.')
    parser.add_argument('-pfu', '--process-first-update', dest='process_first_update', default=False, action='store_true', help='Process first PV update (default: False). This parameter is ignored if processor args dictionary contains "processFirstUpdate" key.')
    parser.add_argument('-np', '--n-processors', type=int, dest='n_processors', default=1, help='Number of data processors to instantiate (default: 1). If > 1, multiprocessing module will be used for receiving and processing data in separate processes.')
    parser.add_argument('-dg', '--distributor-group', dest='distributor_group', default=None, help='Distributor client group that application belongs to (default: None). This parameter should be used only if data distributor plugin will be distributing data between multiple clients. Note that different distributor groups are completely independent of each other.')
    parser.add_argument('-ds', '--distributor-set', dest='distributor_set', default=None, help='Distributor client set that application belongs to within its group (default: None). This parameter should be used only if data distributor plugin will be distributing data between multiple clients. Note that all clients belonging to the same set receive the same PV updates. If set id is not specified (i.e., if a group does not have multiple sets of clients), a PV update will be distributed to only one client.')
    parser.add_argument('-dt', '--distributor-trigger', dest='distributor_trigger', default=None, help='PV structure field that data distributor uses to distinguish different channel updates (default: None). This parameter should be used only if data distributor plugin will be distributing data between multiple clients. In case of, for example, area detector applications, the "uniqueId" field would be a good choice for distinguishing between the different frames.')
    parser.add_argument('-dnu', '--distributor-n-updates', dest='distributor_n_updates', default=None, help='Number of sequential PV channel updates that a client (or a set of clients) will receive (default: None). This parameter should be used only if data distributor plugin will be distributing data between multiple clients.')
    parser.add_argument('-rt', '--runtime', type=float, dest='runtime', default=0, help='Server runtime in seconds; values <=0 indicate infinite runtime (default: infinite).')
    parser.add_argument('-rp', '--report-period', type=float, dest='report_period', default=0, help='Statistics report period for all consumers in seconds; values <=0 indicate no reporting (default: 0).')
    parser.add_argument('-ll', '--log-level', dest='log_level', help='Log level; possible values: DEBUG, INFO, WARN, ERROR, CRITICAL. If not provided, there will be no log output.')
    parser.add_argument('-lf', '--log-file', dest='log_file', help='Log file.')

    args, unparsed = parser.parse_known_args()
    if len(unparsed) > 0:
        print('Unrecognized argument(s): {}'.format(' '.join(unparsed)))
        exit(1)

    if args.n_processors == 1:
        controller = ConsumerController(args)
    else:
        controller = MultiprocessConsumerController(args)
    controller.startConsumers()
    startTime = time.time()
    lastReportTime = startTime
    waitTime = WAIT_TIME
    while True:
        try:
            now = time.time()
            wakeTime = now+waitTime
            if args.runtime > 0:
                runtime = now - startTime
                if runtime > args.runtime:
                    break
            if args.report_period > 0 and now-lastReportTime > args.report_period:
                lastReportTime = now
                controller.reportConsumerStats()

            hasProcessedObject = controller.processPvUpdate(waitTime)
            if not hasProcessedObject:
                # Check if we need to sleep
                delay = wakeTime-time.time()
                if delay > 0:
                    time.sleep(delay)
        except KeyboardInterrupt as ex:
            break

    statsDict = controller.stopConsumers()
    controller.reportConsumerStats(statsDict)

if __name__ == '__main__':
    main()
