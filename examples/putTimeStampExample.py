# This example works with PV Database from exampleCPP
from pvaccess import Channel, PvTimeStamp
c = Channel('PVRdouble')
t = PvTimeStamp(10, 100,1)
print('start ts=',c.get('timeStamp'))
c.put(t,'record[process=false]field(timeStamp)')
val = c.get('value,alarm,timeStamp')
print('after put ts=',c.get('timeStamp'))

