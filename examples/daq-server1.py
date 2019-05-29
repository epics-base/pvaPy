#!/usr/bin/env python

# This server example produces a collection of waveforms and exposes it over 
# "server1_data" channel. It can be used for measuring PVA server performance.

# This example can be used together with daq-server2, which monitors
# "server1_data" channel, and exposes the same data on the "server2_data" 
# channel.

# Modified from the original example code by Tom Fors to allow
# measuring performance, array and data rates.

from math import pi, sin, asin
from time import sleep
import time as unixTime
import numpy as np

import pvaccess as pva
ARRAY_SIZE = 10*1024
N_FLAT_ARRAYS = 100
dataStruct = {
    'ArrayId': pva.UINT, 
    'Time': [pva.DOUBLE],
    'Sinusoid': [pva.FLOAT], 
    'Triangle': [pva.FLOAT],
}
for i in range (0, N_FLAT_ARRAYS):
    arrayName = 'Flat%03d' % (i+1)
    dataStruct[arrayName] = [pva.FLOAT] 

srv = pva.PvaServer('server1_data', pva.PvObject(dataStruct))
t0 = 0.0
n = 0
dt = 1./1000
startTime = unixTime.time()
# Time is double (8 bytes), other arrays are floats (4 bytes each)
dataSize = ARRAY_SIZE*(8+N_FLAT_ARRAYS*4) 
dataSizeMB = dataSize/(1024*1024)

while True:
    time = [t0+dt*i for i in range(0, ARRAY_SIZE)]
    # Disable sinusoid/triangle for performance measurements
    #sinusoid = [sin(2*pi*1.1*t + pi/2) for t in time]
    sinusoid = []
    #triangle = [(2/pi)*asin(sin(2*pi*1.1*t)) for t in time]
    triangle = []
    valueStruct = {'ArrayId': n, 'Time': np.array(time, dtype=np.double), 'Sinusoid': sinusoid, 'Triangle': triangle}
    for i in range (0, N_FLAT_ARRAYS):
        arrayName = 'Flat%03d' % (i+1)
        f = float(i) + (n % ARRAY_SIZE)
        array = np.full((ARRAY_SIZE), f, dtype=np.float32)
        #array = np.empty(ARRAY_SIZE, dtype=np.float32)
        #array.fill(f)
        #valueStruct[arrayName] = [f]*ARRAY_SIZE
        valueStruct[arrayName] = array
    pv = pva.PvObject(dataStruct, valueStruct)
    srv.update(pv)
    t0 = time[-1] + dt
    n += 1
    if n % 1000 == 0:
        deltaT = unixTime.time()- startTime
        arrayRate = n / deltaT
        dataRate = dataSize*arrayRate
        dataRateMB = dataRate/(1024*1024)
        print('Runtime: %.3f [s], Generated Arrays: %s, Array Size: %s [B], %s [MB], Array Rate: %3.f [Hz], Data Rate: %.3f [B/s], %.3f [MB/s]' % (deltaT, n, dataSize, dataSizeMB, arrayRate, dataRate, dataRateMB))

