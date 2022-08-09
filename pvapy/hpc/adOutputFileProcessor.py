#!/usr/bin/env python

import os
import stat
import time
import pvaccess as pva
from PIL import Image
from .adImageProcessor import AdImageProcessor
from ..utility.floatWithUnits import FloatWithUnits
from ..utility.intWithUnits import IntWithUnits

class AdOutputFileProcessor(AdImageProcessor):
    ''' 
    Streaming framework processor class that can be used for saving Area 
    Detector images into files. Configuration dictionary should provide
    the following settings:\n
    \t\- outputDirectory (str)      : defines full path to the output directory\n
    \t\- outputFileNameFormat (str) : defines format to be used for naming output files, e.g. '{uniqueId:06}.{processorId}.tiff'\n
  
    **AdImageProcessor(configDict)**

    :Parameter: *configDict* (dict) - dictionary containing configuration parameters
    '''

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

    def configure(self, configDict):
        '''
        Method invoked at user initiated runtime configuration changes. It
        looks for 'outputDirectory' and 'outputFileNameFormat' in the configuration
        dictionary and reconfigures processor behavior according to the specified
        values.

        :Parameter: *configDict* (dict) - dictionary containing configuration parameters
        '''
        if 'outputDirectory' in configDict:
            outputDirectory = configDict.get('outputDirectory')
            self.logger.debug(f'Reconfigured output directory: {outputDirectory}')
            if not os.path.exists(outputDirectory): 
                self.logger.debug(f'Creating output directory: {self.outputDirectory}')
                os.makedirs(outputDirectory)
            self.outputDirectory = outputDirectory
        if 'outputFileNameFormat' in configDict:
            self.outputFileNameFormat = configDict.get('outputFileNameFormat')
            self.logger.debug(f'Reconfigured output file name format: {self.outputFileNameFormat}')

    def process(self, pvObject):
        ''' 
        Method invoked every time input channel updates its PV record. It reshapes
        input NtNdArray object and saves image data into output file.

        :Parameter: *pvObject* (NtNdArray) - channel monitor update object
        '''
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

    def resetStats(self):
        ''' 
        Method invoked at user initiated application statistics reset. It resets
        total processing time, as well as counters for the number of files and for 
        the total number of bytes saved.
        '''
        self.nFilesSaved = 0
        self.nBytesSaved = 0
        self.fileProcessingTime = 0

    def getStats(self):
        '''
        Method invoked periodically for generating processor statistics (number
        of files and bytes saved and corresponding processing/storage rates).
        
        :Returns: Dictionary containing processor statistics parameters
        '''
        fileProcessingRate = 0
        dataStorageRateMBps = 0
        if self.fileProcessingTime > 0:
            fileProcessingRate = self.nFilesSaved/self.fileProcessingTime
            dataStorageRateMBps = self.nBytesSaved/self.fileProcessingTime/self.BYTES_IN_MEGABYTE
        return {
            'nFilesSaved' : self.nFilesSaved,
            'nBytesSaved' : IntWithUnits(self.nBytesSaved, 'B'),
            'fileProcessingTime' : FloatWithUnits(self.fileProcessingTime, 's'),
            'fileProcessingRate' : FloatWithUnits(fileProcessingRate, 'fps'),
            'dataStorageRateMBps' : FloatWithUnits(dataStorageRateMBps, 'MBps')
        }

    def getStatsPvaTypes(self):
        '''
        Method invoked at processing startup. It defines processor part
        of the status PvObject published on the status PVA channel.
        
        :Returns: Dictionary containing PVA types for the processor statistics parameters
        '''
        return {
            'nFilesSaved' : pva.UINT,
            'nBytesSaved' : pva.ULONG,
            'fileProcessingTime' : pva.DOUBLE,
            'fileProcessingRate' : pva.DOUBLE,
            'dataStorageRateMBps' : pva.DOUBLE
        }

