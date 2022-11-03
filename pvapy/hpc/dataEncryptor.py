#!/usr/bin/env python

import os
import time
import pvaccess as pva
from .userDataProcessor import UserDataProcessor
from ..utility.encryptionManager import EncryptionManager
from ..utility.floatWithUnits import FloatWithUnits
from ..objects.encryptedData import EncryptedData

class DataEncryptor(UserDataProcessor):
    ''' 
    Processor class for encrypting data and publishing it on the output channel.
    This class should be used as a base for encrypting specific PVA types. 
    The configuration dictionary should provide
    the following settings:\n
    \t\- privateKeyFilePath (str) : defines full path to the RSA private key in PEM format.\n
    \t\- sign (bool) : if True, encrypted data will be signed (default: False)\n
  
    **DataEncryptor(configDict)**

    :Parameter: *configDict* (dict) - dictionary containing configuration parameters
    '''

    def __init__(self, configDict={}):
        UserDataProcessor.__init__(self,configDict)
        self.privateKeyFilePath = configDict.get('privateKeyFilePath')
        if not self.privateKeyFilePath:
            raise pva.InvalidArgument(f'Private key file path not provided.')
        if not self.privateKeyFilePath or not os.path.exists(self.privateKeyFilePath):
            raise pva.InvalidArgument(f'Private key file path {self.privateKeyFilePath} does not exist.')
        self.logger.debug(f'Using private key: {self.privateKeyFilePath}')
        self.sign = configDict.get('sign', False)
        self.logger.debug(f'Configured sign policy to: {self.sign}')
        self.encryptionManager = EncryptionManager(self.privateKeyFilePath)
        self.nEncrypted = 0
        self.processingTime = 0

    def configure(self, configDict):
        '''
        Method invoked at user initiated runtime configuration changes. It
        looks for 'privateKeyFilePath' and 'sign' in the configuration
        dictionary and reconfigures processor according to the specified
        value.

        :Parameter: *configDict* (dict) - dictionary containing configuration parameters
        '''
        if 'privateKeyFilePath' in configDict:
            self.privateKeyFilePath = configDict.get('privateKeyFilePath')
            self.encryptionManager = EncryptionManager(self.privateKeyFilePath)
            self.logger.debug(f'Reconfigured private key file path: {self.privateKeyFilePath}')
        if 'sign' in configDict:
            self.sign = configDict.get('sign')
            self.logger.debug(f'Reconfigured sign policy to: {self.sign}')

    def process(self, pvObject):
        ''' 
        Method invoked every time input channel updates its PV record. It encrypts
        input object and publishes result on the output channel

        :Parameter: *pvObject* (PvObject) - channel monitor update object
        '''
        t0 = time.time()
        objectId = pvObject[self.objectIdField]
        encryptedPvObject = self.encryptionManager.encrypt(pvObject, objectId, self.sign)
        self.updateOutputChannel(encryptedPvObject)
        t1 = time.time()
        dt = t1-t0
        self.logger.debug(f'Encrypted object {objectId} in {dt:.4f} seconds')
        self.nEncrypted += 1
        self.processingTime += dt
        return encryptedPvObject

    def resetStats(self):
        ''' 
        Method invoked at user initiated application statistics reset. It resets
        total processing time, as well as counters for the number of processed
        objects.
        '''
        self.nEncrypted = 0
        self.processingTime = 0

    def getStats(self):
        '''
        Method invoked periodically for generating processor statistics (number
        of files and bytes saved and corresponding processing/storage rates).
        
        :Returns: Dictionary containing processor statistics parameters
        '''
        processingRate = 0
        if self.processingTime > 0:
            processingRate = self.nEncrypted/self.processingTime
        return {
            'nEncrypted' : self.nEncrypted,
            'processingTime' : FloatWithUnits(self.processingTime, 's'),
            'processingRate' : FloatWithUnits(processingRate, 'Hz'),
        }

    def getStatsPvaTypes(self):
        '''
        Method invoked at processing startup. It defines processor part
        of the status PvObject published on the status PVA channel.
        
        :Returns: Dictionary containing PVA types for the processor statistics parameters
        '''
        return {
            'nEncrypted' : pva.UINT,
            'processingTime' : pva.DOUBLE,
            'processingRate' : pva.DOUBLE
        }

    def getOutputPvObjectType(self, pvObject):
        '''
        Method invoked at processing startup that defines PVA structure for
        the output (processed) PvObject. This method is called immediately after
        receiving the first input channel update.

        There is no need to override this method if the structure of input and
        output objects are the same, or if the application will not publish
        processing output.

        :Parameter: *pvObject* (PvObject) - input channel object
        :Returns: PvObject with the same structure as generated by the process() method
        '''
        return EncryptedData()
