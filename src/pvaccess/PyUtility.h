// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PY_UTILITY_H
#define PY_UTILITY_H

#include <string>
#include "boost/python/extract.hpp"
#include "boost/python/object.hpp"
#include "boost/python/list.hpp"
#include "boost/python/dict.hpp"

#include "StringUtility.h"
#include "InvalidArgument.h"
#include "FieldNotFound.h"

namespace PyUtility
{

std::string extractStringFromPyObject(const boost::python::object& pyObject);

template<typename PyType>
PyType extractValueFromPyObject(const boost::python::object& pyObject)
{
    boost::python::extract<PyType> extractValue(pyObject);
    if (extractValue.check()) {
        return extractValue();
    }
    std::string objectString = extractStringFromPyObject(pyObject);
    throw InvalidArgument("Invalid data type for '" + objectString + "'");
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

} // namespace PyUtility

#endif 

