#!/usr/bin/env python

import time
from pvaccess import INT, PvaServer, PvObject 

pv = PvObject({'x': INT, 'y' : INT})
pvaServer = PvaServer('pair', pv)

print('\nStart monitoring the "pair" channel in a different terminal, using "pvget -m pair" command...\n')
time.sleep(5)
pv['x'] = 100
print('Updated pair to (100,0)') 
time.sleep(5)
pv['y'] = 200
print('Updated pair to (100,200)') 
time.sleep(5)

print('\nStarting updates to (x,2x) for x in range (0,10)\n') 
for i in range (0,10):
    x = i
    y = 2*i
    pv2 = PvObject({'x': INT, 'y' : INT}, {'x' : x, 'y' : y})
    pvaServer.update(pv2)
    print('Updated pair to (%s,%s)' % (x,y))
    time.sleep(1)

