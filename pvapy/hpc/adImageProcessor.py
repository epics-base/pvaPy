#!/usr/bin/env python

import pvaccess as pva
from .userDataProcessor import UserDataProcessor
from ..utility.adImageUtility import AdImageUtility

# Class that should be used as base for all AD image processing
class AdImageProcessor(UserDataProcessor):

    def __init__(self, configDict={}):
        UserDataProcessor.__init__(self, configDict)

    @classmethod
    def reshapeNtNdArray(cls, ntnda):
        return AdImageUtility.reshapeNtNdArray(ntnda)

    @classmethod
    def getNtNdArrayDataTypeKey(cls, image):
        return AdImageUtility.getNtNdArrayDataTypeKey(image)

    @classmethod
    def generateNtNdArray2D(cls, imageId, image, extraFieldsPvObject=None):
        return AdImageUtility.generateNtNdArray2D(imageId, image, extraFieldsPvObject)

    # Process monitor update
    def process(self, pvObject):
        self.logger.debug(f'Consumer {self.consumerId} processing frame {pvObject["uniqueId"]}')
        return pvObject

