#!/usr/bin/env python

import time
from pvaccess import *

c = Channel('float03')
def echo(x):
    print 'Got value: ', x
c.subscribe('echo', echo)

time.sleep(10)
c.unsubscribe('echo')
time.sleep(5)
