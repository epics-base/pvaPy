// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python.hpp"
#include "PyUtility.h"
#include "InvalidArgument.h"

namespace PyUtility
{

std::string extractStringFromPyObject(const boost::python::object& pyObject)
{
    try {
        return boost::python::extract<std::string>(boost::python::str(pyObject))();
    }
    catch(const boost::python::error_already_set&) {
        PyErr_Print();
        PyErr_Clear();
    }
    throw InvalidArgument("Unable to convert object to string.");
}

bool isPyNone(const boost::python::object& pyObject)
{
    // RHEL 6 version of boost does not have is_none()
    //if (pyObject.is_none()) {
    if (pyObject.ptr() == boost::python::object().ptr()) {
        return true;
    }
    return false;
}

bool isPyList(const boost::python::object& pyObject)
{
    boost::python::extract<boost::python::list> extractListValue(pyObject);
    return extractListValue.check();
}

#if defined HAVE_NUM_PY_SUPPORT && HAVE_NUM_PY_SUPPORT == 1
bool isNumPyNDArray(const boost::python::object& pyObject)
{
    boost::python::extract<numpy_::ndarray> extractArrayValue(pyObject);
    return extractArrayValue.check();
}
#endif // if defined HAVE_NUM_PY_SUPPORT && HAVE_NUM_PY_SUPPORT == 1

std::string extractStringFromPyList(const boost::python::object& pyObject)
{
    std::string result = "";
    boost::python::extract<boost::python::list> extractListValue(pyObject);
    if(!extractListValue.check()) {
        return result;
    }
    boost::python::list pyList = extractListValue();
    int listSize = boost::python::len(pyList);
    for (int i = 0; i < listSize; i++) {
        boost::python::extract<int> charExtract(pyList[i]);
        if(charExtract.check()) {
            result.push_back(char(charExtract()));
        }
    }
    return result;
}


} // namespace PyUtility


