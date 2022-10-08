#!/usr/bin/env python

import json
import threading
import time
import pvaccess as pva
from ..utility.objectUtility import ObjectUtility
from .dataProcessingController import DataProcessingController
from .sourceChannel import SourceChannel
from .dataCollector import DataCollector
from .hpcController import HpcController


class DataCollectorController(HpcController):

    CONTROLLER_TYPE = 'collector'

    def __init__(self, args):
        HpcController.__init__(self, args)
        self.createCollector(args.collector_id, args=self.args)

    def createDataProcessorConfig(self, collectorId, args):
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

    def getProducerIdList(self, args):
        # Evaluate producer id list; it should be given either as range() spec
        # or as comma-separated list.
        producerIdList = args.producer_id_list
        if not producerIdList.startswith('range') and not producerIdList.startswith('['):
            producerIdList = f'[{producerIdList}]'
        producerIdList = list(eval(producerIdList))
        return producerIdList

    def createCollector(self, collectorId, args):
        self.processingController = self.createDataProcessor(collectorId, args)
        inputChannel = args.input_channel
        self.logger.debug(f'Input channel name: {inputChannel}')

        self.producerIdList = self.getProducerIdList(args)
        self.logger.debug(f'Producer id list: {self.producerIdList}')

        self.metadataChannelIdList = []
        if args.metadata_channels:
            self.metadataChannelIdList = range(1,len(args.metadata_channels.split(','))+1)
        self.logger.debug(f'Metadata channel id list: {self.metadataChannelIdList}')

        self.createOutputChannels(collectorId, args)

        # Share PVA server
        if self.processingController:
            self.processingController.pvaServer = self.pvaServer
            self.processingController.createUserDefinedOutputChannel()

        objectIdField = self.processorConfig['objectIdField']
        objectIdOffset = self.processorConfig['objectIdOffset']
        fieldRequest = self.processorConfig['fieldRequest']
        self.dataCollector = DataCollector(collectorId, inputChannel, producerIdList=self.producerIdList, objectIdField=objectIdField, objectIdOffset=objectIdOffset, fieldRequest=fieldRequest, serverQueueSize=args.server_queue_size, monitorQueueSize=args.monitor_queue_size, collectorCacheSize=args.collector_cache_size, metadataChannels=args.metadata_channels, processingController=self.processingController)

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
            self.pvaServer.update(self.statusChannel, statusObject)
        return statsDict 

