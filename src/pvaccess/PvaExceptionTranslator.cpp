#include "PvaExceptionTranslator.h"
#include "boost/python.hpp"

void PvaExceptionTranslator::translator(const PvaException& ex)
{
    PyErr_SetString(PyExc_UserWarning, ex.what());
}

