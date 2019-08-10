#!/usr/bin/env python

import time

from pvaccess import *
name = raw_input("enter channel channel: ")
chan = Channel(name)

def gotMonitor(x):
    print(x.toJSON(False))

chan.subscribe('gotMonitor', gotMonitor)
chan.startMonitor("value,timeStamp")
time.sleep(100)
chan.unsubscribe('gotMonitor')

    

