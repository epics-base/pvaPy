#!/usr/bin/env python

from pvaccess import MultiChannel
from pvaccess import PvInt
from pvaccess import PvDouble
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

    #
    # MultiChannel Put
    #

    def testPut_IntDouble(self):
        iv = TestUtility.getRandomInt()
        ic = TestUtility.getIntChannel()

        dv = TestUtility.getRandomDouble()
        dc = TestUtility.getDoubleChannel()

        mc = MultiChannel([ic.getName(),dc.getName()])
        mc.put([PvInt(iv),PvDouble(dv)])
        pv = mc.get()
        iv2 = pv['value'][0][0]['value']
        assert(iv2 == iv)
        dv2 = pv['value'][1][0]['value']
        TestUtility.assertDoubleEquality(dv,dv2)

