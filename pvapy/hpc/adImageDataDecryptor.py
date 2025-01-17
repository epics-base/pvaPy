#!/usr/bin/env python

import pvaccess as pva
from .dataDecryptor import DataDecryptor

class AdImageDataDecryptor(DataDecryptor):
    ''' 
    Processor for decrypting Area Detector image data. The configuration dictionary
    should provide the following settings:\n
    \t\- privateKeyFilePath (str) : defines full path to the RSA private key in PEM format.\n
    \t\- verify (bool) : if True, encrypted data will be verified (default: False)\n
  
    **AdImageDataDecryptor(configDict)**

    :Parameter: *configDict* (dict) - dictionary containing configuration parameters
    '''

    def __init__(self, configDict={}):
        DataDecryptor.__init__(self, configDict)

    def process(self, pvObject):
        ''' 
        Method invoked every time input channel updates its PV record. It encrypts
        input object and publishes result on the output channel

        :Parameter: *pvObject* (PvObject) - channel update object
        '''
        return DataDecryptor.process(self, pva.NtNdArray(pvObject))

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
        return pva.NtNdArray()
