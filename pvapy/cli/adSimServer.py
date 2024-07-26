#!/usr/bin/env python

'''
Area Detector Simulation Server
'''

import sys
import time
import random
import tempfile
import threading
import argparse
import os
import os.path
import ctypes.util
import numpy as np
# HDF5 is optional
try:
    import h5py as h5
except ImportError:
    h5 = None
try:
    import hdf5plugin
except ImportError:
    pass

# fabio optional
try:
    import fabio
except ImportError:
    fabio = None
# yaml optional
try:
    import yaml
except ImportError:
    yaml = None

import pvaccess as pva
from pvapy.utility.adImageUtility import AdImageUtility
from pvapy.utility.floatWithUnits import FloatWithUnits
from pvapy.utility.intWithUnits import IntWithUnits
__version__ = pva.__version__

class FrameGenerator:
    ''' Base frame generator class. '''

    def __init__(self):
        self.frames = np.array([])
        self.nInputFrames = 0
        self.rows = 0
        self.cols = 0
        self.dtype = None
        self.compressorName = None
        self.colorMode = 0

    def getFrameData(self, frameId):
        if frameId < self.nInputFrames:
            return self.frames[frameId]
        return None

    def getFrameInfo(self):
        if len(self.frames) > 0 and not self.nInputFrames:
            if not self.colorMode:
                self.nInputFrames, self.rows, self.cols = self.frames.shape
            else:
                self.nInputFrames, self.rows, self.cols, _ = self.frames.shape
            self.dtype = self.frames.dtype
        return (self.nInputFrames, self.rows, self.cols, self.colorMode, self.dtype, self.compressorName)

    def getUncompressedFrameSize(self):
        return self.rows*self.cols*self.frames[0].itemsize

    def getCompressedFrameSize(self):
        if self.compressorName:
            return len(self.getFrameData(0))
        return self.getUncompressedFrameSize()

    def getCompressorName(self):
        return self.compressorName

class HdfFileGenerator(FrameGenerator):
    ''' HDF frame generator class. '''

    COMPRESSOR_NAME_MAP = {
        '32001' : 'blosc',
        '32004' : 'lz4',
        '32008' : 'bslz4'
    }

    def __init__(self, filePath, datasetPath, compressionMode=False):
        FrameGenerator.__init__(self)
        self.filePath = filePath
        self.datasetPath = datasetPath
        self.dataset = None
        self.compressionMode = compressionMode
        if not h5:
            raise Exception('Missing HDF support.')
        if not filePath:
            raise Exception('Invalid input file path.')
        if not datasetPath:
            raise Exception(f'Missing HDF dataset specification for input file {filePath}.')
        self.loadInputFile()

    def loadInputFile(self):
        try:
            self.file = h5.File(self.filePath, 'r')
            self.dataset = self.file[self.datasetPath]
            self.frames = self.dataset
            if self.compressionMode:
                for datasetId in self.dataset._filters.keys():
                    compressorName = self.COMPRESSOR_NAME_MAP.get(datasetId)
                    if compressorName:
                        self.compressorName = compressorName
                        break
            print(f'Loaded input file {self.filePath} (compressor: {self.compressorName})')
        except Exception as ex:
            print(f'Cannot load input file {self.filePath}: {ex}')
            raise

    def getFrameData(self, frameId):
        frameData = None
        if frameId < self.nInputFrames and frameId >= 0:
            if not self.compressorName:
                # Read uncompressed data
                frameData = self.frames[frameId]
            else:
                # Read compressed data directly into numpy array
                data = self.dataset.id.read_direct_chunk((frameId,0,0))
                frameData = np.frombuffer(data[1], dtype=np.uint8)
        return frameData

