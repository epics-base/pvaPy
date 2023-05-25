#!/usr/bin/env python

import pvaccess as pva

class StatsUtility:

    @classmethod
    def addKeyValues(cls, dict1, dict2, keys=[]):
        if not keys:
            keys = list(set(list(dict1.keys())+list(dict2.keys())))
        dict12 = {}
        for key in keys:
            dict12[key] = dict1.get(key,0) + dict2.get(key,0)
        return dict12

    @classmethod
    def minKeyValues(cls, dict1, dict2, keys=[]):
        if not keys:
            keys = list(set(list(dict1.keys())+list(dict2.keys())))
        dict12 = {}
        for key in keys:
            dict12[key] = min(dict1.get(key,float('inf')),dict2.get(key,float('inf')))
        return dict12

    @classmethod
    def maxKeyValues(cls, dict1, dict2, keys=[]):
        if not keys:
            keys = list(set(list(dict1.keys())+list(dict2.keys())))
        dict12 = {}
        for key in keys:
            dict12[key] = max(dict1.get(key,float('-inf')),dict2.get(key,float('-inf')))
        return dict12

