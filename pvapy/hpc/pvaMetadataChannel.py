#!/usr/bin/env python

import pvaccess as pva
from .sourceChannel import SourceChannel

class PvaMetadataChannel(SourceChannel):
    def __init__(self, channelId, channelName, serverQueueSize, monitorQueueSize, parentObject):
        loggerName = f'pvaMetadata-{channelId}'
        SourceChannel.__init__(self, channelId, channelName, pva.PVA, serverQueueSize, monitorQueueSize, loggerName, parentObject)

