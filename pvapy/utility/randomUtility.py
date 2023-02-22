#!/usr/bin/env python

'''
This module contains random value utilities.
'''

import random
import string

class RandomUtility:
    '''
    Class that provides commonly used random value utility methods.

    Usage example:
    ::\n\n
    \ts = RandomUtility.getRandomString(nChars=16)
    \n\n
    '''

    @classmethod
    def getRandomString(cls, nChars=256):
        '''
        Get random string consisting of letters and numbers.

        :Parameter: *nChars* (int) - requested string length
        :Returns: random string value
        '''
        size = int(random.uniform(1,nChars))
        allowedValues = string.ascii_uppercase + string.ascii_lowercase + string.digits
        return ''.join(random.choice(allowedValues) for _ in range(size))

    @classmethod
    def getRandomInt(cls, minValue=-2147483648, maxValue=2147483648):
        '''
        Get random integer from a given range.

        :Parameter: *minValue* (int) - minimum value
        :Parameter: *maxValue* (int) - maximum value
        :Returns: random integer value
        '''
        return int(random.uniform(minValue,maxValue))
