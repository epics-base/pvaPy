#!/usr/bin/env python

'''
Data collector controller module.
'''

import json
import threading
import time
import pvaccess as pva
from ..utility.objectUtility import ObjectUtility
from .dataProcessingController import DataProcessingController
from .sourceChannel import SourceChannel
from .dataCollector import DataCollector
from .systemController import SystemController


class DataCollectorController(SystemController):

    CONTROLLER_TYPE = 'collector'

    ''' 
    Controller class for data collector.
  
    **DataCollectorController(inputChannel, outputChannel=None, statusChannel=None, controlChannel=None, idFormatSpec=None, processorFile=None, processorClass=None, processorArgs=None, objectIdField='uniqueId', objectIdOffset=0, fieldRequest='', skipInitialUpdates=1, reportStatsList='all', logLevel=None, logFile=None, disableCurses=False, collectorId=1, producerIdList='1,2', serverQueueSize=0, monitorQueueSize=-1, collectorCacheSize=-1, metadataChannels=None)**

    :Parameter: *inputChannel* (str) - Input PV channel name. The "*" character will be replaced with <producerId> formatted using <idFormatSpec> specification.
    :Parameter: *outputChannel* (str) - Output PVA channel name (default: None). If specified, this channel can be used for publishing processing results. The value of "_" indicates that the output channel name will be set to "pvapy:collector:<collectorId>:output", while the "*" character will be replaced with <collectorId> formatted using <idFormatSpec> specification.
    :Parameter: *statusChannel* (str) - Status PVA channel name (default: None). If specified, this channel will provide collector status. The value of "_" indicates that the status channel name will be set to "pvapy:collector:<collectorId>:status", while the "*" character will be replaced with <collectorId> formatted using <idFormatSpec> specification.
    :Parameter: *controlChannel* (str) - Control channel name (default: None). If specified, this channel can be used to control collector configuration and processing. The value of "_" indicates that the control channel name will be set to "pvapy:collector:<collectorId>:control", while the "*" character will be replaced with <collectorId> formatted using <idFormatSpec> specification. The control channel object has two strings: command and args. The only allowed values for the command string are: "configure", "reset_stats", "get_stats" and "stop". The configure command is used to allow for runtime configuration changes; in this case the keyword arguments string should be in json format to allow data collector to convert it into python dictionary that contains new configuration. For example, sending configuration dictionary via pvput command might look like this: pvput pvapy:collector:1:control \'{"command" : "configure", "args" : "{\\"x\\":100}"}\'. Note that system parameters that can be modified at runtime are the following: "collectorCacheSize", "monitorQueueSize" (only if client monitor queues have been configured at the start), "skipInitialUpdates" (affects processing behavior after resetting stats), and "objectIdOffset". The reset_stats command will cause collector to reset its statistics data, the get_stats will force statistics data update, and the stop command will result in collector process exiting; for all of these commands args string is not needed.
    :Parameter: *idFormatSpec* (str) - Specification to be used for producer or collector id when forming input, output, status and control channel names (default: None).
    :Parameter: *processorFile* (str) - Full path to the python file containing user processor class. If this option is not used, the processor class should be specified using "<modulePath>.<className>" notation.
    :Parameter: *processorClass* (str) - Name of the class located in the user processor file that will be processing PV updates. Alternatively, if processor file is not given, the processor class should be specified using the "<modulePath>.<className>" notation. The class should be initialized with a dictionary and must implement the "process(self, pv)" method.
    :Parameter: *processorArgs* (str) - JSON-formatted string that can be converted into dictionary and used for initializing user processor object.
    :Parameter: *objectIdField* (str) - PV update id field used for calculating data processor statistics (default: uniqueId).
    :Parameter: *objectIdOffset* (int) - This parameter determines by how much object id should change between the two PV updates, and is used for determining the number of missed PV updates (default: 1).
    :Parameter: *fieldRequest* (str) - PV field request string (default: None). This parameter can be used to request only a subset of the data available in the input channel. The system will automatically append object id field to the specified request string.
    :Parameter: *skipInitialUpdates* (int) - Number of initial PV updates that should not be processed (default: 1).
    :Parameter: *reportStatsList* (str) - Comma-separated list of statistics subsets that should be reported (default: all); possible values: monitor, queue, processor, user, all.
    :Parameter: *logLevel* (str) - Log level; possible values: debug, info, warning, error, critical. If not provided, there will be no log output.
    :Parameter: *logFile* (str) - Log file.
    :Parameter: *disableCurses* (bool) - Disable curses library screen handling. This is enabled by default, except when logging into standard output is turned on.
    :Parameter: *collectorId* (int) - Collector id (default: 1). This may be used for naming various PVA channels, so care must be taken when multiple collector processes are running independently of each other.
    :Parameter: *producerIdList* (str) - Comma-separated list of producer IDs (default: 1,2). This option can also be specified as "range(<firstId>,<lastId+1>[,<idStep>).
    :Parameter: *serverQueueSize* (int) - Server queue size (default: 0); this setting will increase memory usage on the server side, but may help prevent missed PV updates.
    :Parameter: *monitorQueueSize* (int) - PVA channel monitor (client) queue size (default: -1); if < 0, PV updates will be processed immediately without copying them into PvObjectQueue; if >= 0, PvObjectQueue will be used for receving PV updates (value of zero indicates infinite queue size).
    :Parameter: *collectorCacheSize* (int) - Collector cache size (default: -1). Collector puts all received PV updates into its cache; once the cache is full, PV updates are sorted by the objectIdField value, removed from the cache and further processed. If specified cache size is negative, or smaller than the minimum allowed value (nProducers*10), this option will be ignored.
    :Parameter: *metadataChannels* (str) - Comma-separated list of metadata channels specified in the form "protocol:\\<channelName>", where protocol can be either "ca" or "pva". If channel name is specified without a protocol, "ca" is assumed.
    '''
    def __init__(self, inputChannel, outputChannel=None, statusChannel=None, controlChannel=None, idFormatSpec=None, processorFile=None, processorClass=None, processorArgs=None, objectIdField='uniqueId', objectIdOffset=0, fieldRequest='', skipInitialUpdates=1, reportStatsList='all', logLevel=None, logFile=None, disableCurses=False, collectorId=1, producerIdList='1,2', serverQueueSize=0, monitorQueueSize=-1, collectorCacheSize=-1, metadataChannels=None):

        SystemController.__init__(self, inputChannel, outputChannel=outputChannel, statusChannel=statusChannel, controlChannel=controlChannel, idFormatSpec=idFormatSpec, processorFile=processorFile, processorClass=processorClass, processorArgs=processorArgs, objectIdField=objectIdField, objectIdOffset=objectIdOffset, fieldRequest=fieldRequest, skipInitialUpdates=skipInitialUpdates, reportStatsList=reportStatsList, logLevel=logLevel, logFile=logFile, disableCurses=disableCurses)

        self.collectorId = collectorId
        self.producerIdListSpec = producerIdList 
        self.serverQueueSize = serverQueueSize
        self.monitorQueueSize = monitorQueueSize
        self.collectorCacheSize = collectorCacheSize 
        self.metadataChannels = metadataChannels

        self.createCollector(collectorId)

    def createDataProcessorConfig(self, collectorId):
        self.logger.debug(f'Input channel: {self.inputChannel}')

        collectorIdString = self.formatIdString(collectorId)
        if self.outputChannel == '_':
            self.outputChannel = f'pvapy:collector:{collectorIdString}:output'
        if self.outputChannel:
            self.outputChannel = self.outputChannel.replace('*', collectorIdString)
            self.logger.debug(f'Processor output channel name: {self.outputChannel}')

        # Create config dict
        return SystemController.createDataProcessorConfig(self, collectorId)

    def getStatusTypeDict(self):
        statusTypeDict = DataCollector.STATUS_TYPE_DICT
        if self.processingController:
            userStatsTypeDict = self.processingController.getUserStatsPvaTypes()
            if userStatsTypeDict:
                statusTypeDict['userStats'] = userStatsTypeDict 
        for producerId in self.producerIdList:
            statusTypeDict[f'producerStats_{producerId}'] = SourceChannel.STATUS_TYPE_DICT
        for metadataChannelId in self.metadataChannelIdList:
            statusTypeDict[f'metadataStats_{metadataChannelId}'] = SourceChannel.STATUS_TYPE_DICT
        return statusTypeDict

    def createCollector(self, collectorId):
        self.logger.debug(f'Input channel name: {self.inputChannel}')

        self.producerIdList = self.generateIdList(self.producerIdListSpec)
        self.logger.debug(f'Producer id list: {self.producerIdList}')

        self.metadataChannelIdList = []
        if self.metadataChannels:
            self.metadataChannelIdList = range(1,len(self.metadataChannels.split(','))+1)
        self.logger.debug(f'Metadata channel id list: {self.metadataChannelIdList}')

        self.createDataProcessor(collectorId)
        self.createOutputChannels(collectorId)

        # Share PVA server
        self.processingController.pvaServer = self.pvaServer

        self.dataCollector = DataCollector(collectorId, self.inputChannel, producerIdList=self.producerIdList, idFormatSpec=self.idFormatSpec, objectIdField=self.objectIdField, objectIdOffset=self.objectIdOffset, fieldRequest=self.fieldRequest, serverQueueSize=self.serverQueueSize, monitorQueueSize=self.monitorQueueSize, collectorCacheSize=self.collectorCacheSize, metadataChannels=self.metadataChannels, processingController=self.processingController)

        # References used in the base class
        self.hpcObject = self.dataCollector
        self.hpcObjectId = collectorId

        return self.dataCollector

    def getStats(self):
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
                producerStatusObject = {'producerId' : producerId, 'channel' : producerStatsDict.get('channel', '')}
                producerStatusObject['monitorStats'] = producerStatsDict.get('monitorStats', {})
                producerStatusObject['queueStats'] = producerStatsDict.get('queueStats', {})
                statusObject[f'producerStats_{producerId}'] = producerStatusObject
            for metadataChannelId in self.metadataChannelIdList:
                producerStatsDict = statsDict.get(f'metadataStats', {})
                producerStatsDict = producerStatsDict.get(f'metadata-{metadataChannelId}', {})
                producerStatusObject = {'producerId' : metadataChannelId, 'channel' : producerStatsDict.get('channel', '')}
                producerStatusObject['monitorStats'] = producerStatsDict.get('monitorStats', {})
                producerStatusObject['queueStats'] = producerStatsDict.get('queueStats', {})
                statusObject[f'metadataStats_{metadataChannelId}'] = producerStatusObject
            if self.statusChannel:
                self.pvaServer.update(self.statusChannel, statusObject)
        return statsDict 

