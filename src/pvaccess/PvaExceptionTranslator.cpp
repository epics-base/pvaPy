// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvaExceptionTranslator.h"
#include "boost/python.hpp"

std::map<std::string,PyObject*> PvaExceptionTranslator::exceptionClassMap;

PyObject* PvaExceptionTranslator::createExceptionClass(const char* name, PyObject* baseClass)
{
    std::map<std::string,PyObject*>::iterator iterator = exceptionClassMap.find(name);
    if (iterator != exceptionClassMap.end()) {
        return iterator->second;
    }

    std::string scopeName = boost::python::extract<std::string>(boost::python::scope().attr("__name__"));
    std::string qualifiedName0 = scopeName + "." + name;
    char* qualifiedName1 = const_cast<char*>(qualifiedName0.c_str());

    PyObject* excClass = PyErr_NewException(qualifiedName1, baseClass, 0);
    if(!excClass) {
        boost::python::throw_error_already_set();
    }
    boost::python::scope().attr(name) = boost::python::handle<>(boost::python::borrowed(excClass));
    exceptionClassMap[name] = excClass;
    return excClass;
}

void PvaExceptionTranslator::translator(const PvaException& ex)
{
    const char* pyExceptionClassName = ex.getPyExceptionClassName();
    std::map<std::string,PyObject*>::iterator iterator = exceptionClassMap.find(pyExceptionClassName);
    PyObject* exceptionClass = PyExc_UserWarning;
    if (iterator != exceptionClassMap.end()) {
        exceptionClass = iterator->second;
    }

    PyErr_SetString(exceptionClass, ex.what());
}