class FabIOFileGenerator(FrameGenerator):
    '''file generator (fabio based for alternate file formats) class'''

    def __init__(self, filePath, config):
        FrameGenerator.__init__(self)
        self.filePath = filePath
        self.cfg = config
        self.nInputFrames = 0
        self.rows = 0
        self.cols = 0
        self.file = None
        if not fabio:
            raise Exception('Missing fabio support.')
        if not filePath:
            raise Exception('Invalid input file path.')
        self.fileSize = os.path.getsize(filePath)
        if self.cfg is not None:
            self.bin = True
            if yaml is None:
                raise Exception('Please install pyyaml')
            self.success = self.loadBinInputFile()
        else:
            self.bin = False
            self.success = self.loadInputFile()

    def loadInputFile(self):
        try:
            self.file = fabio.open(self.filePath)
            self.frames = self.file.data
            if self.frames is not None:
                self.frames = np.expand_dims(self.frames, 0)
            print(f'Loaded input file {self.filePath}')
            self.nInputFrames += self.file.nframes
            return 1
        except Exception as ex:
            print(f'Cannot load input file {self.filePath}: {ex}, skipping it')
            return None

    def loadBinInputFile(self):
        try:
            image = fabio.binaryimage.BinaryImage()
            # reads in file as one big data array
            size = np.dtype(self.cfg['file_info']['datatype']).itemsize
            nFrames = int((self.fileSize-self.cfg['file_info']['header_offset']) / (self.cfg['file_info']['height'] * self.cfg['file_info']['width'] * size))
            dataDimension = self.cfg['file_info']['height'] * self.cfg['file_info']['width'] * nFrames
            print("Loading . . . ")
            images = image.read(fname=self.filePath, dim1=dataDimension, dim2=1, offset=self.cfg['file_info']['header_offset'], bytecode=self.cfg['file_info']['datatype'], endian=self.cfg['file_info']['endian'])
            self.frames = images.data
            self.frames = np.ndarray.flatten(self.frames)
            print(f'Loaded input file {self.filePath}')
            self.nInputFrames += nFrames
            self.rows = self.cfg['file_info']['height']
            self.cols = self.cfg['file_info']['width']
            return 1
        except Exception as ex:
            print(f'Cannot load input file {self.filePath}: {ex}, skipping it')
            return None

    def getFrameData(self, frameId):
        # for raw binary file: extracts a specific frame from large data array, using specifications in the config file.
        if self.bin:
            if frameId < self.nInputFrames and frameId >= 0:
                frameSize = self.cfg['file_info']['height'] * self.cfg['file_info']['width']
                offset = frameSize * frameId
                frameData = self.frames[offset:offset+frameSize]
                frameData = np.resize(frameData, (self.cfg['file_info']['height'], self.cfg['file_info']['width']))
                return frameData
            return None
        # other formats: one frame, just return data. Multiple frames, get selected frame.
        if self.nInputFrames == 1:
            return self.file.data
        elif frameId < self.nInputFrames and frameId >= 0:
            return self.file.getframe(frameId).data
        return None

    def getFrameInfo(self):
        if self.file is not None and self.frames is not None and not self.bin:
            self.dtype = self.file.dtype
            frames, self.cols, self.rows = self.frames.shape
        elif self.frames is not None and self.bin:
            self.dtype = self.frames.dtype
        return (self.nInputFrames, self.rows, self.cols, self.colorMode, self.dtype, self.compressorName)

    def isLoaded(self):
        if self.success is not None:
            return True
        return False

class NumpyFileGenerator(FrameGenerator):
    ''' NumPy file generator class. '''

    def __init__(self, filePath, mmapMode):
        FrameGenerator.__init__(self)
        self.filePath = filePath
        self.mmapMode = mmapMode
        if not filePath:
            raise Exception('Invalid input file path.')
        self.loadInputFile()

    def loadInputFile(self):
        try:
            if self.mmapMode:
                self.frames = np.load(self.filePath, mmap_mode='r')
            else:
                self.frames = np.load(self.filePath)
            print(f'Loaded input file {self.filePath}')
        except Exception as ex:
            print(f'Cannot load input file {self.filePath}: {ex}')
            raise

