#!/usr/bin/env python

'''
PVAS Data Publisher.
'''

from .dataPublisher import DataPublisher

class PvasDataPublisher(DataPublisher):
    ''' 
    Class for publishing data via PVA server.
    '''

    def __init__(self, outputChannel, pvaServer, outputPvObject):
        DataPublisher.__init__(self, outputChannel)
        self.checkedUpdate = True
        self.pvaServer = pvaServer
        self.outputPvObject = outputPvObject.copy()
        self.pvaServer.addRecord(self.outputChannel, self.outputPvObject, None)
        self.logger.debug('Added output channel %s', self.outputChannel)

    def updateOutputChannel(self, pvObject):
        try:
            if self.checkedUpdate:
                self.checkedUpdate = False
                self.pvaServer.update(self.outputChannel, pvObject)
            else:
                self.pvaServer.updateUnchecked(self.outputChannel, pvObject)
            self.nPublished += 1
        except Exception as ex:
            self.nErrors += 1
            self.logger.error('Error publishing object: %s', ex)
