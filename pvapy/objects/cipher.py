#!/usr/bin/env python

import pvaccess as pva

class Cipher(pva.PvObject):
    '''
    Provides information about encryption algorithm.
    '''

    PVA_TYPE_ID = 'pvapy:Cipher:1.0'
    PVA_STRUCTURE_DICT = {
        'name' : pva.STRING,
        'parameters' : ()
    }

    def __init__(self, valueDict={}):
        pva.PvObject.__init__(self, self.PVA_STRUCTURE_DICT, valueDict, self.PVA_TYPE_ID)
