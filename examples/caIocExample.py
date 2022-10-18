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
ioc.loadRecords('./int.db','NAME=I1')
ioc.loadRecords('./calc.db','NAME=C1')
ioc.loadRecords('./double.db','NAME=D1')
ioc.loadRecords('./waveform.db','NAME=WF1')
print('Starting IOC')
ioc.start()
print('IOC is up and running')
print('Retrieving record names')
ioc.dbl('', '')
records = ioc.getRecordNames()
print(f'Available records: {records}')
for i in range(0,10):
    value = ioc.getField('D1')
    print(f'D1 value before put #{i}: {value}')
    ioc.putField('D1', i*1.0)
    value = ioc.getField('D1')
    print(f'D1 value after put #{i}: {value}')
    print()
    time.sleep(1)

for i in range(0,10):
    value = ioc.getField('WF1')
    print(f'WF1 value before put #{i}: {value}')
    ioc.putField('WF1', [i]*10)
    value = ioc.getField('WF1')
    print(f'WF1 value after put #{i}: {value}')
    print()
    time.sleep(1)

time.sleep(60)
