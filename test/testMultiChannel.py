#!/usr/bin/env python

from pvaccess import MultiChannel
from testUtility import TestUtility

class TestMultiChannel:

    #
    # MultiChannel Get
    #

    def testGet_IntDouble(self):
        iv = TestUtility.getRandomInt()
        ic = TestUtility.getIntChannel()
        ic.put(iv)

        dv = TestUtility.getRandomDouble()
        dc = TestUtility.getDoubleChannel()
        dc.put(dv)

        mc = MultiChannel([ic.getName(),dc.getName()])
        pv = mc.get()
        iv2 = pv['value'][0][0]['value']
        assert(iv2 == iv)
        dv2 = pv['value'][1][0]['value']
        TestUtility.assertDoubleEquality(dv,dv2)

