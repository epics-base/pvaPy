// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PVA_EXCEPTION_TRANSLATOR_H
#define PVA_EXCEPTION_TRANSLATOR_H

#include <map>
#include "PvaException.h"
#include "boost/python/object.hpp"

class PvaExceptionTranslator 
{
public:
    static PyObject* createExceptionClass(const char* name, PyObject* baseClass=PyExc_Exception);
    static void translator(const PvaException& ex);
private:
    static std::map<std::string,PyObject*> exceptionClassMap;
};

#endif

