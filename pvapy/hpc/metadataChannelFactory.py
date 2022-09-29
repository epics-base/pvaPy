#!/usr/bin/env python

import pvaccess as pva
from .caMetadataChannel import CaMetadataChannel
from .pvaMetadataChannel import PvaMetadataChannel
from ..utility.loggingManager import LoggingManager

class MetadataChannelFactory:

    # Default metadata queue size
    DEFAULT_METADATA_QUEUE_SIZE = 1000
    logger = LoggingManager.getLogger('MetadataChannelFactory')

    @classmethod
    def createMetadataChannels(cls, metadataChannels, serverQueueSize, monitorQueueSize, parentObject):
        metadataChannelMap = {}
        metadataQueueMap = {}
        if not metadataChannels:
            return (metadataChannelMap,metadataQueueMap)

        metadataMonitorQueueSize = cls.getMonitorQueueSize(monitorQueueSize)
        cls.logger.debug(f'Metadata client queue size is set to {metadataMonitorQueueSize}')
        metadataChannelList = metadataChannels.split(',')
        metadataChannelId = 0
        for metadataChannel in metadataChannelList:
            if not metadataChannel:
                continue
            metadataChannelId += 1
            if metadataChannel.startswith('pva://'):
                cName = metadataChannel.replace('pva://', '')
                cls.logger.debug(f'Creating PVA metadata channel {cName} with id {metadataChannelId}')
                c = PvaMetadataChannel(metadataChannelId, cName, serverQueueSize, metadataMonitorQueueSize, parentObject)
                metadataChannelMap[metadataChannelId] = c
                metadataQueueMap[cName] = c.pvObjectQueue
            else:
                # Assume CA metadata channel 
                cName = metadataChannel.replace('ca://', '')
                cls.logger.debug(f'Creating CA metadata channel {cName} with id {metadataChannelId}')
                c = CaMetadataChannel(metadataChannelId, cName, serverQueueSize, metadataMonitorQueueSize, parentObject)
                metadataChannelMap[metadataChannelId] = c
                metadataQueueMap[cName] = c.pvObjectQueue
        return (metadataChannelMap,metadataQueueMap)

    @classmethod
    def getMonitorQueueSize(cls, monitorQueueSize):
        if monitorQueueSize < 0:
            return cls.DEFAULT_METADATA_QUEUE_SIZE 
        return monitorQueueSize 

