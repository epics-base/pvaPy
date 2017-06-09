#!/usr/bin/env python

# 
# This example can be used to demonstrate pvaPy server with multiple channels.
#

import time
from pvaccess import ULONG
from pvaccess import STRING
from pvaccess import PvObject
from pvaccess import PvaServer

if __name__ == '__main__':
    server = PvaServer()
    pv = PvObject({'c' : ULONG})
    server.addRecord('c', pv)
    pv2 = PvObject({'c2' : STRING})
    server.addRecord('c2', pv2)
    print('CHANNELS: %s' % server.getRecordNames())
    c = 0
    startTime = time.time()
    while True:
        c += 1
        pv = PvObject({'c' : ULONG}, {'c' : c})
        server.update('c', pv)
        pv2 = PvObject({'c2' : STRING}, {'c2' : str(c)})
        server.update('c2', pv2)
        if c % 100000 == 0:
            currentTime = time.time()
            runtime = currentTime - startTime
            updateRateKHz = c/runtime/1000.0
            print('Runtime: %.2f; Counter: %9d, Rate: %.2f [kHz]' % (runtime, c, updateRateKHz))
        
        
