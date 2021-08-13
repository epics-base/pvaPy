#!/usr/bin/env python

# Example AS definition file:
#
# $ cat /local/sveseli/EPICS/acl.conf
# UAG(uag) {user1,user2}
# HAG(hag) {host1,host2}
# ASG(DEFAULT) {
#     RULE(0,READ)
#     RULE(0,WRITE)
#     RULE(1,READ)
#     RULE(1,WRITE) {
#         UAG(uag)
#         HAG(hag)
#     }
# }

# Example client call to test the server:
#
# python -c "from pvaccess import *; c = Channel('x'); print(c.get()); c.put(1); print(c.get())"
#

from pvaccess import *
import time

print('Serving channel x')
s = PvaServer()
s.initAs('/local/sveseli/EPICS/acl.conf', '')
pv = PvObject({'x' : INT}, {'x':0})
s.addRecordWithAs('x', pv, 1, 'DEFAULT')

time.sleep(120)

