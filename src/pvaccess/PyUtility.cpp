// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include <sstream>
#include "PyUtility.h"
#include "InvalidArgument.h"

namespace bp = boost::python;

namespace PyUtility
{

std::string extractStringFromPyObject(const bp::object& pyObject)
{
    try {
        return bp::extract<std::string>(bp::str(pyObject))();
    }
    catch(const bp::error_already_set&) {
        PyErr_Print();
        PyErr_Clear();
    }
    throw InvalidArgument("Unable to convert object to string.");
}

bool isPyNone(const bp::object& pyObject)
{
    // RHEL 6 version of boost does not have is_none()
    //if (pyObject.is_none()) {
    if (pyObject.ptr() == bp::object().ptr()) {
        return true;
    }
    return false;
}

bool isPyList(const bp::object& pyObject)
{
    bp::extract<bp::list> extractListValue(pyObject);
    return extractListValue.check();
}

#if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1
bool isNumPyNDArray(const bp::object& pyObject)
{
    bp::extract<numpy_::ndarray> extractArrayValue(pyObject);
    return extractArrayValue.check();
}
#endif // if defined HAVE_NUMPY_SUPPORT && HAVE_NUMPY_SUPPORT == 1

std::string extractStringFromPyList(const bp::object& pyObject)
{
    std::string result = "";
    bp::extract<bp::list> extractListValue(pyObject);
    if(!extractListValue.check()) {
        return result;
    }
    bp::list pyList = extractListValue();
    int listSize = bp::len(pyList);
    for (int i = 0; i < listSize; i++) {
        bp::extract<int> charExtract(pyList[i]);
        if(charExtract.check()) {
            result.push_back(char(charExtract()));
        }
    }
    return result;
}

std::string getErrorMessageFromTraceback(boost::python::error_already_set& e)
{
    PyObject *extype, *value, *traceback;
    PyErr_Fetch(&extype, &value, &traceback);
    if (!extype) {
        return "";
    }

    bp::object o_extype(bp::handle<>(bp::borrowed(extype)));
    bp::object o_value(bp::handle<>(bp::borrowed(value)));
    bp::object o_traceback(bp::handle<>(bp::borrowed(traceback)));

    bp::object mod_traceback = bp::import("traceback");
    bp::object lines = mod_traceback.attr("format_exception")(
        o_extype, o_value, o_traceback);

    std::stringstream ss;
    for (int i = 0; i < len(lines); ++i) {
        ss << bp::extract<std::string>(lines[i])();
    }

    // PyErr_Fetch clears the error state, uncomment
    // the following line to restore the error state:
    // PyErr_Restore(extype, value, traceback);

    // release the GIL
    return ss.str();
}

} // namespace PyUtility


