// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PY_UTILITY_H
#define PY_UTILITY_H

#include <string>
#include <boost/python.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/object.hpp>
#include <boost/python/list.hpp>
#include <boost/python/dict.hpp>

#include "StringUtility.h"
#include "InvalidArgument.h"
#include "InvalidDataType.h"
#include "FieldNotFound.h"

#include "pvapy.environment.h"

#if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1
#include NUMPY_HEADER_FILE
#endif // if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1

namespace PyUtility
{

std::string extractStringFromPyObject(const boost::python::object& pyObject);
bool isPyNone(const boost::python::object& pyObject);
bool isPyList(const boost::python::object& pyObject);

#if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1
bool isNumPyNDArray(const boost::python::object& pyObject);
#endif // if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1

std::string extractStringFromPyList(const boost::python::object& pyObject);

template<typename PyType>
PyType extractValueFromPyObject(const boost::python::object& pyObject)
{
    boost::python::extract<PyType> extractValue(pyObject);
    if (extractValue.check()) {
        return extractValue();
    }
    std::string objectString = extractStringFromPyObject(pyObject);
    throw InvalidDataType("Invalid data type for '" + objectString + "'");
}

template<typename PyType>
PyType extractKeyValueFromPyDict(const std::string& key, const boost::python::dict& pyDict)
{
    boost::python::object pyObject = pyDict[key];
    if (!pyObject.ptr()) {
        throw FieldNotFound("Dictionary does not have field " + key);
    }
    return extractValueFromPyObject<PyType>(pyObject);
}

std::string getErrorMessageFromTraceback(boost::python::error_already_set& ex);
 
} // namespace PyUtility

#endif 

