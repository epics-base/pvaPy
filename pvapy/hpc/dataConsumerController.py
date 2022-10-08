#!/usr/bin/env python

import threading
import time
import json
import pvaccess as pva
from ..utility.objectUtility import ObjectUtility
from .dataProcessingController import DataProcessingController
from .sourceChannel import SourceChannel
from .dataConsumer import DataConsumer
from .hpcController import HpcController

class DataConsumerController(HpcController):

    CONTROLLER_TYPE = 'consumer'

    def __init__(self, args):
        HpcController.__init__(self, args)
        self.createConsumer(args.consumer_id, args=self.args)

    def createDataProcessorConfig(self, consumerId, args):
        oidOffset = 1
        if args.oid_offset > 0:
            oidOffset = args.oid_offset
        elif args.distributor_updates is not None:
            if args.n_distributor_sets > 1:
                self.logger.debug(f'Using oid offset appropriate for {args.n_distributor_sets} distributor client sets')
                if args.distributor_set is None:
                    raise pva.InvalidArgument(f'Specified number of distributor sets {args.n_distributor_sets} is greater than 1, but the actual distributor set name has not been set.')
                oidOffset = (args.n_distributor_sets-1)*int(args.distributor_updates)+1
            else:
                self.logger.debug('Using oid offset appropriate for a single distributor client set')
                oidOffset = (args.n_consumers-1)*int(args.distributor_updates)+1
        self.logger.debug(f'Determined oid offset: {oidOffset}')
       
        inputChannel = args.input_channel.replace('*', f'{consumerId}')
        self.logger.debug(f'Processor input channel name: {inputChannel}')

        outputChannel = args.output_channel
        if outputChannel == '_':
            outputChannel = f'pvapy:consumer:{consumerId}:output'
        if outputChannel:
            outputChannel = outputChannel.replace('*', f'{consumerId}')
            self.logger.debug(f'Processor output channel name: {outputChannel}')

        # Create config dict
        processorConfig = {}
        if args.processor_args:
            processorConfig = json.loads(args.processor_args)
        processorConfig['inputChannel'] = inputChannel
        if not 'processorId' in processorConfig:
            processorConfig['processorId'] = consumerId
        if not 'skipInitialUpdates' in processorConfig:
            processorConfig['skipInitialUpdates'] = args.skip_initial_updates
        if not 'objectIdField' in processorConfig:
            processorConfig['objectIdField'] = args.oid_field
        if not 'objectIdOffset' in processorConfig:
            processorConfig['objectIdOffset'] = oidOffset
        if not 'fieldRequest' in processorConfig:
            processorConfig['fieldRequest'] = args.field_request
        if not 'outputChannel' in processorConfig:
            processorConfig['outputChannel'] = outputChannel
        self.processorConfig = processorConfig
        return processorConfig

    def getStatusTypeDict(self):
        statusTypeDict = DataConsumer.STATUS_TYPE_DICT
        if self.processingController:
            userStatsTypeDict = self.processingController.getUserStatsPvaTypes()
            if userStatsTypeDict:
                statusTypeDict['userStats'] = userStatsTypeDict
        for metadataChannelId in self.metadataChannelIdList:
            statusTypeDict[f'metadataStats_{metadataChannelId}'] = SourceChannel.STATUS_TYPE_DICT
        return statusTypeDict

    def createConsumer(self, consumerId, args):
        self.processingController = self.createDataProcessor(consumerId, args)
        self.usingPvObjectQueue = (args.monitor_queue_size >= 0)

        inputChannel = args.input_channel.replace('*', f'{consumerId}')
        self.logger.debug(f'Input channel name: {inputChannel}')

        self.metadataChannelIdList = []
        if args.metadata_channels:
            self.metadataChannelIdList = range(1,len(args.metadata_channels.split(','))+1)
        self.logger.debug(f'Metadata channel id list: {self.metadataChannelIdList}')

        self.createOutputChannels(consumerId, args)

        # Share PVA server
        if self.processingController and self.pvaServer:
            self.processingController.pvaServer = self.pvaServer
            self.processingController.createUserDefinedOutputChannel()

        objectIdField = self.processorConfig['objectIdField']
        fieldRequest = self.processorConfig['fieldRequest']
        self.dataConsumer = DataConsumer(consumerId, inputChannel, providerType=args.input_provider_type, objectIdField=objectIdField, fieldRequest=fieldRequest, serverQueueSize=args.server_queue_size, monitorQueueSize=args.monitor_queue_size, accumulateObjects=args.accumulate_objects, accumulationTimeout=args.accumulation_timeout, distributorPluginName=args.distributor_plugin_name, distributorGroupId=args.distributor_group, distributorSetId=args.distributor_set, distributorTriggerFieldName=args.distributor_trigger, distributorUpdates=args.distributor_updates, distributorUpdateMode=None, metadataChannels=args.metadata_channels, processingController=self.processingController)

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
            self.pvaServer.update(self.statusChannel, statusObject)
        return statsDict 

    def processPvUpdate(self, updateWaitTime):
        if self.usingPvObjectQueue:
            # This should be only done for a single consumer using a queue
            return self.dataConsumer.processFromQueue(updateWaitTime)
        return False