class NumpyRandomGenerator(FrameGenerator):
    ''' NumPy random generator class. '''

    def __init__(self, nf, nx, ny, colorMode, datatype, minimum, maximum):
        FrameGenerator.__init__(self)
        self.nf = nf
        self.nx = nx
        self.ny = ny
        self.colorMode = colorMode
        self.datatype = datatype
        self.minimum = minimum
        self.maximum = maximum
        self.generateFrames()

    def generateFrames(self):
        if self.colorMode not in AdImageUtility.COLOR_MODE_MAP:
            raise Exception(f'Invalid color mode: {self.colorMode}. Available modes: {list(AdImageUtility.COLOR_MODE_MAP.keys())}')

        print(f'Generating random frames using color mode: {AdImageUtility.COLOR_MODE_MAP[self.colorMode]}')

        # Example frame:
        # frame = np.array([[0,0,0,0,0,0,0,0,0,0],
        #                  [0,0,0,0,1,1,0,0,0,0],
        #                  [0,0,0,1,2,3,2,0,0,0],
        #                  [0,0,0,1,2,3,2,0,0,0],
        #                  [0,0,0,1,2,3,2,0,0,0],
        #                  [0,0,0,0,0,0,0,0,0,0]], dtype=np.uint16)


        frameArraySize = (self.nf, self.ny, self.nx)
        if self.colorMode != AdImageUtility.COLOR_MODE_MONO:
            frameArraySize = (self.nf, self.ny, self.nx, 3)

        dt = np.dtype(self.datatype)
        if not self.datatype.startswith('float'):
            dtinfo = np.iinfo(dt)
            mn = dtinfo.min
            if self.minimum is not None:
                mn = int(max(dtinfo.min, self.minimum))
            mx = dtinfo.max
            if self.maximum is not None:
                mx = int(min(dtinfo.max, self.maximum))
            self.frames = np.random.randint(mn, mx, size=frameArraySize, dtype=dt)
        else:
            # Use float32 for min/max, to prevent overflow errors
            dtinfo = np.finfo(np.float32)
            mn = dtinfo.min
            if self.minimum is not None:
                mn = float(max(dtinfo.min, self.minimum))
            mx = dtinfo.max
            if self.maximum is not None:
                mx = float(min(dtinfo.max, self.maximum))
            self.frames = np.random.uniform(mn, mx, size=frameArraySize)
            if self.datatype == 'float32':
                self.frames = np.float32(self.frames)

        print(f'Generated frame shape: {self.frames[0].shape}')
        print(f'Range of generated values: [{mn},{mx}]')

