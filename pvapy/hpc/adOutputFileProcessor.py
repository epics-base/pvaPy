#!/usr/bin/env python

import os
import stat
import time
import pvaccess as pva
from PIL import Image
from .adImageProcessor import AdImageProcessor

# Processor that saves output files
class AdOutputFileProcessor(AdImageProcessor):

    BYTES_IN_MEGABYTE = 1000000

    def __init__(self, configDict={}):
        AdImageProcessor.__init__(self,configDict)
        self.outputDirectory = configDict.get('outputDirectory', '.')
        self.logger.debug(f'Using output directory: {self.outputDirectory}')
        if not os.path.exists(self.outputDirectory): 
            self.logger.debug(f'Creating output directory: {self.outputDirectory}')
            os.makedirs(self.outputDirectory)
        self.outputFileNameFormat = configDict.get('outputFileNameFormat', '{uniqueId:06}.{processorId}.tiff')
        self.logger.debug(f'Using output file name format: {self.outputFileNameFormat}')
        self.nFilesSaved = 0
        self.nBytesSaved = 0
        self.fileProcessingTime = 0

    # Method called at start
    def start(self):
        pass

    # Configure user processor
    def configure(self, kwargs):
        if 'outputDirectory' in kwargs:
            outputDirectory = kwargs.get('outputDirectory')
            self.logger.debug(f'Reconfigured output directory: {outputDirectory}')
            if not os.path.exists(outputDirectory): 
                self.logger.debug(f'Creating output directory: {self.outputDirectory}')
                os.makedirs(outputDirectory)
            self.outputDirectory = outputDirectory
        if 'outputFileNameFormat' in kwargs:
            self.outputFileNameFormat = kwargs.get('outputFileNameFormat')
            self.logger.debug(f'Reconfigured output file name format: {self.outputFileNameFormat}')

    # Process monitor update
    def process(self, pvObject):
        t0 = time.time()
        (frameId,imageData,nx,ny,nz,colorMode,fieldKey) = self.reshapeNtNdArray(pvObject)
        if not nx:
            self.logger.debug(f'Frame {frameId} is empty')
            return 
        uniqueId = frameId
        objectId = frameId
        processorId = self.processorId
        filePath = os.path.join(self.outputDirectory, self.outputFileNameFormat)
        filePath = filePath.format(frameId=frameId,uniqueId=uniqueId,objectId=frameId,processorId=self.processorId)
        self.logger.debug(f'Saving frame {frameId} to file {filePath}')
        im = Image.fromarray(imageData)
        im.save(filePath)
        self.updateOutputChannel(pvObject)
        t1 = time.time()
        dt = t1-t0
        nBytesSaved = os.stat(filePath)[stat.ST_SIZE]
        self.logger.debug(f'Saved {nBytesSaved} bytes (frame {frameId}) to file {filePath} in {dt:.4f} seconds')
        self.nFilesSaved += 1
        self.nBytesSaved += nBytesSaved
        self.fileProcessingTime += dt
        return pvObject

    # Method called at shutdown
    def stop(self):
        pass
    
    # Reset statistics for user processor
    def resetStats(self):
        self.nFilesSaved = 0
        self.nBytesSaved = 0
        self.fileProcessingTime = 0

    # Retrieve statistics for user processor
    def getStats(self):
        fileProcessingRate = 0
        dataStorageRateMBps = 0
        if self.fileProcessingTime > 0:
            fileProcessingRate = self.nFilesSaved/self.fileProcessingTime
            dataStorageRateMBps = self.nBytesSaved/self.fileProcessingTime/self.BYTES_IN_MEGABYTE
        return {
            'nFilesSaved' : self.nFilesSaved,
            'nBytesSaved' : self.nBytesSaved,
            'fileProcessingTime' : self.fileProcessingTime,
            'fileProcessingRate' : fileProcessingRate,
            'dataStorageRateMBps' : dataStorageRateMBps
        }

    # Define PVA types for different stats variables
    def getStatsPvaTypes(self):
        return {
            'nFilesSaved' : pva.UINT,
            'nBytesSaved' : pva.ULONG,
            'fileProcessingTime' : pva.DOUBLE,
            'fileProcessingRate' : pva.DOUBLE,
            'dataStorageRateMBps' : pva.DOUBLE
        }

