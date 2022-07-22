#!/usr/bin/env python

import numpy as np
import pvaccess as pva
from .dataProcessor import DataProcessor

# AD Image Processor class
class AdImageProcessor(DataProcessor):

    # Area detector color modes values. Source: https://github.com/areaDetector/ADCore/blob/master/ADApp/ADSrc/NDArray.h#L29
    COLOR_MODE_MONO = 0 # [NX, NY]
    COLOR_MODE_RGB1 = 2 # [3, NX, NY]
    COLOR_MODE_RGB2 = 3 # [NX, 3, NY]
    COLOR_MODE_RGB3 = 4 # [NX, NY, 3]

    COLOR_MODES = {
        COLOR_MODE_MONO : "MONO",
        COLOR_MODE_RGB1 : "RGB1",
        COLOR_MODE_RGB2 : "RGB2",
        COLOR_MODE_RGB3 : "RGB3",
    }

    def __init__(self, configDict={}):
        DataProcessor.__init__(self, configDict)

    @classmethod
    def reshapeFrame(cls, frame):
        """ Reshape area detector numpy array. """
        # Get color mode
        attributes = frame['attribute']
        colorMode = None
        for attribute in attributes:
            if attribute['name'] == 'ColorMode':
                colorMode = attribute['value'][0]['value']
                break

        # Get dimensions
        dims = frame['dimension']
        nDims = len(dims)

        if colorMode is None and nDims > 2:
            raise pva.InvalidArgument('NTNDArray does not contain ColorMode attribute.')
        else:
            colorMode = cls.COLOR_MODE_MONO

        if nDims == 2 and colorMode == cls.COLOR_MODE_MONO:
            nx = dims[0]['size']
            ny = dims[1]['size']
            nz = None
        elif nDims == 3 and colorMode == cls.COLOR_MODE_RGB1:
            nx = dims[1]['size']
            ny = dims[2]['size']
            nz = dims[0]['size']
        elif nDims == 3 and colorMode == cls.COLOR_MODE_RGB2:
            nx = dims[0]['size']
            ny = dims[2]['size']
            nz = dims[1]['size']
        elif nDims == 3 and colorMode == cls.COLOR_MODE_RGB3:
            nx = dims[0]['size']
            ny = dims[1]['size']
            nz = dims[2]['size']
        else:
            raise pva.InvalidArgument(f'Invalid dimensions {dims} and color mode {colorMode} combination.')

        # Alternative ways of getting the image array and type
        fieldKey = frame.getSelectedUnionFieldName()
        ###fieldKey = next(iter(frame['value'][0].keys()))

        #imageUnionField = frame.getUnion()
        #image = imageUnionField[fieldKey]
        ##image = frame.getUnion()[fieldKey]
        ###image = next(iter(frame['value'][0].values()))
        image = frame['value'][0][fieldKey]

        if colorMode == cls.COLOR_MODE_MONO:
            image = np.reshape(image, (ny, nx))

        elif colorMode == cls.COLOR_MODE_RGB1:
            image = np.reshape(image, (ny, nx, nz))

        elif colorMode == cls.COLOR_MODE_RGB2:
            image = np.reshape(image, (ny, nz, nx))
            image = np.swapaxes(image, 2, 1)

        elif colorMode == cls.COLOR_MODE_RGB3:
            image = np.reshape(image, (nz, ny, nx))
            image = np.swapaxes(image, 0, 2)
            image = np.swapaxes(image, 0, 1)

        else:
            raise pva.InvalidArgument('Unsupported color mode: {colorMode}')

        return (image,nx,ny,nz,colorMode,fieldKey)

    def process(self, pvObject):
        return DataProcessor.process(self, pvObject)
