#!/usr/bin/env python

import pvaccess as pva

class TimeUtility:

    NANOSECONDS_IN_SECOND = 1000000000.0

    @classmethod
    def getTimeStampAsFloat(cls, pvObject, fieldName=''):
        if not fieldName:
            ts = pvObject
        elif fieldName not in pvObject:
            raise pva.InvalidArgument(f'Input object does not have field {fieldName}')
        else:
            ts = pvObject[fieldName]
        if 'secondsPastEpoch' not in ts:
            raise pva.InvalidArgument(f'Input object does not contain secondsPastEpoch field')
        s = ts['secondsPastEpoch']
        if 'nanoseconds' not in ts:
            raise pva.InvalidArgument(f'Input object does not contain nanoseconds field')
        ns = ts['nanoseconds']
        return s + ns/cls.NANOSECONDS_IN_SECOND 

