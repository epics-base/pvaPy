#!/usr/bin/env python

# This example can be used together with daq-server1. It monitors
# "server1_data" channel, and exposes the same data on the "server2_data" 
# channel.

import time
from pvaccess import Channel, PvaServer

class ServerUpdater:
    def __init__(self, server):
        self.server = server
        self.received = 0
        self.missed = 0
        self.prevId = None
        self.prevTime = None
        self.startTime = None

    def updateServer(self, x):
        t = time.time()
        self.server.update(x)
        self.received += 1
        id = x['ArrayId']
        if not self.prevId:
            self.startTime = t
        if self.prevId:
            missed = id - self.prevId - 1
            self.missed += missed
            dt = t - self.prevTime
            currentRate = 1.0/dt
            if self.received % 1000 ==0:
                deltaT = t - self.startTime
                averageArrayRate = self.received/deltaT
                print('Runtime: %.3f [s], Received Arrays: %s, Missed Arrays: %s, Missed Arrays Since Last: %s, Current Array Rate: %.3f [Hz], Average Array Rate: %.3f [Hz]' % (deltaT, self.received, self.missed, missed, currentRate, averageArrayRate))
        self.prevId = id
        self.prevTime = t

c = Channel('server1_data')
pv = c.get('')
s = PvaServer('server2_data', pv)
updater = ServerUpdater(s)
c.monitor(updater.updateServer, '')

time.sleep(100) 


