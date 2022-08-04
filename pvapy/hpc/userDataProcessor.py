#!/usr/bin/env python

import pvaccess as pva
from ..utility.loggingManager import LoggingManager

# Base user data processor class.
class UserDataProcessor:

    def __init__(self, configDict={}):
        self.logger = LoggingManager.getLogger(self.__class__.__name__)

        # The following will be set after object gets created.
        self.processorId = None
        self.pvaServer = None
        self.outputChannel = None
        self.objectIdField = None

    # Method called at start
    def start(self):
        pass

    # Configure user processor
    def configure(self, kwargs):
        pass

    # Process monitor update
    def process(self, pvObject):
        self.logger.debug(f'Processor {self.processorId} processing object {pvObject[self.objectIdField]}')
        self.updateOutputChannel(pvObject)
        return pvObject

    # Method called at shutdown
    def stop(self):
        pass
    
    # Reset statistics for user processor
    def resetStats(self):
        pass

    # Retrieve statistics for user processor
    def getStats(self):
        return {}

    # Define PVA types for different stats variables
    def getStatsPvaTypes(self):
        return {}

    # Define PVA types for output object
    # This method does not need to be implemented if output
    # object has the same structure as the input object
    def getOutputPvaTypes(self):
        return {}

    # Update output channel
    # No need to override this method
    def updateOutputChannel(self, pvObject):
        if not self.outputChannel or not self.pvaServer:
            return
        self.pvaServer.update(self.outputChannel, pvObject)

