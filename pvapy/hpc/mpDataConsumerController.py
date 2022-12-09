#!/usr/bin/env python

import threading
import time
import queue
import pvaccess as pva
import multiprocessing as mp
from ..utility.loggingManager import LoggingManager
from .dataConsumer import DataConsumer
from .systemController import SystemController
from .dataConsumerController import DataConsumerController

class MpDataConsumerController(SystemController):

    ''' 
    Controller class for a multiple data consumers.
  
    **MpDataConsumerController(inputChannel, outputChannel=None, statusChannel=None, controlChannel=None, idFormatSpec=None, processorFile=None, processorClass=None, processorArgs=None, objectIdField='uniqueId', objectIdOffset=0, fieldRequest='', skipInitialUpdates=1, reportStatsList='all', logLevel=None, logFile=None, disableCurses=False, consumerId=1, nConsumers=1, inputProviderType='pva', serverQueueSize=0, monitorQueueSize=-1, accumulateObjects=-1, accumulationTimeout=1, distributorPluginName='pydistributor', distributorGroup=None, distributorSet=None, distributorTrigger=None, distributorUpdates=None, nDistributorSets=1, metadataChannels=None)**

    :Parameter: *inputChannel* (str) - Input PV channel name. The "*" character will be replaced with <consumerId> formatted using <idFormatSpec> specification.
    :Parameter: *outputChannel* (str) - Output PVA channel name (default: None). If specified, this channel can be used for publishing processing results. The value of "_" indicates that the output channel name will be set to "pvapy:consumer:<consumerId>:output", while the "*" character will be replaced with <consumerId> formatted using <idFormatSpec> specification.
    :Parameter: *statusChannel* (str) - Status PVA channel name (default: None). If specified, this channel will provide consumer status. The value of "_" indicates that the status channel name will be set to "pvapy:consumer:<consumerId>:status", while the "*" character will be replaced with <consumerId> formatted using <idFormatSpec> specification.
    :Parameter: *controlChannel* (str) - Control channel name (default: None). If specified, this channel can be used to control consumer configuration and processing. The value of "_" indicates that the control channel name will be set to "pvapy:consumer:<consumerId>:control", while the "*" character will be replaced with <consumerId> formatted using <idFormatSpec> specification. The control channel object has two strings: command and args. The only allowed values for the command string are: "configure", "reset_stats", "get_stats" and "stop". The configure command is used to allow for runtime configuration changes; in this case the keyword arguments string should be in json format to allow data consumer to convert it into python dictionary that contains new configuration. For example, sending configuration dictionary via pvput command might look like this: pvput input_channel:consumer:2:control \'{"command" : "configure", "args" : "{\\"x\\":100}"}\'. Note that system parameters that can be modified at runtime are the following: "monitorQueueSize" (only if client monitor queue has been configured at the start), "skipInitialUpdates" (affects processing behavior after resetting stats), and "objectIdOffset" (may be used to adjust offset if consumers have been added or removed from processing). The reset_stats command will cause consumer to reset its statistics data, the get_stats will force statistics data update, and the stop command will result in consumer process exiting; for all of these commands args string is not needed.
    :Parameter: *idFormatSpec* (str) - Specification to be used for consumer id when forming input, output, status and control channel names (default: None).
    :Parameter: *processorFile* (str) - Full path to the python file containing user processor class. If this option is not used, the processor class should be specified using "<modulePath>.<className>" notation.
    :Parameter: *processorClass* (str) - Name of the class located in the user processor file that will be processing PV updates. Alternatively, if processor file is not given, the processor class should be specified using the "<modulePath>.<className>" notation. The class should be initialized with a dictionary and must implement the "process(self, pv)" method.
    :Parameter: *processorArgs* (str) - JSON-formatted string that can be converted into dictionary and used for initializing user processor object.
    :Parameter: *objectIdField* (str) - PV update id field used for calculating data processor statistics (default: uniqueId).
    :Parameter: *objectIdOffset* (int) - This parameter determines by how much object id should change between the two PV updates, and is used for determining the number of missed PV updates (default: 0). This parameter should be modified only if data distributor plugin will be distributing data between multiple clients, in which case it should be set to "(<nConsumers>-1)*<nUpdates>+1" for a single client set, or to "(<nSets>-1)*<nUpdates>+1" for multiple client sets. Values <= 0 will be replaced with the appropriate value depending on the number of client sets specified. Note that this relies on using the same value for the --n-distributor-sets when multiple instances of this command are running separately. 
    :Parameter: *fieldRequest* (str) - PV field request string (default: None). This parameter can be used to request only a subset of the data available in the input channel. The system will automatically append object id field to the specified request string. Note that this parameter is ignored when data distributor is used.
    :Parameter: *skipInitialUpdates* (int) - Number of initial PV updates that should not be processed (default: 1).
    :Parameter: *reportStatsList* (str) - Comma-separated list of statistics subsets that should be reported (default: all); possible values: monitor, queue, processor, user, all.
    :Parameter: *logLevel* (str) - Log level; possible values: debug, info, warning, error, critical. If not provided, there will be no log output.
    :Parameter: *logFile* (str) - Log file.
    :Parameter: *disableCurses* (bool) - Disable curses library screen handling. This is enabled by default, except when logging into standard output is turned on.
    :Parameter: *consumerId* (int) - First consumer id; for each subsequent consumer id will be increased by 1. Note that consumer id is used for naming various PVA channels, so care must be taken when multiple consumer processes are running independently of each other.
    :Parameter: *nConsumers* (int) - Number of consumers to instantiate (default: 1). Multiprocessing module will be used for receiving and processing data in separate processes.
    :Parameter: *consumerIdList* (str) - Comma-separated list of consumer IDs (default: None). This option can also be specified as "range(<firstId>,<lastId+1>[,<idStep>). If this option is used, values given for <consumerId> and <nConsumers> options will be ignored.
    :Parameter: *inputProviderType* (str) - Input PV channel provider type, it must be either "pva" or "ca" (default: pva).
    :Parameter: *serverQueueSize* (int) - Server queue size (default: 0); this setting will increase memory usage on the server side, but may help prevent missed PV updates.
    :Parameter: *monitorQueueSize* (int) - PVA channel monitor (client) queue size (default: -1); if < 0, PV updates will be processed immediately without copying them into PvObjectQueue; if >= 0, PvObjectQueue will be used for receving PV updates (value of zero indicates infinite queue size).
    :Parameter: *accumulateObjects* (int) - Number of objects to accumulate in the PVA channel monitor (client) queue before they can be processed (default: -1); if <= 0 the processing happens regarding of the current monitor queue length. This option is ignored unless monitor (client) queue size is set (i.e., >= 0). Note that after accumulation timeout, all objects in the queue will be processed.
    :Parameter: *accumulateTimeout* (float) - Time period since last received item after which objects in the PVA channel monitor (client) queue will be processed regardless of the current queue length (default: 1 second). This option is ignored unless monitor (client) queue size is set (i.e, >= 0) and if number of accumulated objects is not set (i.e., <= 0).
    :Parameter: *distributorPluginName* (str) - Distributor plugin name (default: pydistributor).
    :Parameter: *distributorGroup* (str) - Distributor client group that application belongs to (default: None). This parameter should be used only if data distributor plugin will be distributing data between multiple clients. Note that different distributor groups are completely independent of each other.
    :Parameter: *distributorSet* (str) - Distributor client set that application belongs to within its group (default: None). This parameter should be used only if data distributor plugin will be distributing data between multiple clients. Note that all clients belonging to the same set receive the same PV updates. If set id is not specified (i.e., if a group does not have multiple sets of clients), a PV update will be distributed to only one client.
    :Parameter: *distributorTrigger* (str) - PV structure field that data distributor uses to distinguish different channel updates (default: None). This parameter should be used only if data distributor plugin will be distributing data between multiple clients. In case of, for example, area detector applications, the "uniqueId" field would be a good choice for distinguishing between the different frames.
    :Parameter: *distributorUpdates* (int) - Number of sequential PV channel updates that a client (or a set of clients) will receive (default: None). This parameter should be used only if data distributor plugin will be distributing data between multiple clients.
    :Parameter: *nDistributorSets* (int) - Number of distributor client sets (default: 1). This setting is used to determine appropriate value for the processor object id offset in case where multiple instances of this command are running separately for different client sets. If distributor client set is not specified, this setting is ignored.
    :Parameter: *metadataChannels* (str) - Comma-separated list of metadata channels specified in the form "protocol:\\<channelName>", where protocol can be either "ca" or "pva". If channel name is specified without a protocol, "ca" is assumed.
    '''
    def __init__(self, inputChannel, outputChannel=None, statusChannel=None, controlChannel=None, idFormatSpec=None, processorFile=None, processorClass=None, processorArgs=None, objectIdField='uniqueId', objectIdOffset=0, fieldRequest='', skipInitialUpdates=1, reportStatsList='all', logLevel=None, logFile=None, disableCurses=False, consumerId=1, nConsumers=1, consumerIdList=None, inputProviderType='pva', serverQueueSize=0, monitorQueueSize=-1, accumulateObjects=-1, accumulationTimeout=1, distributorPluginName='pydistributor', distributorGroup=None, distributorSet=None, distributorTrigger=None, distributorUpdates=None, nDistributorSets=1, metadataChannels=None):

        SystemController.__init__(self, inputChannel, outputChannel=outputChannel, statusChannel=statusChannel, controlChannel=controlChannel, idFormatSpec=idFormatSpec, processorFile=processorFile, processorClass=processorClass, processorArgs=processorArgs, objectIdField=objectIdField, objectIdOffset=objectIdOffset, fieldRequest=fieldRequest, skipInitialUpdates=skipInitialUpdates, reportStatsList=reportStatsList, logLevel=logLevel, logFile=logFile, disableCurses=disableCurses)
        self.consumerId = consumerId # used as a start of the consumer id range
        self.nConsumers = nConsumers
        self.consumerIdListSpec = consumerIdList
        if consumerIdList:
            self.consumerIdList = self.generateIdList(consumerIdList)
            self.nConsumers = len(self.consumerIdList)
            self.consumerId = self.consumerIdList[0]
        else:
            self.consumerIdList = list(range(self.consumerId, self.consumerId+self.nConsumers))
        self.logger.debug(f'Consumer id list: {self.consumerIdList}')

        self.inputProviderType = inputProviderType
        self.serverQueueSize = serverQueueSize
        self.monitorQueueSize = monitorQueueSize
        self.accumulateObjects = accumulateObjects
        self.accumulationTimeout = accumulationTimeout
        self.distributorPluginName = distributorPluginName
        self.distributorGroup = distributorGroup
        self.distributorSet = distributorSet
        self.distributorTrigger = distributorTrigger
        self.distributorUpdates = distributorUpdates
        self.nDistributorSets = nDistributorSets
        self.metadataChannels = metadataChannels

        self.mpProcessMap = {}
        self.requestQueueMap = {}
        self.responseQueueMap = {}
        self.lastStatsObjectIdMap = {}

    def start(self):
        # Replace interrupt handler for worker processes
        # so we can exit cleanly
        import signal
        originalSigintHandler = signal.signal(signal.SIGINT, signal.SIG_IGN)
        for consumerId in self.consumerIdList:
            requestQueue = mp.Queue()
            self.requestQueueMap[consumerId] = requestQueue
            responseQueue = mp.Queue()
            self.responseQueueMap[consumerId] = responseQueue
            mpProcess = mp.Process(target=mpdcController, args=(requestQueue, responseQueue, self.inputChannel, self.outputChannel, self.statusChannel, self.controlChannel, self.idFormatSpec, self.processorFile, self.processorClass, self.processorArgs, self.objectIdField, self.objectIdOffset, self.fieldRequest, self.skipInitialUpdates, self.reportStatsList, self.logLevel, self.logFile, self.disableCurses, consumerId, self.nConsumers, self.inputProviderType, self.serverQueueSize, self.monitorQueueSize, self.accumulateObjects, self.accumulationTimeout, self.distributorPluginName, self.distributorGroup, self.distributorSet, self.distributorTrigger, self.distributorUpdates, self.nDistributorSets, self.metadataChannels,))
            self.mpProcessMap[consumerId] = mpProcess
            self.logger.debug(f'Starting consumer {consumerId}')
            mpProcess.start()
        signal.signal(signal.SIGINT, originalSigintHandler)

    def reportStats(self, statsDict=None):
        if not statsDict:
            statsDict = self.getStats()
        report = ''
        for consumerId,statsDict2 in statsDict.items():
            statsDict2['consumerId'] = consumerId
            report += self.prettyPrinter.pformat(statsDict2)
            report += '\n'
        if self.screen:
            try:
                self.screen.erase()
                self.screen.addstr(report)
                self.screen.refresh()
                return
            except Exception as ex:
                # Turn screen off on errors
                self.stopScreen()
        # Remove extra newline character
        print(report[0:-1])

    def getStats(self):
        for consumerId in self.consumerIdList:
            requestQueue = self.requestQueueMap[consumerId]
            try:
                requestQueue.put(self.GET_STATS_COMMAND, block=True, timeout=self.WAIT_TIME)
            except Exception as ex:
                self.stopScreen()
                self.logger.error(f'Cannot request stats from consumer {consumerId}: {ex}')
        statsDict = {}
        for consumerId in self.consumerIdList:
            statsDict[consumerId] = {}
            lastStatsObjectId = self.lastStatsObjectIdMap.get(consumerId, 0)
            try:
                while True:
                    responseQueue = self.responseQueueMap[consumerId]
                    statsDict[consumerId] = responseQueue.get(block=True, timeout=self.WAIT_TIME)
                    statsObjectId = statsDict[consumerId].get('objectId', 0)
                    if statsObjectId != lastStatsObjectId:
                        self.lastStatsObjectIdMap[consumerId] = statsObjectId 
                        break
                    else:
                        self.logger.warning(f'Discarding stale stats received from consumer {consumerId}')
            except queue.Empty:
                self.stopScreen()
                self.logger.error(f'No stats received from consumer {consumerId}')
        return statsDict

    def stop(self):
        for consumerId in self.consumerIdList:
            requestQueue = self.requestQueueMap[consumerId]
            try:
                requestQueue.put(self.STOP_COMMAND, block=True, timeout=self.WAIT_TIME)
            except Exception as ex:
                self.stopScreen()
                self.logger.error(f'Cannot stop consumer {consumerId}: {ex}')
        statsDict = {}
        for consumerId in self.consumerIdList:
            statsDict[consumerId] = {}
            try:
                responseQueue = self.responseQueueMap[consumerId]
                statsDict[consumerId] = responseQueue.get(block=True, timeout=self.WAIT_TIME)
                self.logger.debug(f'Received final stats for consumer {consumerId}')
            except queue.Empty:
                self.stopScreen()
                self.logger.error(f'No stats received from consumer {consumerId}')
        for consumerId in self.consumerIdList:
            mpProcess = self.mpProcessMap[consumerId]
            mpProcess.join(self.WAIT_TIME)
            self.logger.info(f'Stopped process for consumer {consumerId}')
        if self.screen:
            self.curses.endwin()
            self.screen = None
        self.logger.debug('Controller exiting')
        return statsDict

