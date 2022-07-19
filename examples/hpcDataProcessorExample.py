#!/usr/bin/env python

from pvapy.hpc.dataProcessor import DataProcessor

# Example for a data processor implementation
class HpcDataProcessor(DataProcessor):

    def __init__(self, configDict={}):
        DataProcessor.__init__(self, configDict)

    def start(self):
        DataProcessor.start(self)

    def stop(self):
        DataProcessor.stop(self)

    def getStats(self):
        return DataProcessor.getStats(self)

    def updateStats(self, t=0):
        return DataProcessor.updateStats(self, t)

    def updateOutputChannel(self, pvObject):
        DataProcessor.updateOutputChannel(self, pvObject)

    def process(self, pvObject):
        return DataProcessor.process(self, pvObject)

