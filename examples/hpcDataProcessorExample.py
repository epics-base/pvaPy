#!/usr/bin/env python

from pvapy.hpc.userDataProcessor import UserDataProcessor

# Example for a trivial data processor implementation
class HpcDataProcessor(UserDataProcessor):

    def __init__(self, configDict={}):
        UserDataProcessor.__init__(self, configDict)

    def start(self):
        pass

    def configure(self, kwargs):
        pass

    def process(self, pvObject):
        return pvObject

    def stop(self):
        pass

    def resetStats(self):
        pass

    def getStats(self):
        return {}

    def getStatsPvaTypes(self):
        return {}

