#!/usr/bin/env python

class ObjectUtility:

    @classmethod
    def createObjectInstanceFromFile(cls, filePath, moduleName, className, args=None):
        import importlib.util
        spec = importlib.util.spec_from_file_location(moduleName, filePath)
        m = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(m)
        c = m.__dict__.get(className)
        o = None
        if c:
            if args:
                o = c(args)
            else:
                o = c()
        return o 
