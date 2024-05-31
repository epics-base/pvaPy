'''
AD Image Utility class
'''

import time
import os
import numpy as np
import pvaccess as pva

# We attempt to import modules needed for compressed images
# and ignore errors if they are not there.
try:
    import blosc
except ImportError:
    pass
try:
    import lz4.block
except ImportError:
    pass
try:
    import bitshuffle
except ImportError:
    pass

class AdImageUtility:
    '''
    This class contains number of utility methods for manipulating
    Area Detector images.    
    '''
    # Area detector color modes values. Source: https://github.com/areaDetector/ADCore/blob/master/ADApp/ADSrc/NDArray.h#L29
    COLOR_MODE_MONO = 0 # [NX, NY]
    COLOR_MODE_RGB1 = 2 # [3, NX, NY]
    COLOR_MODE_RGB2 = 3 # [NX, 3, NY]
    COLOR_MODE_RGB3 = 4 # [NX, NY, 3]

    COLOR_MODE_MAP = {
        COLOR_MODE_MONO : "MONO",
        COLOR_MODE_RGB1 : "RGB1",
        COLOR_MODE_RGB2 : "RGB2",
        COLOR_MODE_RGB3 : "RGB3",
    }

    NUMPY_DATA_TYPE_MAP = {
        pva.UBYTE   : np.dtype('uint8'),
        pva.BYTE    : np.dtype('int8'),
        pva.USHORT  : np.dtype('uint16'),
        pva.SHORT   : np.dtype('int16'),
        pva.UINT    : np.dtype('uint32'),
        pva.INT     : np.dtype('int32'),
        pva.ULONG   : np.dtype('uint64'),
        pva.LONG    : np.dtype('int64'),
        pva.FLOAT   : np.dtype('float32'),
        pva.DOUBLE  : np.dtype('float64')
    }

    NTNDA_DATA_FIELD_KEY_MAP = {
        np.dtype('uint8')   : 'ubyteValue',
        np.dtype('int8')    : 'byteValue',
        np.dtype('uint16')  : 'ushortValue',
        np.dtype('int16')   : 'shortValue',
        np.dtype('uint32')  : 'uintValue',
        np.dtype('int32')   : 'intValue',
        np.dtype('uint64')  : 'ulongValue',
        np.dtype('int64')   : 'longValue',
        np.dtype('float32') : 'floatValue',
        np.dtype('float64') : 'doubleValue'
    }

    PVA_DATA_TYPE_MAP = {
        np.dtype('uint8')   : pva.UBYTE,
        np.dtype('int8')    : pva.BYTE,
        np.dtype('uint16')  : pva.USHORT,
        np.dtype('int16')   : pva.SHORT,
        np.dtype('uint32')  : pva.UINT,
        np.dtype('int32')   : pva.INT,
        np.dtype('uint64')  : pva.ULONG,
        np.dtype('int64')   : pva.LONG,
        np.dtype('float32') : pva.FLOAT,
        np.dtype('float64') : pva.DOUBLE
    }

    # Compressed payload parameters for AD codecs will
    # be determined from environment variables on the first
    # decompression attempt

    # For some HDF5 files that use LZ4 codec one may have to set
    # PVAPY_COMPRESSED_PAYLOAD_START=16

    # For some HDF5 files that use BSLZ4 codec one may have to set
    # PVAPY_COMPRESSED_PAYLOAD_START=12

    COMPRESSED_PAYLOAD_START = None
    COMPRESSED_PAYLOAD_END = None

    # Bitshuffle block size
    BSLZ4_BLOCK_SIZE = 0

    @classmethod
    def getCompressedPayload(cls, inputArray):
        if cls.COMPRESSED_PAYLOAD_START is None:
            cls.COMPRESSED_PAYLOAD_START = int(os.environ.get('PVAPY_COMPRESSED_PAYLOAD_START', 0))
            cls.COMPRESSED_PAYLOAD_END = int(os.environ.get('PVAPY_COMPRESSED_PAYLOAD_END', 0))
        if cls.COMPRESSED_PAYLOAD_END != 0:
            payload = inputArray[cls.COMPRESSED_PAYLOAD_START:cls.COMPRESSED_PAYLOAD_END]
        else:
            payload = inputArray[cls.COMPRESSED_PAYLOAD_START:]
        return payload

    @classmethod
    def getDecompressor(cls, codecName):
        utilityMap = {
            'blosc' : cls.bloscDecompress,
            'bslz4' : cls.bslz4Decompress,
            'lz4' : cls.lz4Decompress
        }
        decompressor = utilityMap.get(codecName)
        if not decompressor:
            raise pva.InvalidArgument(f'Unsupported compression: {codecName}')
        return decompressor

    @classmethod
    def bloscDecompress(cls, inputArray, inputType, uncompressedSize):
        try:
            oadt = cls.NUMPY_DATA_TYPE_MAP.get(inputType)
            oasz = uncompressedSize // oadt.itemsize
            payload = cls.getCompressedPayload(inputArray)
            outputArray = np.empty(oasz, dtype=oadt)
            nBytesWritten = blosc.decompress_ptr(bytearray(payload), outputArray.__array_interface__['data'][0])
        except NameError as ex:
            raise pva.ConfigurationError(f'Required module for bloscDecompress is missing: {ex}')
        except Exception as ex:
            raise
        return outputArray

    @classmethod
    def lz4Decompress(cls, inputArray, inputType, uncompressedSize):
        try:
            oadt = cls.NUMPY_DATA_TYPE_MAP.get(inputType)
            payload = cls.getCompressedPayload(inputArray)
            outputBytes = lz4.block.decompress(bytearray(payload), uncompressed_size=uncompressedSize)
            outputArray = np.frombuffer(outputBytes, dtype=oadt)
        except NameError as ex:
            raise pva.ConfigurationError(f'Required module for lz4Decompress is missing: {ex}')
        except Exception as ex:
            raise
        return outputArray

    @classmethod
    def bslz4Decompress(cls, inputArray, inputType, uncompressedSize):
        try:
            oadt = cls.NUMPY_DATA_TYPE_MAP.get(inputType)
            oasz = uncompressedSize // oadt.itemsize
            oash = (oasz,)
            payload = cls.getCompressedPayload(inputArray)
            outputArray = bitshuffle.decompress_lz4(payload, oash, oadt, cls.BSLZ4_BLOCK_SIZE)
        except NameError as ex:
            raise pva.ConfigurationError(f'Required module for bslz4Decompress is missing: {ex}')
        except Exception as ex:
            raise
        return outputArray

    @classmethod
    def reshapeNtNdArray(cls, ntNdArray):
        ''' Reshape area detector numpy array. '''
        # Get color mode
        imageId = ntNdArray['uniqueId']
        colorMode = None
        attributes = []
        if 'attribute' in ntNdArray:
            attributes = ntNdArray['attribute']
        for attribute in attributes:
            if attribute['name'] == 'ColorMode':
                colorMode = attribute['value'][0]['value']
                break

        # Get dimensions
        dims = ntNdArray['dimension']
        nDims = len(dims)

        if colorMode is None and nDims > 2:
            raise pva.InvalidArgument('NTNDArray does not contain ColorMode attribute.')
        colorMode = cls.COLOR_MODE_MONO

        if nDims == 0:
            nx = None
            ny = None
            nz = None
            image = None
            colorMode = None
            fieldKey = None
            return (imageId,image,nx,ny,nz,colorMode,fieldKey)
        if nDims == 2 and colorMode == cls.COLOR_MODE_MONO:
            # [NX, NY]
            nx = dims[0]['size']
            ny = dims[1]['size']
            nz = None
        elif nDims == 3 and colorMode == cls.COLOR_MODE_RGB1:
            # [3, NX, NY]
            nx = dims[1]['size']
            ny = dims[2]['size']
            nz = dims[0]['size']
        elif nDims == 3 and colorMode == cls.COLOR_MODE_RGB2:
            # [NX, 3, NY]
            nx = dims[0]['size']
            ny = dims[2]['size']
            nz = dims[1]['size']
        elif nDims == 3 and colorMode == cls.COLOR_MODE_RGB3:
            # [NX, NY, 3]
            nx = dims[0]['size']
            ny = dims[1]['size']
            nz = dims[2]['size']
        else:
            raise pva.InvalidArgument(f'Invalid dimensions {dims} and color mode {colorMode} combination.')

        # Alternative ways of getting the image array and type
        fieldKey = ntNdArray.getSelectedUnionFieldName()
        ###fieldKey = next(iter(ntNdArray['value'][0].keys()))

        #imageUnionField = ntNdArray.getUnion()
        #image = imageUnionField[fieldKey]
        ##image = ntNdArray.getUnion()[fieldKey]
        ###image = next(iter(ntNdArray['value'][0].values()))
        image = ntNdArray['value'][0][fieldKey]

        # Decompress image if needed
        codecName = ntNdArray['codec']['name']
        if codecName:
            uncompressedSize = ntNdArray['uncompressedSize']
            uncompressedType = ntNdArray['codec.parameters'][0]['value']
            decompress = cls.getDecompressor(codecName)
            image = decompress(image, uncompressedType, uncompressedSize)

        # Reshape image
        if colorMode == cls.COLOR_MODE_MONO:
            # [NX, NY]
            image = np.reshape(image, (ny, nx))

        elif colorMode == cls.COLOR_MODE_RGB1:
            # [3, NX, NY]
            image = np.reshape(image, (ny, nx, nz))

        elif colorMode == cls.COLOR_MODE_RGB2:
            # [NX, 3, NY]
            image = np.reshape(image, (ny, nz, nx))
            image = np.swapaxes(image, 2, 1)

        elif colorMode == cls.COLOR_MODE_RGB3:
            # [NX, NY, 3]
            image = np.reshape(image, (nz, ny, nx))
            image = np.swapaxes(image, 0, 2)
            image = np.swapaxes(image, 0, 1)

        else:
            raise pva.InvalidArgument(f'Unsupported color mode: {colorMode}')

        return (imageId,image,nx,ny,nz,colorMode,fieldKey)

    @classmethod
    def getNtNdArrayDataFieldKey(cls, image):
        ''' Get NTNDA data field key. '''
        return cls.NTNDA_DATA_FIELD_KEY_MAP.get(image.dtype)

    @classmethod
    def generateNtNdArray2D(cls, imageId, imageData, nx=None, ny=None, dtype=None, compressorName=None, extraFieldsPvObject=None):
        ''' Generate NTNDA for a mono image. '''
        if extraFieldsPvObject is None:
            ntNdArray = pva.NtNdArray()
        else:
            ntNdArray = pva.NtNdArray(extraFieldsPvObject.getStructureDict())

        dataFieldKey = cls.NTNDA_DATA_FIELD_KEY_MAP.get(imageData.dtype)
        data = imageData.flatten()
        if not compressorName:
            pvaDataType = cls.PVA_DATA_TYPE_MAP.get(imageData.dtype)
            ny, nx = imageData.shape
            size = nx*ny*data.itemsize
            ntNdArray['compressedSize'] = size
            ntNdArray['uncompressedSize'] = size
        else:
            dtype = np.dtype(dtype)
            pvaDataType = cls.PVA_DATA_TYPE_MAP.get(dtype)
            codec = pva.PvCodec(compressorName, pva.PvInt(int(pvaDataType)))
            ntNdArray['codec'] = codec
            size = nx*ny*dtype.itemsize
            ntNdArray['uncompressedSize'] = size
            ntNdArray['compressedSize'] = len(data)

        ntNdArray['uniqueId'] = int(imageId)
        dims = [pva.PvDimension(nx, 0, nx, 1, False), \
                pva.PvDimension(ny, 0, ny, 1, False)]
        ntNdArray['dimension'] = dims
        ts = pva.PvTimeStamp(time.time())
        ntNdArray['timeStamp'] = ts
        ntNdArray['dataTimeStamp'] = ts
        ntNdArray['descriptor'] = 'Image generated by PvaPy'

        # Alternative way of setting data
        #u = pva.PvObject({dataFieldKey : [pvaDataType]}, {dataFieldKey : data})
        #ntNdArray.setUnion(u)
        ntNdArray['value'] = {dataFieldKey : data}
        attrs = [pva.NtAttribute('ColorMode', pva.PvInt(0))]
        ntNdArray['attribute'] = attrs
        if extraFieldsPvObject is not None:
            ntNdArray.set(extraFieldsPvObject)
        return ntNdArray

    @classmethod
    def replaceNtNdArrayImage2D(cls, ntNdArray, imageId, image, extraFieldsPvObject=None):
        '''
        Assumes new image is of the same data type as the old one
        and replaces image data, dimensions, etc. in the provided NtNd Array
        '''
        dataFieldKey = cls.NTNDA_DATA_FIELD_KEY_MAP.get(image.dtype)
        pvaDataType = cls.PVA_DATA_TYPE_MAP.get(image.dtype)
        data = image.flatten()
        ntNdArray['uniqueId'] = int(imageId)

        ny, nx = image.shape
        dims = ntNdArray['dimension']
        if dims[0]['size'] != nx or dims[1]['size'] != ny:
            dims = [pva.PvDimension(nx, 0, nx, 1, False), \
                    pva.PvDimension(ny, 0, ny, 1, False)]
            ntNdArray['dimension'] = dims
            size = nx*ny*data.itemsize
            ntNdArray['compressedSize'] = size
            ntNdArray['uncompressedSize'] = size
        ts = pva.PvTimeStamp(time.time())
        ntNdArray['timeStamp'] = ts
        ntNdArray['dataTimeStamp'] = ts

        u = pva.PvObject({dataFieldKey : [pvaDataType]}, {dataFieldKey : data})
        ntNdArray.setUnion(u)
        if extraFieldsPvObject is not None:
            ntNdArray.set(extraFieldsPvObject)
        return ntNdArray

    @classmethod
    def getImageDimensions(cls, nx, ny, colorMode=COLOR_MODE_MONO):
        ''' Generate dimensions. '''
        nz = 3
        if colorMode == cls.COLOR_MODE_MONO:
            # [NX, NY]
            dims = [pva.PvDimension(nx, 0, nx, 1, False), \
                    pva.PvDimension(ny, 0, ny, 1, False)]
        elif colorMode == cls.COLOR_MODE_RGB1:
            # [3, NX, NY]
            dims = [pva.PvDimension(nz, 0, nz, 1, False), \
                    pva.PvDimension(nx, 0, nx, 1, False), \
                    pva.PvDimension(ny, 0, ny, 1, False)]

        elif colorMode == cls.COLOR_MODE_RGB2:
            # [NX, 3, NY]
            dims = [pva.PvDimension(nx, 0, nx, 1, False), \
                    pva.PvDimension(nz, 0, nz, 1, False), \
                    pva.PvDimension(ny, 0, ny, 1, False)]

        elif colorMode == cls.COLOR_MODE_RGB3:
            # [NX, NY, 3]
            dims = [pva.PvDimension(nx, 0, nx, 1, False), \
                    pva.PvDimension(ny, 0, ny, 1, False), \
                    pva.PvDimension(nz, 0, nz, 1, False)]
        else:
            raise pva.InvalidArgument(f'Unsupported color mode: {colorMode}')
        return dims

    @classmethod
    def generateNtNdArray(cls, imageId, imageData, nx=None, ny=None, colorMode=COLOR_MODE_MONO, dtype=None, compressorName=None, extraFieldsPvObject=None):
        ''' Generate NTNDA. '''
        if colorMode not in cls.COLOR_MODE_MAP:
            raise pva.InvalidArgument(f'Unsupported color mode: {colorMode}')

        if extraFieldsPvObject is None:
            ntNdArray = pva.NtNdArray()
        else:
            ntNdArray = pva.NtNdArray(extraFieldsPvObject.getStructureDict())

        dataFieldKey = cls.NTNDA_DATA_FIELD_KEY_MAP.get(imageData.dtype)
        if not compressorName:
            pvaDataType = cls.PVA_DATA_TYPE_MAP.get(imageData.dtype)
            nz = 1
            if colorMode == cls.COLOR_MODE_MONO:
                # Reverse of: image = np.reshape(image, (ny, nx))
                ny, nx = imageData.shape
            elif colorMode == cls.COLOR_MODE_RGB1:
                # Reverse of: image = np.reshape(image, (ny, nx, nz))
                ny, nx, nz = imageData.shape

            elif colorMode == cls.COLOR_MODE_RGB2:
                # Reverse of: image = np.reshape(image, (ny, nz, nx))
                #             image = np.swapaxes(image, 2, 1)
                imageData = np.swapaxes(imageData, 2, 1) # (ny,nx,nz)=>(ny,nz,nx)
                ny, nz, nx = imageData.shape

            elif colorMode == cls.COLOR_MODE_RGB3:
                # Reverse of: image = np.reshape(image, (nz, ny, nx))
                #             image = np.swapaxes(image, 0, 2)
                #             image = np.swapaxes(image, 0, 1)
                imageData = np.swapaxes(imageData, 0, 1) # (ny,nx,nz)=>(nx,ny,nz)
                imageData = np.swapaxes(imageData, 0, 2) # (nx,ny,nz)=>(nz,ny,nx)
                nz, ny, nx = imageData.shape

            size = nx*ny*nz*imageData.itemsize
            cSize = size
        else:
            nz = 3
            dtype = np.dtype(dtype)
            pvaDataType = cls.PVA_DATA_TYPE_MAP.get(dtype)
            codec = pva.PvCodec(compressorName, pva.PvInt(int(pvaDataType)))
            size = nz*nx*ny*dtype.itemsize
            cSize = len(imageData)
            ntNdArray['codec'] = codec
        dims = cls.getImageDimensions(nx, ny, colorMode)
        data = imageData.flatten()
        ts = pva.PvTimeStamp(time.time())
        attrs = [pva.NtAttribute('ColorMode', pva.PvInt(colorMode))]

        ntNdArray['uniqueId'] = int(imageId)
        ntNdArray['dimension'] = dims
        ntNdArray['compressedSize'] = cSize
        ntNdArray['uncompressedSize'] = size
        ntNdArray['timeStamp'] = ts
        ntNdArray['dataTimeStamp'] = ts
        ntNdArray['descriptor'] = 'Image generated by PvaPy'
        ntNdArray['value'] = {dataFieldKey : data}
        ntNdArray['attribute'] = attrs
        if extraFieldsPvObject is not None:
            ntNdArray.set(extraFieldsPvObject)
        return ntNdArray
