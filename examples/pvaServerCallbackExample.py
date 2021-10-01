#!/usr/bin/env python

import time
from pvaccess import PvObject, PvaServer, INT, Channel

def echo(x):
    print('Got value: %s' % x)

data = PvObject({'value': INT}, {'value': 1})
data2 = PvObject({'value': INT}, {'value': 2})
print("Starting Server")
server = PvaServer('foo', data, echo)
channel = Channel('foo')
print("Attempting get")
print(channel.get())
print("Attempting put")
channel.put(data2)

print("Put done")
print(channel.get())

print("Stopping server")
server.stop()
time.sleep(3)

print("Starting server")
server.start()
time.sleep(3)

print("Attempting get")
print(channel.get())

