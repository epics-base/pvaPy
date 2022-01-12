// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PVA_SERVER_H
#define PVA_SERVER_H

#include <string>
#include <map>
#include <boost/python/list.hpp>
#include <pv/pvData.h>
#include <pv/pvAccess.h>
#include <pv/serverContext.h>

#include "PvObject.h"
#include "PyPvRecord.h"
#include "PvaPyLogger.h"
#include "SynchronizedQueue.h"

class PvaServer 
{
public:
    PvaServer();
    PvaServer(const std::string& channelName, const PvObject& pvObject);
    PvaServer(const std::string& channelName, const PvObject& pvObject, const boost::python::object& onWriteCallback);
    PvaServer(const PvaServer&);
    virtual ~PvaServer();
#if PVA_API_VERSION >= 483
    virtual void initAs(const std::string& filePath);
    virtual void initAs(const std::string& filePath, const std::string& substitutions);
    virtual bool isAsActive();
#endif // if PVA_API_VERSION >= 483
    virtual void update(const std::string& channelName, const epics::pvData::PVStructurePtr& pvStructurePtr);
    virtual void update(const PvObject& pvObject);
    virtual void update(const std::string& channelName, const PvObject& pvObject);

    virtual void addRecord(const std::string& channelName, const epics::pvData::PVStructurePtr& pvStructurePtr);
#ifndef WINDOWS
    virtual void addRecord(const std::string& channelName, const PvObject& pvObject, const boost::python::object& onWriteCallback = boost::python::object());
#else
    virtual void addRecord(const std::string& channelName, const PvObject& pvObject, const boost::python::object& onWriteCallback);
#endif // WINDOWS

#if PVA_API_VERSION >= 483
#ifndef WINDOWS
    virtual void addRecordWithAs(const std::string& channelName, const PvObject& pvObject, int asLevel, const std::string& asGroup, const boost::python::object& onWriteCallback = boost::python::object());
#else
    virtual void addRecordWithAs(const std::string& channelName, const PvObject& pvObject, int asLevel, const std::string& asGroup, const boost::python::object& onWriteCallback);
#endif // WINDOWS
#endif // if PVA_API_VERSION >= 483

    virtual void removeRecord(const std::string& channelName);
    virtual void removeAllRecords();
    virtual bool hasRecord(const std::string& channelName);
    virtual boost::python::list getRecordNames();
    virtual void start();
    virtual void stop();

protected:
    void initRecord(const std::string& channelName, const epics::pvData::PVStructurePtr& pvStructurePtr);

private:
    static const double ShutdownWaitTime;
    static const double RecordUpdateTimeout;

    static void callbackThread(PvaServer* server);
    void startCallbackThread();
    void waitForCallbackThreadExit(double timeout);
    void notifyCallbackThreadExit();

    void initRecord(const std::string& channelName, const PvObject& pvObject, const boost::python::object& onWriteCallback = boost::python::object());
#if PVA_API_VERSION >= 483
    void initRecord(const std::string& channelName, const PvObject& pvObject, int asLevel, const std::string& asGroup, const boost::python::object& onWriteCallback = boost::python::object());
#endif // if PVA_API_VERSION >= 483
    PyPvRecordPtr findRecord(const std::string& channelName);

    static PvaPyLogger logger;
    epics::pvAccess::ServerContext::shared_pointer server;
    std::map<std::string, PyPvRecordPtr> recordMap;
    bool isRunning;

    StringQueuePtr callbackQueuePtr;
    bool callbackThreadRunning;
    epics::pvData::Mutex callbackThreadMutex;
    epicsEvent callbackThreadExitEvent;
};

#endif
