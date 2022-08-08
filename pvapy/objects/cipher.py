#!/usr/bin/env python

from pvaccess import PvObject

class Cipher(PvObject):
    '''
    Provides information about encryption algorithm.
    '''

    PVA_TYPE_ID = 'pvapy:Cipher:1.0'
    PVA_STRUCTURE_DICT = {
        'name' : pva.STRING,
        'parameters' : [()]
    }

    def __init__(self, valueDict={}):
        PvObject.__init__(self.PVA_STRUCTURE_DICT, valueDict, self.PVA_TYPE_ID)
