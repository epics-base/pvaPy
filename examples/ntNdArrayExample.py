#!/usr/bin/env python

import time
from datetime import datetime as dt
import numpy as np
import pvaccess as pva

NANOSECONDS_IN_SECOND = 1000000000

def getTimeStamp():
    tStamp = np.datetime64(dt.now(), 'ns')
    t = (tStamp-np.datetime64(0,'ns'))/np.timedelta64(1, 's')
    s = int(t)
    ns = int((t - s)*NANOSECONDS_IN_SECOND)
    return pva.PvTimeStamp(s,ns,0)

def createImage(id, nx, ny, colorMode, extraFieldsPvObject=None):
    timeStamp = getTimeStamp()  
    if extraFieldsPvObject:
        nda = pva.NtNdArray(extraFieldsPvObject.getStructureDict())
    else:
        nda = pva.NtNdArray()
    nda['uniqueId'] = id
    dims = [pva.PvDimension(nx, 0, nx, 1, False), pva.PvDimension(ny, 0, ny, 1, False)]
    nda['codec'] = pva.PvCodec('pvapyc', pva.PvInt(14))
    nda['dimension'] = dims
    nda['descriptor'] = 'PvaPy Simulated Image'
    nda['compressedSize'] = nx*ny
    nda['uncompressedSize'] = nx*ny
    nda['timeStamp'] = timeStamp
    nda['dataTimeStamp'] = timeStamp
    attrs = [pva.NtAttribute('ColorMode', pva.PvInt(0))]
    nda['attribute'] = attrs
    nda['value'] = {'ubyteValue' : np.random.randint(0,256, size=nx*ny, dtype=np.uint8)}
    nda.set(extraFieldsPvObject)
    return nda

def main():
    server = pva.PvaServer()
    server.start()
    N_IMAGES = 100
    NX = 1024
    NY = 1024
    COLOR_MODE = 0
    IMAGE_RATE = 1.0 # Hz
    EXTRA_FIELDS_OBJECT = pva.PvObject({'customField1' : pva.INT, 'customField2' : pva.STRING}, {'customField1' : 10, 'customField2' : 'GeneratedBy: PvaPy'})
    CHANNEL = 'pvapy:image'
    for i in range(0,N_IMAGES):
        print('Image id: {}'.format(i))
        image = createImage(i, NX, NY, COLOR_MODE, EXTRA_FIELDS_OBJECT)
        time.sleep(1/IMAGE_RATE)
        if i == 0:
            server.addRecord(CHANNEL, image)
        else:
            server.update(CHANNEL, image)


if __name__ == '__main__':
    main()


