#!/usr/bin/env python

import pvaccess as pva
from ..utility.loggingManager import LoggingManager

# Base user data processor class.
# The only method that derived class really needs to implement is process()
class UserDataProcessor:

    def __init__(self, configDict={}):
        self.logger = LoggingManager.getLogger(self.__class__.__name__)

        # The following will be set after object gets created.
        self.consumerId = None
        self.pvaServer = None
        self.outputChannel = None

    # Method called at start
    def start(self):
        pass

    # Configure user processor
    def configure(self, kwargs):
        pass

    # Process monitor update
    def process(self, pvObject):
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

    # Update output channel
    def updateOutputChannel(self, pvObject):
        if not self.outputChannel or not self.pvaServer:
            return
        self.pvaServer.update(self.outputChannel, pvObject)

