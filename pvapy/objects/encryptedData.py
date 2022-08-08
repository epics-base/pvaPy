#!/usr/bin/env python

from pvaccess import PvObject
from .cipher import Cipher

class EncryptedData(PvObject):
    '''
    This class can be used as a container of encrypted data.
    '''

    PVA_TYPE_ID = 'pvapy:EncyptedData:1.0'
    PVA_FIELD_TYPE_ID_DICT = {
        'cipher' : Cipher.PVA_TYPE_ID
    }
    PVA_STRUCTURE_DICT = {
        'objectId' : pva.ULONG, 
        'data' : pva.STRING, 
        'key' : pva.STRING,
        'cipher' : Cipher.PVA_STRUCTURE_DICT
    }

    def __init__(self, valueDict={}):
        PvObject.__init__(self.PVA_STRUCTURE_DICT, valueDict, self.PVA_TYPE_ID, self.PVA_FIELD_TYPE_ID_DICT)