class MpdcControllerRequestProcessingThread(threading.Thread):

    def __init__(self, controller, consumerId, requestQueue, responseQueue):
        threading.Thread.__init__(self)
        self.controller = controller
        self.consumerId = consumerId
        self.requestQueue = requestQueue
        self.responseQueue = responseQueue
        self.logger = LoggingManager.getLogger(f'rpThread-{self.consumerId}')

    def run(self):
        self.logger.debug(f'Request processing thread for consumer {self.consumerId} starting')
        while True:
            try:
                if self.controller.isStopped:
                    self.logger.debug(f'Consumer {self.consumerId} is done, request processing thread is exiting')
                    break

                # Check for new request
                try:
                    request = self.requestQueue.get(block=True, timeout=self.controller.WAIT_TIME)
                    self.logger.debug(f'Received request: {request}')
                    if request == self.controller.STOP_COMMAND:
                        self.controller.shouldBeStopped = True
                        break
                    elif request == self.controller.GET_STATS_COMMAND:
                        statsDict = self.controller.getStats()
                        try:
                            self.responseQueue.put(statsDict, block=False)
                        except Exception as ex:
                            self.logger.error(f'Consumer {consumerId} cannot report stats: {ex}')
                except queue.Empty:
                    pass

            except Exception as ex:
                self.logger.error(f'Request processing error: {ex}')

        self.logger.debug(f'Request processing thread for consumer {self.consumerId} exited')

