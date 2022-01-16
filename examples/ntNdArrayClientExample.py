#!/usr/bin/env python

import time
import numpy as np
import pvaccess as pva

def monitor(pv):
    print('Got image: %d' % pv['uniqueId'])

def main():
    CHANNEL = 'pvapy:image'
    c = pva.Channel(CHANNEL)
    c.monitor(monitor)
    time.sleep(100)


if __name__ == '__main__':
    main()


