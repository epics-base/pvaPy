#!/usr/bin/env python

from pvapy.hpc.userDataProcessor import UserDataProcessor

# Trivial data processor implementation for the streaming framework
class HpcDataProcessor(UserDataProcessor):

    def __init__(self, configDict={}):
        UserDataProcessor.__init__(self, configDict)

    # Method called at start
    def start(self):
        pass

    # Configure user processor
    def configure(self, kwargs):
        pass

    # Process monitor update
    def process(self, pvObject):
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

    # Define output PvObject
    # This method does not need to be implemented if output
    # object has the same structure as the input object
    def getOutputPvObjectType(self):
        return None

