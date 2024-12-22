#!/usr/bin/env python

'''
RPC Data Publisher.
'''

import pvaccess as pva
from .dataPublisher import DataPublisher

class RpcDataPublisher(DataPublisher):
    ''' 
    Class for publishing data via RPC client.
    '''

    def __init__(self, outputChannel):
        DataPublisher.__init__(self, outputChannel)
        self.rpcClient = pva.RpcClient(outputChannel)

    def updateOutputChannel(self, pvObject):
        try:
            self.rpcClient.invoke(pvObject)
            self.nPublished += 1
        except Exception as ex:
            self.nErrors += 1
            self.logger.error('Error publishing object: %s', ex)