class AdSimServer:
    ''' AD Sim Server class. '''

    # Uses frame cache of a given size. If the number of input
    # files is larger than the cache size, the server will be constantly
    # regenerating frames.

    MIN_CACHE_SIZE = 1
    CACHE_TIMEOUT = 1.0
    DELAY_CORRECTION = 0.0001
    NOTIFICATION_DELAY = 0.1
    BYTES_IN_MEGABYTE = 1000000
    METADATA_TYPE_DICT = {
        'value' : pva.DOUBLE,
        'timeStamp' : pva.PvTimeStamp()
    }

    def __init__(self, inputDirectory, inputFile, mmapMode, hdfDataset, hdfCompressionMode, cfgFile, frameRate, nFrames, cacheSize, nx, ny, colorMode, datatype, minimum, maximum, runtime, channelName, notifyPv, notifyPvValue, metadataPv, startDelay, shutdownDelay, reportPeriod, disableCurses):
        self.lock = threading.Lock()
        self.deltaT = 0
        self.cacheTimeout = self.CACHE_TIMEOUT
        if frameRate > 0:
            self.deltaT = 1.0/frameRate
            self.cacheTimeout = max(self.CACHE_TIMEOUT, self.deltaT)
        self.runtime = runtime
        self.reportPeriod = reportPeriod
        self.metadataIoc = None
        self.frameGeneratorList = []
        self.frameCacheSize = max(cacheSize, self.MIN_CACHE_SIZE)
        self.nFrames = nFrames
        self.configFile = None
        self.colorMode = colorMode

        inputFiles = []
        if inputDirectory is not None:
            inputFiles = [os.path.join(inputDirectory, f) for f in os.listdir(inputDirectory) if os.path.isfile(os.path.join(inputDirectory, f))]
        if inputFile is not None:
            inputFiles.append(inputFile)
        allowedHdfExtensions = ['h5', 'hdf', 'hdf5']
        allowedNpExtensions = ['npy', 'npz', 'NPY']
        if nFrames > 0:
            inputFiles = inputFiles[:nFrames]
        inputFiles = sorted(inputFiles)
        if cfgFile is not None and yaml is not None:
            self.configFile = yaml.load(open(cfgFile, 'r'), Loader=yaml.CLoader)
            if self.configFile['file_info']['ordered_files'] is not None:
                inputFiles = self.configFile['file_info']['ordered_files']
        for f in inputFiles:
            ext = f.split('.')[-1]
            if ext in allowedHdfExtensions:
                self.frameGeneratorList.append(HdfFileGenerator(f, hdfDataset, hdfCompressionMode))
            elif ext not in allowedNpExtensions:
                fabioFG = FabIOFileGenerator(f, self.configFile)
                if fabioFG.isLoaded():
                    self.frameGeneratorList.append(fabioFG)
            else:
                self.frameGeneratorList.append(NumpyFileGenerator(f, mmapMode))

        if not self.frameGeneratorList:
            nf = nFrames
            if nf <= 0:
                nf = self.frameCacheSize
            self.frameGeneratorList.append(NumpyRandomGenerator(nf, nx, ny, colorMode, datatype, minimum, maximum))

        self.nInputFrames = 0
        multipleFrameImages = False
        for fg in self.frameGeneratorList:
            nInputFrames, self.rows, self.cols, colorMode, self.dtype, self.compressorName = fg.getFrameInfo()
            if nInputFrames > 1:
                multipleFrameImages = True
            self.nInputFrames += nInputFrames
        if self.nFrames > 0 and not multipleFrameImages:
            self.nInputFrames = min(self.nFrames, self.nInputFrames)

        fg = self.frameGeneratorList[0]
        self.frameRate = frameRate
        self.uncompressedImageSize = IntWithUnits(fg.getUncompressedFrameSize(), 'B')
        self.compressedImageSize = IntWithUnits(fg.getCompressedFrameSize(), 'B')
        self.compressedDataRate = FloatWithUnits(self.compressedImageSize*self.frameRate/self.BYTES_IN_MEGABYTE, 'MBps')
        self.uncompressedDataRate = FloatWithUnits(self.uncompressedImageSize*self.frameRate/self.BYTES_IN_MEGABYTE, 'MBps')

        self.channelName = channelName
        self.pvaServer = pva.PvaServer()
        self.setupMetadataPvs(metadataPv)
        self.pvaServer.addRecord(self.channelName, pva.NtNdArray(), None)

        if notifyPv and notifyPvValue:
            try:
                time.sleep(self.NOTIFICATION_DELAY)
                notifyChannel = pva.Channel(notifyPv, pva.CA)
                notifyChannel.put(notifyPvValue)
                print(f'Set notification PV {notifyPv} to {notifyPvValue}')
            except Exception as ex:
                print(f'Could not set notification PV {notifyPv} to {notifyPvValue}: {ex}')

        # Use PvObjectQueue if cache size is too small for all input frames
        # Otherwise, simple dictionary is good enough
        self.usingQueue = False
        if self.nInputFrames > self.frameCacheSize:
            self.usingQueue = True
            self.frameCache = pva.PvObjectQueue(self.frameCacheSize)
        else:
            self.frameCache = {}

        print(f'Number of input frames: {self.nInputFrames} (size: {self.cols}x{self.rows}, {self.uncompressedImageSize}, type: {self.dtype}, compressor: {self.compressorName}, compressed size: {self.compressedImageSize})')
        print(f'Frame cache type: {type(self.frameCache)} (cache size: {self.frameCacheSize})')
        print(f'Expected data rate: {self.compressedDataRate} (uncompressed: {self.uncompressedDataRate})')

        self.currentFrameId = 0
        self.nPublishedFrames = 0
        self.startTime = 0
        self.lastPublishedTime = 0
        self.startDelay = startDelay
        self.shutdownDelay = shutdownDelay
        self.isDone = False
        self.curses = None
        self.screen = None
        self.screenInitialized = False
        self.disableCurses = disableCurses

    def setupCurses(self):
        screen = None
        if not self.disableCurses:
            try:
                import curses
                screen = curses.initscr()
                self.curses = curses
            except ImportError:
                pass
        return screen

    def setupMetadataPvs(self, metadataPv):
        self.caMetadataPvs = []
        self.pvaMetadataPvs = []
        self.metadataPvs = []
        if not metadataPv:
            return
        mPvs = metadataPv.split(',')
        for mPv in mPvs:
            if not mPv:
                continue

            # Assume CA is the default protocol
            if mPv.startswith('pva://'):
                self.pvaMetadataPvs.append(mPv.replace('pva://', ''))
            else:
                self.caMetadataPvs.append(mPv.replace('ca://', ''))
        self.metadataPvs = self.caMetadataPvs+self.pvaMetadataPvs
        if self.caMetadataPvs:
            if not os.environ.get('EPICS_DB_INCLUDE_PATH'):
                pvDataLib = ctypes.util.find_library('pvData')
                if not pvDataLib:
                    raise Exception('Cannot find dbd directory, please set EPICS_DB_INCLUDE_PATH environment variable to use CA metadata PVs.')
                pvDataLib = os.path.realpath(pvDataLib)
                epicsLibDir = os.path.dirname(pvDataLib)
                dbdDir = os.path.realpath(f'{epicsLibDir}/../../dbd')
                os.environ['EPICS_DB_INCLUDE_PATH'] = dbdDir

        print(f'CA Metadata PVs: {self.caMetadataPvs}')
        if self.caMetadataPvs:
            # Create database and start CA IOC
            dbFile = tempfile.NamedTemporaryFile(delete=False)
            dbFile.write(b'record(ao, "$(NAME)") {}\n')
            dbFile.close()

            self.metadataIoc = pva.CaIoc()
            self.metadataIoc.loadDatabase('base.dbd', '', '')
            self.metadataIoc.registerRecordDeviceDriver()
            for mPv in self.caMetadataPvs:
                print(f'Creating CA metadata record: {mPv}')
                self.metadataIoc.loadRecords(dbFile.name, f'NAME={mPv}')
            self.metadataIoc.start()
            os.unlink(dbFile.name)

        print(f'PVA Metadata PVs: {self.pvaMetadataPvs}')
        if self.pvaMetadataPvs:
            for mPv in self.pvaMetadataPvs:
                print(f'Creating PVA metadata record: {mPv}')
                mPvObject = pva.PvObject(self.METADATA_TYPE_DICT)
                self.pvaServer.addRecord(mPv, mPvObject, None)

    def getMetadataValueDict(self):
        metadataValueDict = {}
        for mPv in self.metadataPvs:
            value = random.uniform(0,1)
            metadataValueDict[mPv] = value
        return metadataValueDict

    def updateMetadataPvs(self, metadataValueDict):
        # Returns time when metadata is published
        # For CA metadata will be published before data timestamp
        # For PVA metadata will have the same timestamp as data
        for mPv in self.caMetadataPvs:
            value = metadataValueDict.get(mPv)
            self.metadataIoc.putField(mPv, str(value))
        t = time.time()
        for mPv in self.pvaMetadataPvs:
            value = metadataValueDict.get(mPv)
            mPvObject = pva.PvObject(self.METADATA_TYPE_DICT, {'value' : value, 'timeStamp' : pva.PvTimeStamp(t)})
            self.pvaServer.updateUnchecked(mPv, mPvObject)
        return t

    def addFrameToCache(self, frameId, ntnda):
        if not self.usingQueue:
            # Using dictionary
            self.frameCache[frameId] = ntnda
        else:
            # Using PvObjectQueue
            try:
                waitTime = self.startDelay + self.cacheTimeout
                self.frameCache.put(ntnda, waitTime)
            except pva.QueueFull:
                pass

    def getFrameFromCache(self):
        if not self.usingQueue:
            # Using dictionary
            cachedFrameId = self.currentFrameId % self.nInputFrames
            if cachedFrameId not in self.frameCache:
                # In case frames were not generated on time, just use first frame
                cachedFrameId = 0
            ntnda = self.frameCache[cachedFrameId]
        else:
            # Using PvObjectQueue
            ntnda = self.frameCache.get(self.cacheTimeout)
        return ntnda

    def frameProducer(self, extraFieldsPvObject=None):
        frameId = 0
        frameData = None
        while not self.isDone:
            for fg in self.frameGeneratorList:
                nInputFrames, ny, nx, colorMode, dtype, compressorName = fg.getFrameInfo()
                for fgFrameId in range(0,nInputFrames):
                    if self.isDone or (self.nInputFrames > 0 and frameId >= self.nInputFrames):
                        break
                    frameData = fg.getFrameData(fgFrameId)
                    if frameData is None:
                        break
                    if self.colorMode == AdImageUtility.COLOR_MODE_MONO:
                        ntnda = AdImageUtility.generateNtNdArray2D(frameId, frameData, nx, ny, dtype, compressorName, extraFieldsPvObject)
                    else:
                        ntnda = AdImageUtility.generateNtNdArray(frameId, frameData, nx, ny, self.colorMode, dtype, compressorName, extraFieldsPvObject)
                    self.addFrameToCache(frameId, ntnda)
                    frameId += 1
            if self.isDone or not self.usingQueue or frameData is None or (self.nInputFrames > 0 and frameId >= self.nInputFrames):
                # All frames are in cache or we cannot generate any more data
                break
        self.printReport(f'Frame producer is done after {frameId} generated frames')

    def prepareFrame(self, t=0):
        # Get cached frame
        frame = self.getFrameFromCache()
        if frame is not None:
            # Correct image id and timestamps
            self.currentFrameId += 1
            frame['uniqueId'] = self.currentFrameId
            if t <= 0:
                t = time.time()
            ts = pva.PvTimeStamp(t)
            frame['timeStamp'] = ts
            frame['dataTimeStamp'] = ts
        return frame

    def framePublisher(self):
        while True:
            if self.isDone:
                return

            # Prepare metadata
            metadataValueDict = self.getMetadataValueDict()

            # Update metadata and take timestamp
            updateTime = self.updateMetadataPvs(metadataValueDict)

            # Prepare frame with a given timestamp
            # so that metadata and image times are as close as possible
            try:
                frame = self.prepareFrame(updateTime)
            except pva.QueueEmpty:
                self.printReport('Server exiting after emptying queue')
                self.isDone = True
                return
            except Exception:
                if self.isDone:
                    return
                raise

            # Publish frame
            self.pvaServer.updateUnchecked(self.channelName, frame)
            self.lastPublishedTime = time.time()
            self.nPublishedFrames += 1
            if self.usingQueue and self.nPublishedFrames >= self.nInputFrames:
                self.printReport(f'Server exiting after publishing {self.nPublishedFrames}')
                self.isDone = True
                return

            runtime = 0
            frameRate = 0
            if self.nPublishedFrames > 1:
                runtime = self.lastPublishedTime - self.startTime
                deltaT = runtime/(self.nPublishedFrames - 1)
                frameRate = 1.0/deltaT
            else:
                self.startTime = self.lastPublishedTime
            if self.reportPeriod > 0 and (self.nPublishedFrames % self.reportPeriod) == 0:
                report = f'Published frame id {self.currentFrameId:6d} @ {self.lastPublishedTime:.3f}s (frame rate: {frameRate:.4f}fps; runtime: {runtime:.3f}s)'
                self.printReport(report)

            if runtime > self.runtime:
                self.printReport(f'Server exiting after reaching runtime of {runtime:.3f} seconds')
                return

            if self.deltaT > 0:
                nextPublishTime = self.startTime + self.nPublishedFrames*self.deltaT
                delay = nextPublishTime - time.time() - self.DELAY_CORRECTION
                if delay > 0:
                    threading.Timer(delay, self.framePublisher).start()
                    return

    def printReport(self, report):
        with self.lock:
            if not self.screenInitialized:
                self.screenInitialized = True
                self.screen = self.setupCurses()
            if self.screen:
                self.screen.erase()
                self.screen.addstr(f'{report}\n')
                self.screen.refresh()
            else:
                print(report)

    def start(self):
        threading.Thread(target=self.frameProducer, daemon=True).start()
        self.pvaServer.start()
        threading.Timer(self.startDelay, self.framePublisher).start()

    def stop(self):
        self.isDone = True
        try:
            time.sleep(self.shutdownDelay)
        except KeyboardInterrupt:
            pass
        if self.screen:
            self.curses.endwin()
            self.screen = None
        self.pvaServer.stop()
        runtime = self.lastPublishedTime - self.startTime
        deltaT = 0
        frameRate = 0
        if self.nPublishedFrames > 1:
            deltaT = runtime/(self.nPublishedFrames - 1)
            frameRate = 1.0/deltaT
        dataRate = FloatWithUnits(self.uncompressedImageSize*frameRate/self.BYTES_IN_MEGABYTE, 'MBps')
        print(f'\nServer runtime: {runtime:.4f} seconds')
        print(f'Published frames: {self.nPublishedFrames:6d} @ {frameRate:.4f} fps')
        print(f'Data rate: {dataRate}')

