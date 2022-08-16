// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/scope.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/docstring_options.hpp>

#include "PvaException.h"
#include "FieldNotFound.h"
#include "ObjectNotFound.h"
#include "ObjectAlreadyExists.h"
#include "InvalidArgument.h"
#include "InvalidDataType.h"
#include "InvalidState.h"
#include "InvalidRequest.h"
#include "ChannelTimeout.h"
#include "QueueEmpty.h"
#include "QueueFull.h"

#include "PvaExceptionTranslator.h"

// Forward wrapper declarations
//
// These functions are defined in pvaccess.<class>.cpp files, and
// are used to make code more readable and to avoid issues with
// compiler running out of memory 
void wrapConstants();
void wrapPvProvider();
void wrapPvType();

void wrapPvObject();
void wrapPvObjectQueue();
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
void wrapPvCodec();
void wrapPvControl();
void wrapPvDimension();
void wrapPvDisplay();
void wrapPvEnum();
void wrapPvValueAlarm();

void wrapNtType();
void wrapNtAttribute();
void wrapNtEnum();
void wrapNtNdArray();
void wrapNtScalar();
void wrapNtTable();

void wrapChannel();
void wrapRpcServer();
void wrapRpcClient();

#if PVA_API_VERSION >= 450
void wrapPvaServer();
#endif // if PVA_API_VERSION >= 450

#if PVA_API_VERSION >= 481
void wrapMultiChannel();
void wrapPvaMirrorServer();
#endif // if PVA_API_VERSION >= 481

void wrapScalarArrayPyOwner();
void wrapCaIoc();

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
PyObject* queueEmptyException = NULL;
PyObject* queueFullException = NULL;

BOOST_PYTHON_MODULE(pvaccess)
{
    Py_Initialize();

    using namespace boost::python;
    scope().attr("__doc__") = "pvaccess module is a python wrapper for `EPICS  <https://epics.anl.gov>`_ PV Access and other C++ libraries."; 
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
    queueEmptyException = PvaExceptionTranslator::createExceptionClass(QueueEmpty::PyExceptionClassName, pvaException);
    queueFullException = PvaExceptionTranslator::createExceptionClass(QueueFull::PyExceptionClassName, pvaException);

    // Constants
    wrapConstants();

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
    wrapPvCodec();
    wrapPvControl();
    wrapPvDimension();
    wrapPvDisplay();
    wrapPvEnum();
    wrapPvValueAlarm();

    wrapNtType();
    wrapNtAttribute();
    wrapNtEnum();
    wrapNtNdArray();
    wrapNtScalar();
    wrapNtTable();

    wrapChannel();
    wrapPvObjectQueue();
    wrapRpcClient();
    wrapRpcServer(); 

#if PVA_API_VERSION >= 450
    wrapPvaServer();
#endif // if PVA_API_VERSION >= 450

#if PVA_API_VERSION >= 481
    wrapMultiChannel();
    wrapPvaMirrorServer();
#endif // if PVA_API_VERSION >= 481

    wrapScalarArrayPyOwner(); 
    wrapCaIoc();
}
