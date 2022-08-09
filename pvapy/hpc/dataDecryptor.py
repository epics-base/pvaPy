#!/usr/bin/env python

import os
import time
import pvaccess as pva
from .userDataProcessor import UserDataProcessor
from ..utility.encryptionManager import EncryptionManager
from ..utility.floatWithUnits import FloatWithUnits
from ..objects.encryptedData import EncryptedData

class DataDecryptor(UserDataProcessor):
    ''' 
    Processor class for decrypting data and publishing it on the output channel.
    This class should be used as a base for decrypting specific PVA types. 
    The configuration dictionary should provide the following settings:\n
    \t\- privateKeyFilePath (str) : defines full path to the RSA private key in PEM format.\n
    \t\- verify (bool) : if True, encrypted data will be verified (default: False)\n
  
    **DataDecryptor(configDict)**

    :Parameter: *configDict* (dict) - dictionary containing configuration parameters
    '''

    def __init__(self, configDict={}):
        UserDataProcessor.__init__(self,configDict)
        self.privateKeyFilePath = configDict.get('privateKeyFilePath')
        if not self.privateKeyFilePath:
            raise pva.InvalidArgument(f'Private key file path not provided.')
        if not os.path.exists(self.privateKeyFilePath):
            raise pva.InvalidArgument(f'Private key file path {self.privateKeyFilePath} does not exist.')
        self.logger.debug(f'Using private key: {self.privateKeyFilePath}')
        self.verify = configDict.get('verify', False)
        self.logger.debug(f'Configured verify policy to: {self.verify}')
        self.encryptionManager = EncryptionManager(self.privateKeyFilePath)
        self.nDecrypted = 0
        self.processingTime = 0

    def configure(self, configDict):
        '''
        Method invoked at user initiated runtime configuration changes. It
        looks for 'privateKeyFilePath' and 'verify' in the configuration
        dictionary and reconfigures processor according to the specified
        value.

        :Parameter: *configDict* (dict) - dictionary containing configuration parameters
        '''
        if 'privateKeyFilePath' in configDict:
            self.privateKeyFilePath = configDict.get('privateKeyFilePath')
            self.encryptionManager = EncryptionManager(self.privateKeyFilePath)
            self.logger.debug(f'Reconfigured private key file path: {self.privateKeyFilePath}')
        if 'verify' in configDict:
            self.verify = configDict.get('verify', False)
            self.logger.debug(f'Reconfigured verify policy to: {self.verify}')

    def process(self, pvObject):
        ''' 
        Method invoked every time input channel updates its PV record. It encrypts
        input object and publishes result on the output channel

        :Parameter: *pvObject* (PvObject) - channel monitor update object
        '''
        t0 = time.time()
        objectId = pvObject[self.objectIdField]
        decryptedPvObject = self.encryptionManager.decrypt(pvObject, self.verify)
        self.updateOutputChannel(decryptedPvObject)
        t1 = time.time()
        dt = t1-t0
        self.logger.debug(f'Decrypted object {objectId} in {dt:.4f} seconds')
        self.nDecrypted += 1
        self.processingTime += dt
        return decryptedPvObject

    def resetStats(self):
        ''' 
        Method invoked at user initiated application statistics reset. It resets
        total processing time, as well as counters for the number of processed
        objects.
        '''
        self.nDecrypted = 0
        self.processingTime = 0

    def getStats(self):
        '''
        Method invoked periodically for generating processor statistics (number
        of files and bytes saved and corresponding processing/storage rates).
        
        :Returns: Dictionary containing processor statistics parameters
        '''
        processingRate = 0
        if self.processingTime > 0:
            processingRate = self.nDecrypted/self.processingTime
        return {
            'nDecrypted' : self.nDecrypted,
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
            'nDecrypted' : pva.UINT,
            'processingTime' : pva.DOUBLE,
            'processingRate' : pva.DOUBLE
        }

