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
    print('Set PV: %s' % pv)

if __name__ == '__main__':
    c = Channel('testInt')
    t = MyThread(10)
    t.start()
    time.sleep(1)
    c.asyncPut(PvInt(42), echo)
    print('Started Async Put')
    time.sleep(5)
    
