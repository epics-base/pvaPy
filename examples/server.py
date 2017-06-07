#!/usr/bin/env python

# 
# This example can be used to demonstrate pvaPy server/client channel 
# monitoring
# 
# Run server.py in one window, and client.py in another one.
#

import time
from pvaccess import ULONG
from pvaccess import PvObject
from pvaccess import PvaServer

if __name__ == '__main__':
    pv = PvObject({'c' : ULONG})
    server = PvaServer('counter', pv)
    c = 0
    startTime = time.time()
    while True:
        c += 1
        pv = PvObject({'c' : ULONG}, {'c' : c})
        server.update(pv)
        if c % 100000 == 0:
            currentTime = time.time()
            runtime = currentTime - startTime
            updateRateKHz = c/runtime/1000.0
            print('Runtime: %.2f; Counter: %9d, Rate: %.2f [kHz]' % (runtime, c, updateRateKHz))
        
        
