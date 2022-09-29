#!/usr/bin/env python

import pvaccess as pva
from .sourceChannel import SourceChannel

class CaMetadataChannel(SourceChannel):
    def __init__(self, channelId, channelName, serverQueueSize, monitorQueueSize, parentObject):
        loggerName = f'caMetadata-{channelId}'
        SourceChannel.__init__(self, channelId, channelName, pva.CA, serverQueueSize, monitorQueueSize, loggerName, parentObject)

    def getPvMonitorRequest(self):
        recordStr = ''
        if self.serverQueueSize > 0:
            recordStr = f'record[queueSize={self.serverQueueSize}]'
        request = f'{recordStr}field(value,timeStamp)'
        return request

