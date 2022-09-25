
import time
import numpy as np
import pvaccess as pva
from pvapy.hpc.adImageProcessor import AdImageProcessor
from pvapy.utility.floatWithUnits import FloatWithUnits
from pvapy.utility.timeUtility import TimeUtility

# Example AD Metadata Processor for the streaming framework
# Updates image attributes with a values from metadata channels
class HpcAdMetadataProcessor(AdImageProcessor):

    # Acceptable difference between image timestamp and metadata timestamp
    DEFAULT_TIMESTAMP_TOLERANCE = 0.001 

    # Offset that will be applied to metadata timestamp before comparing it with
    # the image timestamp
    DEFAULT_METADATA_TIMESTAMP_OFFSET = 0

    def __init__(self, configDict={}):
        AdImageProcessor.__init__(self, configDict)
        # Configuration
        self.timestampTolerance = float(configDict.get('timestampTolerance', self.DEFAULT_TIMESTAMP_TOLERANCE))
        self.logger.debug(f'Using timestamp tolerance: {self.timestampTolerance} seconds')
        self.metadataTimestampOffset = float(configDict.get('metadataTimestampOffset', self.DEFAULT_METADATA_TIMESTAMP_OFFSET))
        self.logger.debug(f'Using metadata timestamp offset: {self.metadataTimestampOffset} seconds')

        # Statistics
        self.nFramesProcessed = 0 # Number of images associated with metadata
        self.nFrameErrors = 0 # Number of images that could not be associated with metadata
        self.nMetadataProcessed = 0 # Number of metadata values associated with images
        self.nMetadataDiscarded = 0 # Number of metadata values that were discarded
        self.processingTime = 0

        # Current metadata map       
        self.currentMetadataMap = {}

        # The last object time
        self.lastFrameTimestamp = 0

        self.logger.debug(f'Created HpcAdCaMetadataProcessor')
      

    # Configure user processor
    def configure(self, configDict):
        self.logger.debug(f'Configuration update: {configDict}')
        if 'timestampTolerance' in configDict:
            self.timestampTolerance = float(configDict.get('timestampTolerance'))
            self.logger.debug(f'Updated timestamp tolerance: {self.timestampTolerance} seconds')
        if 'metadataTimestampOffset' in configDict:
            self.metadataTimestampOffset = float(configDict.get('metadataTimestampOffset'))
            self.logger.debug(f'Updated metadata timestamp offset: {self.metadataTimestampOffset} seconds')

    # Process monitor update
    def process(self, pvObject):
        t0 = time.time()
        frameId = pvObject['uniqueId']
        dims = pvObject['dimension']
        nDims = len(dims)
        if not nDims:
            self.logger.debug(f'Frame id {frameId} contains an empty image.')
            return pvObject

        attributes = []
        if 'attribute' in pvObject:
            attributes = pvObject['attribute']

        frameTimestamp = TimeUtility.getTimeStampAsFloat(pvObject['timeStamp'])
        self.logger.debug(f'Frame id {frameId} timestamp: {frameTimestamp}')

        # self.metadataQueueMap will contain channel:pvObjectQueue map
        associationFailed = False
        for metadataChannel,metadataQueue in self.metadataQueueMap.items():
            while True:
                if metadataChannel not in self.currentMetadataMap:
                    # Get first metadata object from the queue
                    try:
                        self.currentMetadataMap[metadataChannel] = metadataQueue.get(0)
                    except pva.QueueEmpty as ex:
                        # No metadata in the queue, we failed
                        associationFailed = True 
                        break
                mdObject = self.currentMetadataMap[metadataChannel]
                mdTimestamp = TimeUtility.getTimeStampAsFloat(mdObject['timeStamp'])+self.metadataTimestampOffset
                mdValue = mdObject['value']
                self.logger.debug(f'Metadata {metadataChannel} timestamp: {mdTimestamp}, value: {mdValue}')
                if abs(frameTimestamp-mdTimestamp) <= self.timestampTolerance or mdTimestamp > self.lastFrameTimestamp:
                    # We can associate metadata with frame
                    attributes.append(pva.NtAttribute(metadataChannel, pva.PvFloat(mdValue)))
                    self.logger.debug(f'Associating frame id {frameId} with metadata {metadataChannel} value of {mdValue}')
                    self.nMetadataProcessed += 1 
                    del self.currentMetadataMap[metadataChannel]
                    break
                elif frameTimestamp > mdTimestamp:
                    # This metadata is too old, discard it and try next one
                    self.nMetadataDiscarded += 1 
                    del self.currentMetadataMap[metadataChannel]
                    self.logger.debug(f'Discarding old metadata {metadataChannel} value of {mdValue}')
                else:
                    # This metadata is newer than the frame
                    # Association failed, but keep metadata for the next frame
                    associationFailed = True 
                    self.logger.debug(f'Metadata {metadataChannel} value of {mdValue} is too new, association failed')
                    break
        
        if associationFailed:
            self.nFrameErrors += 1 
        else:
            self.nFramesProcessed += 1 
                       
        pvObject['attribute'] = attributes 
        self.updateOutputChannel(pvObject)
        self.lastFrameTimestamp = frameTimestamp
        t1 = time.time()
        self.processingTime += (t1-t0)
        return pvObject

    # Reset statistics for user processor
    def resetStats(self):
        self.nFramesProcessed = 0 
        self.nFrameErrors = 0 
        self.nMetadataProcessed = 0 
        self.nMetadataDiscarded = 0 
        self.processingTime = 0

    # Retrieve statistics for user processor
    def getStats(self):
        processedFrameRate = 0
        frameErrorRate = 0
        if self.processingTime > 0:
            processedFrameRate = self.nFramesProcessed/self.processingTime
            frameErrorRate = self.nFrameErrors/self.processingTime
        return { 
            'nFramesProcessed' : self.nFramesProcessed,
            'nFrameErrors' : self.nFrameErrors,
            'nMetadataProcessed' : self.nMetadataProcessed,
            'nMetadataDiscarded' : self.nMetadataDiscarded,
            'processingTime' : FloatWithUnits(self.processingTime, 's'),
            'processedFrameRate' : FloatWithUnits(processedFrameRate, 'fps'),
            'frameErrorRate' : FloatWithUnits(frameErrorRate, 'fps')
        }

    # Define PVA types for different stats variables
    def getStatsPvaTypes(self):
        return { 
            'nFramesProcessed' : pva.UINT,
            'nFrameErrors' : pva.UINT,
            'nMetadataProcessed' : pva.UINT,
            'nMetadataDiscarded' : pva.UINT,
            'processingTime' : pva.DOUBLE,
            'processedFrameRate' : pva.DOUBLE,
            'frameErrorRate' : pva.DOUBLE
        }
