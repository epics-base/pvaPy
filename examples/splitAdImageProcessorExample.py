
import time
import pvaccess as pva
from pvapy.hpc.adImageProcessor import AdImageProcessor
from pvapy.utility.adImageUtility import AdImageUtility
from pvapy.utility.floatWithUnits import FloatWithUnits

# Split AD Image Processor for the streaming framework
# Splits original image into (nx,ny) tiles
class SplitAdImageProcessor(AdImageProcessor):

    def __init__(self, configDict):
        AdImageProcessor.__init__(self, configDict)
        self.nProcessed = 0
        self.processingTime = 0
        self.logger.debug('Created SplitAdImageProcessor')
        self.configure(configDict)

    # Configure user processor
    def configure(self, configDict):
        self.logger.debug('Configuration update: %s', configDict)
        self.nx = int(configDict.get('nx', 0))
        self.ny = int(configDict.get('ny', 0))
        if self.nx and self.ny:
            self.logger.debug('Image will be split into tiles with %s rows, %s columns', self.ny, self.nx)

    # Process monitor update
    def process(self, pvObject):
        t0 = time.time()
        (frameId,image,nx,ny,nz,colorMode,fieldKey) = self.reshapeNtNdArray(pvObject)
        if nx is None:
            self.logger.debug('Frame id %s contains an empty image.', frameId)
            return pvObject
        if not self.nx and not self.ny:
            self.logger.debug('Tile dimensions have not been configured, frame id %s was not modified', frameId)
            self.updateOutputChannel(pvObject)
        if not nz:
            nxTiles = nx//self.nx
            nyTiles = ny//self.ny
            self.logger.debug('Frame id %s (%sx%s) will be reshaped into %s (%sx%s) tiles', frameId, nx, ny, nxTiles*nyTiles, self.nx, self.ny)
            tiles = image.reshape(nyTiles, self.ny, nxTiles, self.nx)
            tiles = tiles.swapaxes(1, 2)
            for i in range (0,tiles.shape[0]):
                for j in range (0,tiles.shape[1]):
                    self.logger.debug('Generating tile %s,%s', i, j)
                    tile = tiles[i,j]
                    # Add tile coordinates as a extra fields.
                    tileCoordinates = pva.PvObject({'tileIndexX' : pva.INT, 'tileIndexY' : pva.INT}, {'tileIndexX' : i, 'tileIndexY' : j})
                    tilePvObject = AdImageUtility.generateNtNdArray2D(frameId, tile, extraFieldsPvObject=tileCoordinates)

                    # Alternative approach for keeping track of tile
                    # coordinates involves adding new attributes to the
                    # output image, as shown below.
                    # Note that in this case the getOutputPvObjectType() method
                    # does not need to be implemented.

                    #tilePvObject = self.generateNtNdArray2D(frameId, tile)
                    #attrs = tilePvObject['attribute']
                    #attrs.append(pva.NtAttribute('tileIndexX', pva.PvInt(i)))
                    #attrs.append(pva.NtAttribute('tileIndexY', pva.PvInt(j)))
                    #tilePvObject['attribute'] = attrs

                    self.updateOutputChannel(tilePvObject)
        else:
            self.logger.error('Frame id %s contains color image, skipping it.', frameId)
        t1 = time.time()
        self.nProcessed += 1
        self.processingTime += (t1-t0)
        return pvObject

    # Reset statistics for user processor
    def resetStats(self):
        self.nProcessed = 0
        self.processingTime = 0

    # Retrieve statistics for user processor
    def getStats(self):
        processingRate = 0
        if self.nProcessed > 0:
            processingRate = self.nProcessed/self.processingTime
        return {
            'nProcessed' : self.nProcessed,
            'processingTime' : FloatWithUnits(self.processingTime, 's'),
            'processingRate' : FloatWithUnits(processingRate, 'fps')
        }

    # Define PVA types for different stats variables
    def getStatsPvaTypes(self):
        return {
            'nProcessed' : pva.UINT,
            'processingTime' : pva.DOUBLE,
            'processingRate' : pva.DOUBLE
        }

    # Define output PVA structure
    def getOutputPvObjectType(self, pvObject):
        # This method does not need to be implemented if
        # NtAttributes are used for keeping track of tile coordinates.
        tileFieldsDict = {'tileIndexX' : pva.INT, 'tileIndexY' : pva.INT}
        return pva.NtNdArray(tileFieldsDict)
