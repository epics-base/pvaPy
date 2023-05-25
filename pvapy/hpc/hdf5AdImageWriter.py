#!/usr/bin/env python

'''
HDF5 AD Image Writer module.
'''

import os
import stat
import time
import h5py
import pvaccess as pva
from .adImageProcessor import AdImageProcessor
from ..utility.floatWithUnits import FloatWithUnits
from ..utility.intWithUnits import IntWithUnits

class Hdf5AdImageWriter(AdImageProcessor):
    '''
    Streaming framework processor class that can be used for saving Area
    Detector images into HDF5 files. Configuration dictionary should provide
    the following settings:\n
    \t\\- outputDirectory (str)      : defines full path to the output directory\n
    \t\\- outputFileNameFormat (str) : defines format to be used for naming output files, e.g. '{outputFileId:06}.{processorId}.hdf'\n
    \t\\- nImagesPerFile (int)       : number of images per output file'\n
    \t\\- datasetName (str)          : name of the dataset under which images will be saved'\n

    **Hdf5AdImageWriter(configDict)**

    :Parameter: *configDict* (dict) - dictionary containing configuration parameters
    '''

    BYTES_IN_MEGABYTE = 1000000
    DEFAULT_OUTPUT_DIRECTORY = '.'
    DEFAULT_OUTPUT_FILE_NAME_FORMAT = '{outputFileId:06}.{processorId}.hdf'
    DEFAULT_N_IMAGES_PER_FILE = 1000
    DEFAULT_DATASET_NAME = 'images'

    def __init__(self, configDict={}):
        AdImageProcessor.__init__(self,configDict)
        self.outputDirectory = configDict.get('outputDirectory', self.DEFAULT_OUTPUT_DIRECTORY)
        self.logger.debug('Using output directory: %s', self.outputDirectory)
        if not os.path.exists(self.outputDirectory):
            self.logger.debug('Creating output directory: %s', self.outputDirectory)
            os.makedirs(self.outputDirectory, exist_ok=True)
        self.outputFileNameFormat = configDict.get('outputFileNameFormat', self.DEFAULT_OUTPUT_FILE_NAME_FORMAT)
        self.logger.debug('Using output file name format: %s', self.outputFileNameFormat)
        self.nImagesPerFile = int(configDict.get('nImagesPerFile', self.DEFAULT_N_IMAGES_PER_FILE))
        self.logger.debug('Number of images per output file: %s', self.nImagesPerFile)
        self.datasetName = configDict.get('datasetName', self.DEFAULT_DATASET_NAME)
        self.logger.debug('Dataset name: %s', self.datasetName)

        self.nDatasetImages = 0
        self.outputFileId = 0
        self.nFilesSaved = 0
        self.nBytesSaved = 0
        self.filePath = ''
        self.h5File = None
        self.h5Dataset = None
        self.lastFileProcessedTime = 0
        self.lastFrameProcessedTime = 0
        self.fileProcessingTime = 0

    def _closeOutputFile(self):
        if self.h5File:
            try:
                self.h5File.close()
                self.h5File = None
            except Exception:
                pass

    def configure(self, configDict):
        '''
        Method invoked at user initiated runtime configuration changes. It
        looks for 'outputDirectory', 'outputFileNameFormat',
        'nImagesPerFile' and 'datasetName' keys in the configuration
        dictionary and reconfigures processor behavior according
        to the specified values.

        :Parameter: *configDict* (dict) - dictionary containing configuration parameters
        '''
        if 'outputDirectory' in configDict:
            outputDirectory = configDict.get('outputDirectory')
            self.logger.debug('Reconfigured output directory: %s', outputDirectory)
            if not os.path.exists(outputDirectory):
                self.logger.debug('Creating output directory: %s', outputDirectory)
                os.makedirs(outputDirectory, exist_ok=True)
            self.outputDirectory = outputDirectory
        if 'outputFileNameFormat' in configDict:
            self.outputFileNameFormat = configDict.get('outputFileNameFormat')
            self.logger.debug('Reconfigured output file name format: %s', self.outputFileNameFormat)
        if 'nImagesPerFile' in configDict:
            self.nImagesPerFile = int(configDict.get('nImagesPerFile'))
            self.logger.debug('Reconfigured output file name format: %s', self.nImagesPerFile)
        if 'datasetName' in configDict:
            self.datasetName = configDict.get('datasetName')
            self.logger.debug('Reconfigured dataset name: %s', self.datasetName)

    def process(self, pvObject):
        '''
        Method invoked every time input channel updates its PV record.
        It reshapes input NtNdArray object and saves image data into
        output file.

        :Parameter: *pvObject* (NtNdArray) - channel monitor update object
        '''
        t0 = time.time()
        (frameId,imageData,nx,ny,nz,_,_) = self.reshapeNtNdArray(pvObject)
        if not nx:
            self.logger.debug('Frame %s is empty', frameId)
            return pvObject
        if not self.nDatasetImages:
            self.outputFileId += 1
            self.filePath = os.path.join(self.outputDirectory, self.outputFileNameFormat)
            self.filePath = self.filePath.format(frameId=frameId,uniqueId=frameId,objectId=frameId,processorId=self.processorId,outputFileId=self.outputFileId)
            self.logger.debug('Opening output file id %s (%s); it should contain %s images', self.outputFileId, self.filePath, self.nImagesPerFile)
            self.h5File = h5py.File(self.filePath,'w')
            if not nz:
                self.h5Dataset = self.h5File.create_dataset(self.datasetName, shape=(self.nImagesPerFile, ny, nx), dtype=imageData.dtype)
            else:
                self.h5Dataset = self.h5File.create_dataset(self.datasetName, shape=(self.nImagesPerFile, ny, nx, nz), dtype=imageData.dtype)
        if not self.h5File:
            self.logger.warning('Output HDF5 file is closed')
            return pvObject

        if not nz:
            self.h5Dataset[self.nDatasetImages:self.nDatasetImages+1:,:] = imageData
        else:
            self.h5Dataset[self.nDatasetImages:self.nDatasetImages+1:,:,:] = imageData
        self.logger.debug('Added frame %s to output file id %s', frameId, self.outputFileId)
        self.nDatasetImages += 1

        if self.nDatasetImages == self.nImagesPerFile:
            self._closeOutputFile()
            t1 = time.time()
            dt = t1-t0
            self.nDatasetImages = 0
            nBytesSaved = os.stat(self.filePath)[stat.ST_SIZE]
            self.logger.debug('Saved %s bytes (frame %s) to file %s in %.4f seconds', self.nBytesSaved, frameId, self.filePath, dt)
            self.nFilesSaved += 1
            self.nBytesSaved += nBytesSaved
            self.lastFileProcessedTime = t1

        self.updateOutputChannel(pvObject)
        t1 = time.time()
        dt = t1-t0
        self.fileProcessingTime += dt
        self.lastFrameProcessedTime = t1
        return pvObject

    def stop(self):
        self._closeOutputFile()

    def resetStats(self):
        '''
        Method invoked at user initiated application statistics reset.
        It resets total processing time, as well as counters for the
        number of files and for the total number of bytes saved.
        '''
        self.nFilesSaved = 0
        self.nBytesSaved = 0
        self.fileProcessingTime = 0
        self.lastFileProcessedTime = 0
        self.lastFrameProcessedTime = 0

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
            'dataStorageRateMBps' : FloatWithUnits(dataStorageRateMBps, 'MBps'),
            'lastFileProcessedTime' : FloatWithUnits(self.lastFileProcessedTime, 's'),
            'lastFrameProcessedTime' : FloatWithUnits(self.lastFrameProcessedTime, 's')
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
            'dataStorageRateMBps' : pva.DOUBLE,
            'lastFileProcessedTime' : pva.DOUBLE,
            'lastFrameProcessedTime' : pva.DOUBLE
        }
