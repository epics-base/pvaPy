#!/usr/bin/env python

# Run this by pointing to the EPICS_BASE dbd directory, e.g.
#EPICS_DB_INCLUDE_PATH=/path/to/epics-7.0.6.1/dbd

import time
import pvaccess as pva
print('Creating IOC')
ioc = pva.CaIoc()
print('Loading DB')
ioc.loadDatabase('base.dbd', '', '')
print('Registering device record driver')
ioc.registerRecordDeviceDriver()
print('Loading records')
ioc.loadRecords('/home/sveseli/PVAPY/dev/test/int.db','NAME=I1')
print('Starting IOC')
ioc.start()
print('IOC is up and running')
print('Available records:')
ioc.dbl('', '')
for i in range(0,60):
    ioc.putField('I1', str(i))
    time.sleep(1)

time.sleep(60)

