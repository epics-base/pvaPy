#!/usr/bin/env python

'''
PVA Data Publisher.
'''

import pvaccess as pva
from .dataPublisher import DataPublisher

class PvaDataPublisher(DataPublisher):
    ''' 
    Class for publishing data via PVA client.
    '''

    def __init__(self, outputChannel):
        DataPublisher.__init__(self, outputChannel)
        self.pvaClient = pva.Channel(outputChannel)

    def updateOutputChannel(self, pvObject):
        try:
            self.pvaClient.put(pvObject, '')
            self.nPublished += 1
        except Exception as ex:
            self.nErrors += 1
            self.logger.error('Error publishing object: %s', ex)
