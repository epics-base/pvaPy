// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PY_PV_RECORD_H
#define PY_PV_RECORD_H

#include <string>

#include "boost/python/object.hpp"

#include "pv/pvData.h"
#include "pv/pvDatabase.h"
#include "PvObject.h"
#include "PvaPyLogger.h"
#include "SynchronizedQueue.h"

class PyPvRecord;
typedef std::tr1::shared_ptr<PyPvRecord> PyPvRecordPtr;

class PyPvRecord : 
    public epics::pvDatabase::PVRecord
{
public:
    static PyPvRecordPtr create(const std::string& name, const PvObject& pvObject, const StringQueuePtr& callbackQueuePtr, const boost::python::object& onWriteCallback = boost::python::object());
    POINTER_DEFINITIONS(PyPvRecord);
    virtual ~PyPvRecord(); 
    virtual bool init();
    virtual void process();
    void update (const PvObject& pvObject);
    void executeCallback();
private:
    static PvaPyLogger logger;
    PyPvRecord(const std::string& name, const PvObject& pvObject, const StringQueuePtr& callbackQueuePtr, const boost::python::object& onWriteCallback = boost::python::object());

    StringQueuePtr callbackQueuePtr; 
    boost::python::object onWriteCallback;

};

#endif
