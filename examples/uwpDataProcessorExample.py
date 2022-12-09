#!/usr/bin/env python

from pvapy.utility.loggingManager import LoggingManager
from pvapy.hpc.userDataProcessor import UserDataProcessor
from pvapy.hpc.userWorkProcessController import UserWorkProcessController
import multiprocessing as mp

# Example for implementing data processor that spawns separate unix process
class UwpDataProcessor2(UserDataProcessor):
    def __init__(self, configDict={}):
        UserDataProcessor.__init__(self, configDict)
        self.udp = UwpDataProcessor()
        self.iq = mp.Queue()
        self.oq = mp.Queue()
        self.uwpc = UserWorkProcessController(2, self.udp, self.iq, self.oq)

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

class UwpDataProcessor(UserDataProcessor):

    def __init__(self, configDict={}):
        UserDataProcessor.__init__(self, configDict)
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
    udp = UwpDataProcessor2()
    iq = mp.Queue()
    oq = mp.Queue()
    uwpc = UserWorkProcessController(1, udp, iq, oq)
    uwpc.start()
    import time
    for i in range(0,10):
        iq.put(i)
        time.sleep(1)
        print(uwpc.getStats())
    statsDict = uwpc.stop()
    print(statsDict)
