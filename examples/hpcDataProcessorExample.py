#!/usr/bin/env python

from pvapy.hpc.userDataProcessor import UserDataProcessor

# Trivial data processor implementation
# All interface hooks are optional except for process()
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
