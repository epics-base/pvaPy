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
    def getNtNdArrayDataFieldKey(cls, image):
        return AdImageUtility.getNtNdArrayDataFieldKey(image)

    # Generates new NtNdArray object
    @classmethod
    def generateNtNdArray2D(cls, imageId, image, extraFieldsPvObject=None):
        return AdImageUtility.generateNtNdArray2D(imageId, image, extraFieldsPvObject)

    # Replaces image data in the existing NtNdArray object
    @classmethod
    def replaceNtNdArrayImage2D(cls, ntnda, imageId, image, extraFieldsPvObject=None):
        return AdImageUtility.replaceNtNdArrayImage2D(ntnda, imageId, image, extraFieldsPvObject)

    # Process monitor update
    def process(self, pvObject):
        self.logger.debug(f'Processor {self.processorId} processing frame {pvObject["uniqueId"]}')
        self.updateOutputChannel(pvObject)
        return pvObject

