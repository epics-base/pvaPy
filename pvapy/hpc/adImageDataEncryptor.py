#!/usr/bin/env python

import pvaccess as pva
from .dataEncryptor import DataEncryptor

class AdImageDataEncryptor(DataEncryptor):
    ''' 
    Processor for encrypting Area Detector image data. The configuration dictionary
    should provide the following settings:\n
    \t\- privateKeyFilePath (str) : defines full path to the RSA private key in PEM format.\n
    \t\- sign (bool) : if True, encrypted data will be signed (default: False)\n
 
    **AdImageDataEncryptor(configDict)**

    :Parameter: *configDict* (dict) - dictionary containing configuration parameters
    '''

    def __init__(self, configDict={}):
        DataEncryptor.__init__(self,configDict)

    def process(self, pvObject):
        ''' 
        Method invoked every time input channel updates its PV record. It encrypts
        input object and publishes result on the output channel

        :Parameter: *pvObject* (PvObject) - channel monitor update object
        '''
        # Convert to NtNdArray so that we have proper structure IDs after pickling.
        return DataEncryptor.process(self, pva.NtNdArray(pvObject))
