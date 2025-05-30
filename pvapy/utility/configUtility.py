#!/usr/bin/env python

class ConfigUtility:

    @classmethod
    def configureObject(cls, obj, configDict, setOnlyExistingKeys=False):
        if not obj or not configDict or not isinstance(configDict, dict):
            return obj
        for key,value in configDict.items():
            key = str(key)
            if setOnlyExistingKeys:
                if hasattr(obj, key):
                    setattr(obj, key, value)
            else:
                setattr(obj, key, value)
        return obj
