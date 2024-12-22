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
    def createMetadataChannels(cls, metadataChannels, serverQueueSize, receiverQueueSize, parentObject):
        metadataChannelMap = {}
        metadataQueueMap = {}
        if not metadataChannels:
            return (metadataChannelMap,metadataQueueMap)

        metadataReceiverQueueSize = cls.getReceiverQueueSize(receiverQueueSize)
        cls.logger.debug('Metadata client queue size is set to %s', metadataReceiverQueueSize)
        metadataChannelList = metadataChannels.split(',')
        metadataChannelId = 0
        for metadataChannel in metadataChannelList:
            if not metadataChannel:
                continue
            metadataChannelId += 1
            if metadataChannel.startswith('pva://'):
                cName = metadataChannel.replace('pva://', '')
                cls.logger.debug('Creating PVA metadata channel %s with id %s', cName, metadataChannelId)
                c = PvaMetadataChannel(metadataChannelId, cName, serverQueueSize, metadataReceiverQueueSize, parentObject)
                metadataChannelMap[metadataChannelId] = c
                metadataQueueMap[cName] = c.pvObjectQueue
            else:
                # Assume CA metadata channel 
                cName = metadataChannel.replace('ca://', '')
                cls.logger.debug('Creating CA metadata channel %s with id %s', cName, metadataChannelId)
                c = CaMetadataChannel(metadataChannelId, cName, serverQueueSize, metadataReceiverQueueSize, parentObject)
                metadataChannelMap[metadataChannelId] = c
                metadataQueueMap[cName] = c.pvObjectQueue
        return (metadataChannelMap,metadataQueueMap)

    @classmethod
    def getReceiverQueueSize(cls, receiverQueueSize):
        if receiverQueueSize < 0:
            return cls.DEFAULT_METADATA_QUEUE_SIZE 
        return receiverQueueSize