def main():
    parser = argparse.ArgumentParser(description='PvaPy Area Detector Simulator')
    parser.add_argument('-v', '--version', action='version', version=f'%(prog)s {__version__}')
    parser.add_argument('-id', '--input-directory', type=str, dest='input_directory', default=None, help='Directory containing input files to be streamed; if input directory or input file are not provided, random images will be generated')
    parser.add_argument('-if', '--input-file', type=str, dest='input_file', default=None, help='Input file to be streamed; if input directory or input file are not provided, random images will be generated')
    parser.add_argument('-mm', '--mmap-mode', action='store_true', dest='mmap_mode', default=False, help='Use NumPy memory map to load the specified input file. This flag typically results in faster startup and lower memory usage for large files.')
    parser.add_argument('-hds', '--hdf-dataset', dest='hdf_dataset', default=None, help='HDF5 dataset path. This option must be specified if HDF5 files are used as input, but otherwise it is ignored.')
    parser.add_argument('-hcm', '--hdf-compression-mode', dest='hdf_compression_mode', default=False, action='store_true', help='Use compressed data from HDF5 file. By default, data will be uncompressed before streaming it.')
    parser.add_argument('-cfg', '--config-file', type=str, dest='config_file', default=None, help='yaml config file for raw binary data (must include for binary data), header_offset, between_frames (space between images), width, height, n_images, datatype')
    parser.add_argument('-fps', '--frame-rate', type=float, dest='frame_rate', default=20, help='Frames per second (default: 20 fps)')
    parser.add_argument('-nx', '--n-x-pixels', type=int, dest='n_x_pixels', default=256, help='Number of pixels in x dimension (default: 256 pixels; does not apply if input file file is given)')
    parser.add_argument('-ny', '--n-y-pixels', type=int, dest='n_y_pixels', default=256, help='Number of pixels in x dimension (default: 256 pixels; does not apply if input file is given)')
    parser.add_argument('-cm', '--color-mode', type=int, dest='color_mode', default=0, help='Color mode (default: 0 (mono); this option does not apply if input file is given). Available modes are: 0 (mono), 2 (RGB1, [3, NX, NY]), 3 (RGB2, [NX, 3, NY]), and 4 (RGB3, [NX, NY, 3]).')
    parser.add_argument('-dt', '--datatype', type=str, dest='datatype', default='uint8', help='Generated datatype. Possible options are int8, uint8, int16, uint16, int32, uint32, float32, float64 (default: uint8; does not apply if input file is given)')
    parser.add_argument('-mn', '--minimum', type=float, dest='minimum', default=None, help='Minimum generated value (does not apply if input file is given)')
    parser.add_argument('-mx', '--maximum', type=float, dest='maximum', default=None, help='Maximum generated value (does not apply if input file is given)')
    parser.add_argument('-nf', '--n-frames', type=int, dest='n_frames', default=0, help='Number of different frames to generate from the input sources; if set to <= 0, the server will use all images found in input files, or it will generate enough images to fill up the image cache if no input files were specified. If the requested number of input frames is greater than the cache size, the server will stop publishing after exhausting generated frames; otherwise, the generated frames will be constantly recycled and republished.')
    parser.add_argument('-cs', '--cache-size', type=int, dest='cache_size', default=1000, help='Number of different frames to cache (default: 1000); if the cache size is smaller than the number of input frames, the new frames will be constantly regenerated as cached ones are published; otherwise, cached frames will be published over and over again as long as the server is running.')
    parser.add_argument('-rt', '--runtime', type=float, dest='runtime', default=300, help='Server runtime in seconds (default: 300 seconds)')
    parser.add_argument('-cn', '--channel-name', type=str, dest='channel_name', default='pvapy:image', help='Server PVA channel name (default: pvapy:image)')
    parser.add_argument('-npv', '--notify-pv', type=str, dest='notify_pv', default=None, help='CA channel that should be notified on start; for the default Area Detector PVA driver PV that controls image acquisition is 13PVA1:cam1:Acquire')
    parser.add_argument('-nvl', '--notify-pv-value', type=str, dest='notify_pv_value', default='1', help='Value for the notification channel; for the Area Detector PVA driver PV this should be set to "Acquire" (default: 1)')
    parser.add_argument('-mpv', '--metadata-pv', type=str, dest='metadata_pv', default=None, help='Comma-separated list of CA channels that should be contain simulated image metadata values')
    parser.add_argument('-std', '--start-delay', type=float, dest='start_delay',  default=10.0, help='Server start delay in seconds (default: 10 seconds)')
    parser.add_argument('-shd', '--shutdown-delay', type=float, dest='shutdown_delay', default=10.0, help='Server shutdown delay in seconds (default: 10 seconds)')
    parser.add_argument('-rp', '--report-period', type=int, dest='report_period', default=1, help='Reporting period for publishing frames; if set to <=0 no frames will be reported as published (default: 1)')
    parser.add_argument('-dc', '--disable-curses', dest='disable_curses', default=False, action='store_true', help='Disable curses library screen handling. This is enabled by default, except when logging into standard output is turned on.')

    args, unparsed = parser.parse_known_args()
    if len(unparsed) > 0:
        print(f'Unrecognized argument(s): {" ".join(unparsed)}')
        sys.exit(1)

    server = None
    try:
        server = AdSimServer(inputDirectory=args.input_directory, inputFile=args.input_file, mmapMode=args.mmap_mode, hdfDataset=args.hdf_dataset, hdfCompressionMode=args.hdf_compression_mode, cfgFile=args.config_file, frameRate=args.frame_rate, nFrames=args.n_frames, cacheSize=args.cache_size, nx=args.n_x_pixels, ny=args.n_y_pixels, colorMode=args.color_mode, datatype=args.datatype, minimum=args.minimum, maximum=args.maximum, runtime=args.runtime, channelName=args.channel_name, notifyPv=args.notify_pv, notifyPvValue=args.notify_pv_value, metadataPv=args.metadata_pv, startDelay=args.start_delay, shutdownDelay=args.shutdown_delay, reportPeriod=args.report_period, disableCurses=args.disable_curses)

        server.start()
        expectedRuntime = args.runtime+args.start_delay
        startTime = time.time()
        runtime = 0
        try:
            while True:
                time.sleep(1)
                now = time.time()
                runtime = now - startTime
                if runtime > expectedRuntime or server.isDone:
                    break
        except KeyboardInterrupt:
            server.printReport(f'Server was interrupted after {runtime:.3f} seconds, exiting...')
    except Exception as ex:
        print(f'{str(ex)}')
    if server is not None:
        server.stop()

if __name__ == '__main__':
    main()
