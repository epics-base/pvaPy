
import time
import threading
import numpy as np
import pvaccess as pva
from pvapy.hpc.adImageProcessor import AdImageProcessor
from pvapy.utility.floatWithUnits import FloatWithUnits

# Example scan processor (all scan images go to single instance)
# Assuming we have two processors, the use case is as follows:
# Processor #1 receives images for entire scan #1, disconnects
# from the channel so that images for scan #2 can go to the processor
# #2, while processor #1 analyzes images it received. After analysis
# is done, processor #1 reconnects to the channel and waits for scan
# #3.

# Example commands:

# Start two consumers:
# pvapy-hpc-consumer --input-channel ad:image --output-channel scan:*:output --control-channel scan:*:control --status-channel scan:*:status --metadata-channels ad:scan_number  --report-period 10 --log-level debug --processor-file ~/GENESIS/pvapy/examples/hpcScanProcessor.py --processor-class HpcScanProcessor --processor-args '{"scanNumberPv" : "ad:scan_number", "scanProcessTime" : 30}' --server-queue-size 1000 --n-consumers 2 --distributor-updates 5000

# Start sim detector in the scan mode, with 10 seconds delay between scans:
# pvapy-ad-sim-server -id /local/sveseli/GENESIS/test_data -fnp '*ptychodus_dp.hdf5' -hds 'dp' -rp 100 -fps 100 -spv ad:scan_number -scd 10 -cn ad:image -rt 300 -dc -msc 10

class HpcScanProcessor(AdImageProcessor):

    THREAD_WAIT_PERIOD = 0.1
    N_FRAMES_REPORT_PERIOD = 100
    MIN_N_FRAMES_TO_PROCESS = 10
    DEFAULT_SCAN_WAIT_DELAY = 0.5
    DEFAULT_SCAN_PROCESS_TIME = 1.0

    def __init__(self, configDict={}):
        AdImageProcessor.__init__(self, configDict)
        self.nProcessed = 0
        self.processingTime = 0
        self.scanNumberPv = ''
        self.scanWaitDelay = self.DEFAULT_SCAN_WAIT_DELAY
        self.scanProcessTime = self.DEFAULT_SCAN_PROCESS_TIME
        self.imageList = []
        self.timeLastReceivedImage = 0
        self.currentScanNumber = None
        self.isDone = False
        self.scanThread = None
        self.configure(configDict)
        self.sleepEvent = threading.Event()
        self.logger.debug('Created HpcScanProcessor, processor id %s', self.processorId)

    def start(self):
        self.scanThread = threading.Thread(target=self.scanProcessingThread)
        self.scanThread.start()

    def processScan(self, scanNumber):
        nFrames = len(self.imageList)
        self.logger.debug('Consumer %s is about to analyze %s frames for scan %s', self.processorId, nFrames, scanNumber)
        for i,frame in enumerate(self.imageList):
            if i % self.N_FRAMES_REPORT_PERIOD == 0:
                self.logger.debug('Consumer %s analyzing frame %s (scan number %s)', self.processorId, i, scanNumber)
            self.sleepEvent.wait(self.scanProcessTime/nFrames)
        self.logger.debug('Consumer %s finished analyzing %s frames for scan %s', self.processorId, len(self.imageList), scanNumber)
        self.imageList = []

    def scanProcessingThread(self):
        self.logger.debug('Starting scan processing thread, processor id %s', self.processorId)
        while True:
            if self.isDone:
                break
            metadataQueue = self.metadataQueueMap.get(self.scanNumberPv)
            newScanNumber = self.currentScanNumber
            if metadataQueue:
                try:
                    while True:
                        newScanNumber = metadataQueue.get(0)['value']
                except pva.QueueEmpty:
                    pass
                if newScanNumber != self.currentScanNumber:
                    currentScanNumber = self.currentScanNumber
                    self.currentScanNumber = newScanNumber
                    self.logger.debug('New scan number received by processor id %s: %s', self.processorId, self.currentScanNumber)
                    self.logger.debug('Consumer %s received %s frames for scan number %s', self.processorId, len(self.imageList), currentScanNumber)
                    if len(self.imageList) > self.MIN_N_FRAMES_TO_PROCESS:
                        self.logger.debug('Stopping data receiver for processor %s', self.processorId)
                        self.sleepEvent.wait(self.scanWaitDelay)
                        self.dataReceiver.stop()
                        self.processScan(currentScanNumber)
                        self.logger.debug('Restarting data receiver for processor %s', self.processorId)
                        self.dataReceiver.start()
                    else:
                        self.logger.debug('Consumer %s discarding %s frames received for scan number %s', self.processorId, len(self.imageList), currentScanNumber)
                        self.imageList = []
        self.logger.debug('Scan processing thread for processor id %s is done', self.processorId)

    # Configure user processor
    def configure(self, kwargs):
        self.logger.debug(f'Configuration update: {kwargs}')
        if 'scanNumberPv' in kwargs:
            self.scanNumberPv = kwargs['scanNumberPv']
        if 'scanWaitDelay' in kwargs:
            self.scanWaitDelay = float(kwargs['scanWaitDelay'])
        if 'scanProcessTime' in kwargs:
            self.scanProcessTime = float(kwargs['scanProcessTime'])

    # Process monitor update
    def process(self, pvObject):
        t0 = time.time()
        (frameId,image,nx,ny,nz,colorMode,fieldKey) = self.reshapeNtNdArray(pvObject)
        if nx is None:
            self.logger.debug(f'Frame id {frameId} contains an empty image.')
            return pvObject
        dtype = image.dtype
        image = image.astype(np.float64)
        self.imageList.insert(0,image)
        if len(self.imageList) % self.N_FRAMES_REPORT_PERIOD == 0:
            self.logger.debug('Consumer %s added frame number %s, id %s', self.processorId, len(self.imageList), frameId)
        self.updateOutputChannel(pvObject)
        t1 = time.time()
        self.nProcessed += 1
        self.processingTime += (t1-t0)
        return pvObject

    # Reset statistics for user processor
    def resetStats(self):
        self.nProcessed = 0
        self.processingTime = 0

    # Retrieve statistics for user processor
    def getStats(self):
        processingRate = 0
        if self.nProcessed > 0:
            processingRate = self.nProcessed/self.processingTime
        return { 
            'nProcessed' : self.nProcessed,
            'processingTime' : FloatWithUnits(self.processingTime, 's'),
            'processingRate' : FloatWithUnits(processingRate, 'fps')
        }

    # Define PVA types for different stats variables
    def getStatsPvaTypes(self):
        return { 
            'nProcessed' : pva.UINT,
            'processingTime' : pva.DOUBLE,
            'processingRate' : pva.DOUBLE
        }

    def stop(self):
        self.isDone = True
        self.scanThread.join(timeout=self.THREAD_WAIT_PERIOD)

