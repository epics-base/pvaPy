#!/usr/bin/env python

import pvaccess as pva
from .userDataProcessor import UserDataProcessor
from ..utility.adImageUtility import AdImageUtility

class AdImageProcessor(UserDataProcessor):
    ''' 
    Class that can be used as a base for user implementation of an
    Area Detector image processor class suitable for usage with the streaming
    framework.
    
    **AdImageProcessor(configDict={})**

    :Parameter: *configDict* (dict) - dictionary containing configuration parameters
    '''
    def __init__(self, configDict={}):
        UserDataProcessor.__init__(self, configDict)

    @classmethod
    def reshapeNtNdArray(cls, ntNdArray):
        '''
        Reshape NtNdArray object and return tuple with image id, NumPy image array,
        image dimensions, color mode and NtNdArray value (union) field key.

        :Parameter: *ntNdArray* (NtNdArray) - NtNdArray object
        :Returns: Tuple (imageId,image,nx,ny,nz,colorMode,fieldKey). 
        '''
        return AdImageUtility.reshapeNtNdArray(ntNdArray)

    @classmethod
    def getNtNdArrayDataFieldKey(cls, image):
        '''
        Get NtNdArray value (union) field key suitable for the given NumPy array.
        
        :Parameter: *image* (numpy.array) - array containing image data
        :Returns: NtNdArray union value field key. Possible return values are 'ubyteValue', 'byteValue', 'ushortValue', 'shortValue', 'uintValue', 'intValue', 'ulongValue', 'longValue', 'floatValue', or 'doubleValue'.
        '''
        return AdImageUtility.getNtNdArrayDataFieldKey(image)

    @classmethod
    def generateNtNdArray2D(cls, imageId, image, nx=None, ny=None, dtype=None, compressorName=None, extraFieldsPvObject=None):
        '''
        Generate new NtNdArray object from NumPy array containing 2D image data.

        :Parameter: *imageId* (int) - Value for the 'uniqueId' field
        :Parameter: *image* (numpy.array) - Image data
        :Parameter: *nx* (int) - X dimension (number of columns), needed only for compressed image data
        :Parameter: *ny* (int) - Y dimension (number of rows), needed only for compressed image data
        :Parameter: *dtype* (numpy.dtype) - Array data type, needed only for compressed image data
        :Parameter: *compressorName* (str) - Compressor name, needed only for compressed image data
        :Parameter: *extraFieldsPvbject* (PvObject) - optional PvObject to be used for setting additional fields in the generated NtNdArray object
        :Returns: NtNdArray object
        '''
        return AdImageUtility.generateNtNdArray2D(imageId, image, nx=nx, ny=ny, dtype=dtype, compressorName=compressorName, extraFieldsPvObject=extraFieldsPvObject)

    @classmethod
    def replaceNtNdArrayImage2D(cls, ntNdArray, imageId, image, extraFieldsPvObject=None):
        '''
        Replace 2D image data in the existing NtNdArray object. This method is 
        slightly faster than generateNtNdArray2D().

        :Parameter: *ntNdArray* (NtNdArray) - target NtNdArray object
        :Parameter: *imageId* (int) - Value for the 'uniqueId' field
        :Parameter: *image* (numpy.array) - Image data
        :Parameter: *extraFieldsPvbject* (PvObject) - optional PvObject to be used for setting additional fields in the generated NtNdArray object
        :Returns: NtNdArray object
        '''
        return AdImageUtility.replaceNtNdArrayImage2D(ntNdArray, imageId, image, extraFieldsPvObject)
