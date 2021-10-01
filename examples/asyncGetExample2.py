#!/usr/bin/env python

import time
from pvaccess import *

def clbk(pv):
    print(pv)
def err(ex):
    print('Error: %s' % ex)
c = Channel('x')
for i in range(0,20):
    try:
        c.asyncGet(clbk, err)
        print('Started Async Get #%d' % i)
    except Exception as ex:
        print('Error for request #%d: %s' % (i,ex))
time.sleep(0.01)
print("Destructor begin")
del c
print("Destructor done")
time.sleep(5)


