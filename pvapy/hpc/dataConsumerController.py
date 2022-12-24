#!/usr/bin/env python

import threading
import time
import json
import pvaccess as pva
from ..utility.objectUtility import ObjectUtility
from .dataProcessingController import DataProcessingController
from .sourceChannel import SourceChannel
from .dataConsumer import DataConsumer
from .systemController import SystemController

class DataConsumerController(SystemController):

    CONTROLLER_TYPE = 'consumer'

    ''' 
    Controller class for a single data consumer.
  
    **DataConsumerController(inputChannel, outputChannel=None, statusChannel=None, controlChannel=None, idFormatSpec=None, processorFile=None, processorClass=None, processorArgs=None, objectIdField='uniqueId', objectIdOffset=0, fieldRequest='', skipInitialUpdates=1, reportStatsList='all', logLevel=None, logFile=None, disableCurses=False, consumerId=1, nConsumers=1, inputProviderType='pva', serverQueueSize=0, monitorQueueSize=-1, accumulateObjects=-1, accumulationTimeout=1, distributorPluginName='pydistributor', distributorGroup=None, distributorSet=None, distributorTrigger=None, distributorUpdates=None, nDistributorSets=1, metadataChannels=None)**

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
    :Parameter: *consumerId* (int) - Consumer id (default: 1). Note that consumer id is used for naming various PVA channels, so care must be taken when multiple consumer processes are running independently of each other.
    :Parameter: *nConsumers* (int) - Number of consumers that will be instantiated (default: 1).
    :Parameter: *consumerIdList* (str) - Comma-separated list of consumer IDs (default: None). This option can also be specified as "range(<firstId>,<lastId+1>[,<idStep>). If this option is used, values given for <consumerId> and <nConsumers> options will be ignored; the first value from this list will be used as <consumerId>, and the length of the list will determine <nConsumers>.
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
        self.consumerId = consumerId
        self.nConsumers = nConsumers
        if consumerIdList:
            cidList = self.generateIdList(consumerIdList)
            self.nConsumers = len(cidList)
            self.consumerId = cidList[0]

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

        self.createConsumer(consumerId)

    def createDataProcessorConfig(self, consumerId):
        objectIdOffset = 1
        if self.objectIdOffset > 0:
            objectIdOffset = self.objectIdOffset
        elif self.distributorUpdates is not None:
            if self.nDistributorSets > 1:
                self.logger.debug(f'Using oid offset appropriate for {self.nDistributorSets} distributor client sets')
                if self.distributorSet is None:
                    raise pva.InvalidArgument(f'Specified number of distributor sets {self.nDistributorSets} is greater than 1, but the actual distributor set name has not been set.')
                objectIdOffset = (self.nDistributorSets-1)*int(self.distributorUpdates)+1
            else:
                self.logger.debug('Using oid offset appropriate for a single distributor client set')
                objectIdOffset = (self.nConsumers-1)*int(self.distributorUpdates)+1
        self.logger.debug(f'Determined oid offset: {objectIdOffset}')
        self.objectIdOffset = objectIdOffset
       
        consumerIdString = self.formatIdString(consumerId)
        self.inputChannel = self.inputChannel.replace('*', consumerIdString)
        self.logger.debug(f'Processor input channel name: {self.inputChannel}')

        if self.outputChannel == '_':
            self.outputChannel = f'pvapy:consumer:{consumerIdString}:output'
        if self.outputChannel:
            self.outputChannel = self.outputChannel.replace('*', consumerIdString)
            self.logger.debug(f'Processor output channel name: {self.outputChannel}')

        # Create config dict
        return SystemController.createDataProcessorConfig(self, consumerId)

    def getStatusTypeDict(self):
        statusTypeDict = DataConsumer.STATUS_TYPE_DICT
        if self.processingController:
            userStatsTypeDict = self.processingController.getUserStatsPvaTypes()
            if userStatsTypeDict:
                statusTypeDict['userStats'] = userStatsTypeDict
        for metadataChannelId in self.metadataChannelIdList:
            statusTypeDict[f'metadataStats_{metadataChannelId}'] = SourceChannel.STATUS_TYPE_DICT
        return statusTypeDict

    def createConsumer(self, consumerId):
        consumerIdString = self.formatIdString(consumerId)
        self.inputChannel = self.inputChannel.replace('*', consumerIdString)
        self.logger.debug(f'Input channel name: {self.inputChannel}')
        self.usingPvObjectQueue = (self.monitorQueueSize >= 0)

        self.metadataChannelIdList = []
        if self.metadataChannels:
            self.metadataChannelIdList = range(1,len(self.metadataChannels.split(','))+1)
        self.logger.debug(f'Metadata channel id list: {self.metadataChannelIdList}')

        self.createDataProcessor(consumerId)
        self.createOutputChannels(consumerId)

        # Share PVA server
        self.processingController.pvaServer = self.pvaServer

        self.dataConsumer = DataConsumer(consumerId, self.inputChannel, providerType=self.inputProviderType, objectIdField=self.objectIdField, fieldRequest=self.fieldRequest, serverQueueSize=self.serverQueueSize, monitorQueueSize=self.monitorQueueSize, accumulateObjects=self.accumulateObjects, accumulationTimeout=self.accumulationTimeout, distributorPluginName=self.distributorPluginName, distributorGroupId=self.distributorGroup, distributorSetId=self.distributorSet, distributorTriggerFieldName=self.distributorTrigger, distributorUpdates=self.distributorUpdates, distributorUpdateMode=None, metadataChannels=self.metadataChannels, processingController=self.processingController)

        # References used in the base class
        self.hpcObject = self.dataConsumer
        self.hpcObjectId = consumerId

        return self.dataConsumer

    def getStats(self):
        statsDict = self.dataConsumer.getStats()
        self.statsObjectId += 1
        statsDict['objectId'] = self.statsObjectId
        t = time.time()
        if self.pvaServer:
            consumerId = self.dataConsumer.getConsumerId()
            inputChannel = self.dataConsumer.inputChannel
            statusObject = pva.PvObject(self.statusTypeDict, {'consumerId' : consumerId, 'inputChannel' : inputChannel, 'objectId' : self.statsObjectId, 'objectTime' : t, 'objectTimestamp' : pva.PvTimeStamp(t)})
            statusObject['monitorStats'] = statsDict.get('monitorStats', {})
            statusObject['queueStats'] = statsDict.get('queueStats', {})
            statusObject['processorStats'] = statsDict.get('processorStats', {})
            userStatsPvaTypes = self.statusTypeDict.get('userStats', {})
            if userStatsPvaTypes: 
                userStats = statsDict.get('userStats', {})
                filteredUserStats = {}
                for k,v in userStats.items():
                    if k in userStatsPvaTypes:
                        filteredUserStats[k] = v
                statusObject['userStats'] = filteredUserStats
            for metadataChannelId in self.metadataChannelIdList:
                producerStatsDict = self.statusTypeDict.get(f'metadataStats', {})
                producerStatsDict = producerStatsDict.get(f'metadata-{metadataChannelId}', {})
                producerStatusObject = {'producerId' : metadataChannelId, 'channel' : producerStatsDict.get('channel', '')}
                producerStatusObject['monitorStats'] = producerStatsDict.get('monitorStats', {})
                producerStatusObject['queueStats'] = producerStatsDict.get('queueStats', {})
                statusObject[f'metadataStats_{metadataChannelId}'] = producerStatusObject
            if self.statusChannel:
                self.pvaServer.update(self.statusChannel, statusObject)
        return statsDict 

    def processPvUpdate(self, updateWaitTime):
        if self.usingPvObjectQueue:
            # This should be only done for a single consumer using a queue
            return self.dataConsumer.processFromQueue(updateWaitTime)
        return False

