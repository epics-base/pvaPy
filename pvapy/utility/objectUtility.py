#!/usr/bin/env python

import pvaccess as pva

class ObjectUtility:

    @classmethod
    def createObjectInstance(cls, module, className, args=None):
        c = module.__dict__.get(className)
        o = None
        if c:
            if args:
                o = c(args)
            else:
                o = c()
        if not o:
            raise pva.InvalidArgument(f'Could not create object instance of class {className} from module {module}')
        return o 

    @classmethod
    def createObjectInstanceFromFile(cls, filePath, moduleName, className, args=None):
        import importlib.util
        spec = importlib.util.spec_from_file_location(moduleName, filePath)
        m = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(m)
        return cls.createObjectInstance(m, className, args)

    @classmethod
    def createObjectInstanceFromClassPath(cls, classPath, args=None):
        import importlib
        moduleName = '.'.join(classPath.split('.')[:-1])
        className = classPath.split('.')[-1]
        m = importlib.import_module(moduleName)
        return cls.createObjectInstance(m, className, args)

