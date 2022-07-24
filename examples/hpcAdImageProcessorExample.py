
import numpy as np
import pvaccess as pva
from pvapy.hpc.adImageProcessor import AdImageProcessor

# Example for HPC AD Image Processor
class HpcAdImageProcessor(AdImageProcessor):

    def __init__(self, configDict={}):
        AdImageProcessor.__init__(self, configDict)
        self.logger.debug('Created HpcAdImageProcessor')

    def configure(self, kwargs):
        self.logger.debug(f'Configuration update: {kwargs}')

    def process(self, pvObject):
        (frameId,image,nx,ny,nz,colorMode,fieldKey) = self.reshapeNtNdArray(pvObject)
        if nx is None:
            self.logger.debug(f'Frame id {frameId} contains an empty image.')
            return pvObject
        self.logger.debug(f'Data sum: {image.sum()} (frame id: {frameId})')
        outputFrame = pvObject
        fieldDataType = outputFrame.getStructureDict()['value'][0][fieldKey]
        image2 = np.swapaxes(image, 0, 1).flatten()
        u2 = pva.PvObject({fieldKey : fieldDataType}, {fieldKey : image2})
        outputFrame.setUnion(u2)
        self.updateOutputChannel(outputFrame)
        return outputFrame
