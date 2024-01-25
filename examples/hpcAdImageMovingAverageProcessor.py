
import time
import numpy as np
import pvaccess as pva
from pvapy.hpc.adImageProcessor import AdImageProcessor
from pvapy.utility.floatWithUnits import FloatWithUnits

# Example processor for producing moving averages for AD images
# Rotates images by swapping x and y axes
class HpcAdImageMovingAverageProcessor(AdImageProcessor):

    DEFAULT_N_IMAGES = 10

    def __init__(self, configDict={}):
        AdImageProcessor.__init__(self, configDict)
        self.nProcessed = 0
        self.processingTime = 0
        self.imageList = []
        self.nImages = self.DEFAULT_N_IMAGES 
        self.configure(configDict)
        self.logger.debug('Created HpcAdImageMovingAverageProcessor')

    # Configure user processor
    def configure(self, kwargs):
        self.logger.debug(f'Configuration update: {kwargs}')
        if 'nImages' in kwargs:
            nImages = int(kwargs.get('nImages'))
            if nImages <= 0:
                self.logger.debug('Ignoring invalid configuration for number of images to average.')
            else:
                self.logger.debug('Configuration number of images to average: %s', nImages)
            self.nImages = nImages

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
        nImages = min(len(self.imageList),self.nImages)
        self.imageList = self.imageList[:nImages]        
        averageImage = sum(self.imageList)/nImages
        averageImage = averageImage.astype(dtype)
        self.logger.debug('Consumer %s averaged %s images after adding frame %s', self.processorId, nImages, frameId)
        pvObject2 = self.replaceNtNdArrayImage2D(pvObject, frameId, averageImage)
        self.updateOutputChannel(pvObject2)
        t1 = time.time()
        self.nProcessed += 1
        self.processingTime += (t1-t0)
        return pvObject2

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
