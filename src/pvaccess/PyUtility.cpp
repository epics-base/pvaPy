#include "PyUtility.h"

namespace PyUtility
{

std::string extractStringFromPyObject(const boost::python::object& pyObject)
{
    // Simply try to extract various data types until one works.
    // There has to be a better way for doing this.
    boost::python::extract<std::string> extractStringValue(pyObject);
    if (extractStringValue.check()) {
        return extractStringValue();
    }

    boost::python::extract<double> extractDoubleValue(pyObject);
    if (extractDoubleValue.check()) {
        return StringUtility::toString<double>(extractDoubleValue());
    }

    boost::python::extract<float> extractFloatValue(pyObject);
    if (extractFloatValue.check()) {
        return StringUtility::toString<float>(extractFloatValue());
    }

    boost::python::extract<unsigned long long> extractULongLongValue(pyObject);
    if (extractULongLongValue.check()) {
        return StringUtility::toString<unsigned long long>(extractULongLongValue());
    }

    boost::python::extract<long long> extractLongLongValue(pyObject);
    if (extractLongLongValue.check()) {
        return StringUtility::toString<long long>(extractLongLongValue());
    }

    boost::python::extract<unsigned int> extractUIntValue(pyObject);
    if (extractUIntValue.check()) {
        return StringUtility::toString<unsigned int>(extractUIntValue());
    }

    boost::python::extract<int> extractIntValue(pyObject);
    if (extractIntValue.check()) {
        return StringUtility::toString<int>(extractIntValue());
    }

    boost::python::extract<unsigned short> extractUShortValue(pyObject);
    if (extractUShortValue.check()) {
        return StringUtility::toString<unsigned short>(extractUShortValue());
    }

    boost::python::extract<short> extractShortValue(pyObject);
    if (extractShortValue.check()) {
        return StringUtility::toString<short>(extractShortValue());
    }

    boost::python::extract<unsigned char> extractUCharValue(pyObject);
    if (extractUCharValue.check()) {
        return StringUtility::toString<unsigned char>(extractUCharValue());
    }

    boost::python::extract<char> extractCharValue(pyObject);
    if (extractCharValue.check()) {
        return StringUtility::toString<char>(extractCharValue());
    }

    boost::python::extract<bool> extractBoolValue(pyObject);
    if (extractBoolValue.check()) {
        return StringUtility::toString<bool>(extractBoolValue());
    }

    throw InvalidDataType("Unable to convert object to string.");
}


} // namespace PyUtility


