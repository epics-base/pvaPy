#!/usr/bin/env python

from .dataProcessor import DataProcessor
from ..utility.adImageUtility import AdImageUtility

# AD Image Processor class
class AdImageProcessor(DataProcessor):

    def __init__(self, configDict={}):
        DataProcessor.__init__(self, configDict)

    @classmethod
    def reshapeNtNdArray(cls, ntnda):
        return AdImageUtility.reshapeNtNdArray(ntnda)

    @classmethod
    def getNtNdArrayDataTypeKey(cls, image):
        return AdImageUtility.getNtNdArrayDataTypeKey(image)

    @classmethod
    def generateNtNdArray2D(cls, imageId, image, extraFieldsPvObject=None):
        return AdImageUtility.generateNtNdArray2D(imageId, image, extraFieldsPvObject)