def mpdcControllerInit():
    # Try to avoid epicsThread stderr messages related to forking new process
    try:
        import sys
        stderr = sys.stderr
        stdout = sys.stdout
        sys.stderr = None
        sys.stdout = None
        pva.PvObjectQueue()
    except:
        pass
    sys.stderr = stderr
    sys.stdout = stdout
    
def mpdcController(requestQueue, responseQueue, inputChannel, outputChannel, statusChannel, controlChannel, idFormatSpec, processorFile, processorClass, processorArgs, objectIdField, objectIdOffset, fieldRequest, skipInitialUpdates, reportStatsList, logLevel, logFile, disableCurses, consumerId, nConsumers, inputProviderType, serverQueueSize, monitorQueueSize, accumulateObjects, accumulationTimeout, distributorPluginName, distributorGroup, distributorSet, distributorTrigger, distributorUpdates, nDistributorSets, metadataChannels):
    logger = LoggingManager.getLogger(f'mpdcController-{consumerId}')
    mpdcControllerInit()
    controller = DataConsumerController(
        inputChannel,
        outputChannel=outputChannel,
        statusChannel=statusChannel,
        controlChannel=controlChannel,
        idFormatSpec=idFormatSpec,
        processorFile=processorFile,
        processorClass=processorClass,
        processorArgs=processorArgs,
        objectIdField=objectIdField,
        objectIdOffset=objectIdOffset,
        fieldRequest=fieldRequest,
        skipInitialUpdates=skipInitialUpdates,
        reportStatsList=reportStatsList,
        logLevel=logLevel,
        logFile=logFile,
        disableCurses=disableCurses,
        consumerId=consumerId,
        nConsumers=nConsumers,
        consumerIdList=None,
        inputProviderType=inputProviderType,
        serverQueueSize=serverQueueSize,
        monitorQueueSize=monitorQueueSize,
        accumulateObjects=accumulateObjects,
        accumulationTimeout=accumulationTimeout,
        distributorPluginName=distributorPluginName,
        distributorGroup=distributorGroup,
        distributorSet=distributorSet,
        distributorTrigger=distributorTrigger,
        distributorUpdates=distributorUpdates,
        nDistributorSets=nDistributorSets,
        metadataChannels=metadataChannels
    )
    controller.start()

    # Process controller requests in a separate thread
    rpThread = MpdcControllerRequestProcessingThread(controller, consumerId, requestQueue, responseQueue)
    rpThread.start()

    waitTime = controller.WAIT_TIME
    while True:
        try:
            if controller.shouldBeStopped:
                break

            now = time.time()
            wakeTime = now+waitTime

            # Try to process object
            delay = 0
            hasProcessedObject = controller.processPvUpdate(waitTime)
            if not hasProcessedObject:
                # Determine if we can wait
                delay = wakeTime-time.time()
                if delay > 0:
                    time.sleep(delay)

        except Exception as ex:
            controller.stopScreen()
            logger.error(f'Processing error: {ex}')

    try: 
        logger.debug(f'Stopping controller for consumer {consumerId}')
        controller.stop()
    except Exception as ex:
        self.logger.warn(f'Could not stop controller for consumer {consumerId}')
    try:
        logger.debug(f'Requesting final stats for consumer {consumerId}')
        statsDict = controller.getStats()
        logger.debug(f'Reporting final stats for consumer {consumerId}')
        responseQueue.put(statsDict, block=True, timeout=controller.WAIT_TIME)
    except Exception as ex:
        logger.error(f'Consumer {consumerId} cannot report stats on exit: {ex}')
    time.sleep(controller.WAIT_TIME)

