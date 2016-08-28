// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/module.hpp"
#include "boost/python/scope.hpp"
#include "boost/python/exception_translator.hpp"
#include "boost/python/docstring_options.hpp"

#include "PvaException.h"
#include "FieldNotFound.h"
#include "ObjectNotFound.h"
#include "ObjectAlreadyExists.h"
#include "InvalidArgument.h"
#include "InvalidDataType.h"
#include "InvalidState.h"
#include "InvalidRequest.h"
#include "ChannelTimeout.h"

#include "PvaExceptionTranslator.h"

// Forward wrapper declarations
//
// These functions are defined in pvaccess.<class>.cpp files, and
// are used to make code more readable and to avoid issues with
// compiler running out of memory 
void wrapPvProvider();
void wrapPvType();

void wrapPvObject();
void wrapPvScalar();
void wrapPvBoolean();
void wrapPvByte();
void wrapPvUByte();
void wrapPvShort();
void wrapPvUShort();
void wrapPvInt();
void wrapPvUInt();
void wrapPvLong();
void wrapPvULong();
void wrapPvFloat();
void wrapPvDouble();
void wrapPvString();
void wrapPvScalarArray();
void wrapPvUnion();
void wrapPvTimeStamp();
void wrapPvAlarm();

void wrapNtType();
void wrapNtTable();

void wrapChannel();
void wrapRpcServer();
void wrapRpcClient();

// Exceptions
PyObject* pvaException = NULL;
PyObject* fieldNotFoundException = NULL;
PyObject* objectNotFoundException = NULL;
PyObject* objectAlreadyExistsException = NULL;
PyObject* invalidArgumentException = NULL;
PyObject* invalidDataTypeException = NULL;
PyObject* invalidRequestException = NULL;
PyObject* invalidStateException = NULL;
PyObject* channelTimeoutException = NULL;

BOOST_PYTHON_MODULE(pvaccess)
{
    using namespace boost::python;
    scope().attr("__doc__") = "pvaccess module is a python wrapper for pvAccess and other `EPICS V4 <http://epics-pvdata.sourceforge.net>`_ C++ libraries."; 
    docstring_options local_docstring_options(true, true, false);

    //
    // Exceptions
    //
    register_exception_translator<PvaException>(PvaExceptionTranslator::translator);
    pvaException = PvaExceptionTranslator::createExceptionClass(PvaException::PyExceptionClassName);
    fieldNotFoundException = PvaExceptionTranslator::createExceptionClass(FieldNotFound::PyExceptionClassName, pvaException);
    objectNotFoundException = PvaExceptionTranslator::createExceptionClass(ObjectNotFound::PyExceptionClassName, pvaException);
    objectAlreadyExistsException = PvaExceptionTranslator::createExceptionClass(ObjectAlreadyExists::PyExceptionClassName, pvaException);
    invalidArgumentException = PvaExceptionTranslator::createExceptionClass(InvalidArgument::PyExceptionClassName, pvaException);
    invalidDataTypeException = PvaExceptionTranslator::createExceptionClass(InvalidDataType::PyExceptionClassName, pvaException);
    invalidRequestException = PvaExceptionTranslator::createExceptionClass(InvalidRequest::PyExceptionClassName, pvaException);
    invalidStateException = PvaExceptionTranslator::createExceptionClass(InvalidState::PyExceptionClassName, pvaException);
    channelTimeoutException = PvaExceptionTranslator::createExceptionClass(ChannelTimeout::PyExceptionClassName, pvaException);

    // Enum wrappers
    wrapPvType();
    wrapPvProvider();

    // Class wrappers
    wrapPvObject();
    wrapPvScalar();
    wrapPvBoolean();
    wrapPvByte();
    wrapPvUByte();
    wrapPvShort();
    wrapPvUShort();
    wrapPvInt();
    wrapPvUInt();
    wrapPvLong();
    wrapPvULong();
    wrapPvFloat();
    wrapPvDouble();
    wrapPvString();
    wrapPvScalarArray();
    wrapPvUnion();
    wrapPvTimeStamp();
    wrapPvAlarm();

    wrapNtType();
    wrapNtTable();

    wrapChannel();
    wrapRpcClient();
    wrapRpcServer(); 
}
