#ifndef PY_UTILITY_H
#define PY_UTILITY_H

#include <string>
#include "boost/python/extract.hpp"
#include "boost/python/object.hpp"
#include "boost/python/list.hpp"
#include "boost/python/dict.hpp"

#include "StringUtility.h"
#include "InvalidDataType.h"
#include "FieldNotFound.h"

namespace PyUtility
{

template<typename PyType>
PyType extractValueFromPyObject(const boost::python::object& pyObject)
{
    boost::python::extract<PyType> extractValue(pyObject);
    if (extractValue.check()) {
        return extractValue();
    }
    std::string objectString = boost::python::extract<std::string>(pyObject);
    throw InvalidDataType("Invalid data type for object " + objectString);
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

std::string extractStringFromPyObject(const boost::python::object& pyObject);

} // namespace PyUtility

#endif 

