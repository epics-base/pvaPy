#!/usr/bin/env python

from pvapy.utility.loggingManager import LoggingManager
from pvapy.hpc.userMpDataProcessor import UserMpDataProcessor
from pvapy.hpc.userMpWorkerController import UserMpWorkerController
import multiprocessing as mp

# Example for implementing data processor that spawns separate unix process
class UmpDataProcessor2(UserMpDataProcessor):
    def __init__(self):
        UserMpDataProcessor.__init__(self)
        self.udp = UmpDataProcessor()
        self.iq = mp.Queue()
        self.uwpc = UserMpWorkerController(2, self.udp, self.iq)

    def start(self):
        self.uwpc.start()

    def configure(self, configDict):
        self.configure(configDict)

    def process(self, pvObject):
        self.iq.put(pvObject)
        return pvObject
	
    def resetStats(self):
        self.uwpc.resetStats()
        
    def getStats(self):
        return self.uwpc.getStats()
        
    def stop(self):
        self.uwpc.stop()

class UmpDataProcessor(UserMpDataProcessor):

    def __init__(self):
        UserMpDataProcessor.__init__(self)
        self.nProcessed = 0

    # Process monitor update
    def process(self, pvObject):
        self.nProcessed += 1
        self.logger.debug(f'Processing: {pvObject} (nProcessed: {self.nProcessed})')
        return pvObject

    # Reset statistics for user processor
    def resetStats(self):
        self.nProcessed = 0

    # Retrieve statistics for user processor
    def getStats(self):
        return {'nProcessed' : self.nProcessed}

if __name__ == '__main__':
    LoggingManager.addStreamHandler()
    LoggingManager.setLogLevel('DEBUG')
    udp = UmpDataProcessor2()
    iq = mp.Queue()
    uwpc = UserMpWorkerController(1, udp, iq)
    uwpc.start()
    import time
    for i in range(0,10):
        iq.put(i)
        time.sleep(1)
        print(uwpc.getStats())
    statsDict = uwpc.stop()
    print(statsDict)
