#!/usr/bin/env python

import time
from threading import Thread
from pvaccess import *

class MyThread(Thread):
    def __init__(self, n):
        Thread.__init__(self)
        self.n = n

    def run(self):
        while self.n > 0:
            print('Time is %.2f' % time.time())
            time.sleep(0.5)
            self.n-=1

def echo(pv):
    print('Got PV: %s' % pv)
    time.sleep(1)

def error(ex):
    print('Got error: %s' % ex)

if __name__ == '__main__':
    c = Channel('testInt')
    t = MyThread(10)
    t.start()
    time.sleep(1)
    for i in range(0,12):
        try:
            c.asyncGet(echo, error)
            print('Started Async Get #%d' % i)
        except Exception as ex:
            print('Error for request #%d: %s' % (i,ex))
    print('Waiting for callbacks...')
    time.sleep(30)
    
