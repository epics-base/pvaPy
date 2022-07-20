#!/usr/bin/env python

from pvapy.hpc.dataProcessor import DataProcessor

# Example for a data processor implementation
class HpcDataProcessor(DataProcessor):

    def __init__(self, configDict={}):
        DataProcessor.__init__(self, configDict)

    def start(self):
        pass

    def configure(self, kwargs):
        pass

    def process(self, pvObject):
        pass

    def stop(self):
        pass
