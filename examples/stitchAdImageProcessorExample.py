
import time
import threading
import multiprocessing as mp
import queue
import numpy as np
import pvaccess as pva
from pvapy.hpc.adImageProcessor import AdImageProcessor
from pvapy.utility.adImageUtility import AdImageUtility
from pvapy.utility.floatWithUnits import FloatWithUnits

# Stitch AD Image Processor for the streaming framework
# Stitches image tiles into a larger image and reverses
# SplitAdImageProcessor
class StitchAdImageProcessor(AdImageProcessor):

    def __init__(self, configDict):
        AdImageProcessor.__init__(self, configDict)
        self.tileCache = {}
        self.frameTileCounter = {}
        self.stitchQueue = mp.Queue(maxsize=-1)
        self.lastFrameId = None
        self.isDone = False

        # Stats
        self.nFramesStitched = 0
        self.nFramesMissed = 0
        self.nTilesProcessed = 0
        self.nTilesReceived = 0
        self.nTilesDiscarded = 0
        self.totalTileProcessingTime = 0
        self.totalFrameStitchingTime = 0

        self.configure(configDict)
        self.logger.debug('Created StitchAdImageProcessor')

    # Configure user processor
    def configure(self, configDict):
        self.logger.debug('Configuration update: %s', configDict)
        self.nx = int(configDict.get('nx', 0))
        self.ny = int(configDict.get('ny', 0))
        self.nxTiles = 0  # number of tiles in x dimension
        self.nyTiles = 0  # number of tiles in y dimension
        self.nxyTiles = 0 # total number of image tiles in both dimensions
        if self.nx and self.ny:
            self.logger.debug('Image tiles with the same id will be stitched into a larger image with %s rows, %s columns', self.ny, self.nx)

    def _stitchWorker(self):
        self.logger.debug('Starting stitching worker')
        waitTime = 1
        while True:
            if self.isDone:
                break
            try:
                # Stitch frame
                frameId = self.stitchQueue.get(block=True, timeout=waitTime)
                t0 = time.time()
                self.logger.debug('Stitching frame id %s', frameId)
                tiles = np.array(self.tileCache[frameId])
                image = tiles.swapaxes(1,2)
                image = image.reshape(self.ny, self.nx)
                imagePvObject = AdImageUtility.generateNtNdArray2D(frameId, image)
                t1 = time.time()

                # Publish frame
                self.updateOutputChannel(imagePvObject)

                # Update stats
                dt = t1-t0
                self.totalFrameStitchingTime += dt
                self.logger.debug('Stitched frame id %s in %s seconds', frameId, dt)

                self.nFramesStitched += 1
                self.nTilesProcessed += self.nxyTiles
                nFramesMissed = 0
                if self.lastFrameId is not None:
                    nFramesMissed = frameId-self.lastFrameId -1
                self.lastFrameId = frameId
                if nFramesMissed > 0:  
                    self.nFramesMissed += nFramesMissed

                # Keep internal caches clean
                for fId in list(self.frameTileCounter.keys()):
                    if fId >= frameId:
                        break
                    nFrameTilesReceived = self.frameTileCounter[fId]
                    self.nTilesDiscarded += nFrameTilesReceived
                    del self.frameTileCounter[fId]
                    del self.tileCache[fId]
                del self.frameTileCounter[frameId]
                del self.tileCache[frameId]
        
            except queue.Empty:
                continue
            except KeyboardInterrupt:
                self.isDone = True
            except Exception as ex:
                self.logger.error('Unexpected error caught: %s', ex)
        self.logger.debug('Stitching worker is done')

    def start(self):
        self._stitchThread = threading.Thread(target=self._stitchWorker)
        self._stitchThread.start()

    def stop(self):
        self.logger.debug('Signaling stitch worker to stop')
        self.isDone = True

    # Process monitor update
    def process(self, pvObject):
        t0 = time.time()
        (frameId,image,nx,ny,nz,colorMode,fieldKey) = self.reshapeNtNdArray(pvObject)
        if nx is None:
            self.logger.debug('Frame id %s contains an empty image.', frameId)
            return pvObject

        if not self.nx and not self.ny:
            self.logger.debug('Stitched image dimensions have not been configured, frame id %s was not modified', frameId)
            return pvObject

        if nz:
            self.logger.error('Frame id %s contains color image, skipping it.', frameId)
            return pvObject

        # Assume tile has (x,y) indices that are
        # added by the split processor; put tile in a list
        # so that we can easily get to it
        # 
        tileIndexX = pvObject['tileIndexX']
        tileIndexY = pvObject['tileIndexY']
        if not self.nxTiles:
            self.nxTiles = self.nx//nx
            self.nyTiles = self.ny//ny
            self.nxyTiles = self.nxTiles*self.nyTiles

        # Arrange tiles into a 2D matrix
        tiles = self.tileCache.get(frameId, [[None]*self.nxTiles,[None]*self.nyTiles])
        tiles[tileIndexX][tileIndexY] = image
        self.tileCache[frameId] = tiles
        nFrameTilesReceived = self.frameTileCounter.get(frameId, 0)
        nFrameTilesReceived += 1
        self.nTilesReceived += 1
        self.frameTileCounter[frameId] = nFrameTilesReceived 

        self.logger.debug('Received (%sx%s) tile id %s with coordinates (%s,%s); total received so far %s out of %s', nx, ny, frameId, tileIndexX, tileIndexY, nFrameTilesReceived, self.nxyTiles)

        if nFrameTilesReceived == self.nxyTiles:
            self.logger.debug('Frame id %s is ready for stitching', frameId)
            self.stitchQueue.put(frameId)
        t1 = time.time()
        self.totalTileProcessingTime += t1-t0
        return pvObject

    # Reset statistics for user processor
    def resetStats(self):
        self.nFramesStitched = 0
        self.nFramesMissed = 0
        self.nTilesProcessed = 0
        self.nTilesReceived = 0
        self.nTilesDiscarded = 0
        self.totalTileProcessingTime = 0
        self.totalFrameStitchingTime = 0
        self.lastFrameId = None

    # Retrieve statistics for user processor
    def getStats(self):
        tileProcessingRate = 0
        tileProcessingTime = 0
        frameStitchingRate = 0
        frameStitchingTime = 0
        if self.nTilesReceived > 0:
            tileProcessingTime = self.totalTileProcessingTime/self.nTilesReceived
            tileProcessingRate = self.nTilesReceived/self.totalTileProcessingTime
        if self.nFramesStitched > 0:
            frameStitchingTime = self.totalFrameStitchingTime/self.nFramesStitched
            frameStitchingRate = self.nFramesStitched/self.totalFrameStitchingTime
        return {
            'nFramesStitched' : self.nFramesStitched,
            'nFramesMissed' : self.nFramesMissed,
            'nTilesProcessed' : self.nTilesProcessed,
            'nTilesReceived' : self.nTilesReceived,
            'nTilesDiscarded' : self.nTilesDiscarded,
            'tileProcessingTime' : FloatWithUnits(tileProcessingTime, 's'),
            'tileProcessingRate' : FloatWithUnits(tileProcessingRate, 'tps'),
            'frameStitchingTime' : FloatWithUnits(frameStitchingTime, 's'),
            'frameStitchingRate' : FloatWithUnits(frameStitchingRate, 'fps')
        }

    # Define PVA types for different stats variables
    def getStatsPvaTypes(self):
        return {
            'nFramesStitched' : pva.UINT,
            'nFramesMissed' : pva.UINT,
            'nTilesProcessed' : pva.UINT,
            'nTilesReceived' : pva.UINT,
            'nTilesDiscarded' : pva.UINT,
            'tileProcessingTime' : pva.DOUBLE,
            'tileProcessingRate' : pva.DOUBLE,
            'frameStitchingTime' : pva.DOUBLE,
            'frameStitchingRate' : pva.DOUBLE
        }

    # Define output PVA structure
    def getOutputPvObjectType(self, pvObject):
        return pva.NtNdArray()
