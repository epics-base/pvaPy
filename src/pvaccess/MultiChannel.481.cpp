// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution


#include <boost/python.hpp>
#include <iostream>

#include <epicsThread.h>

#include "MultiChannel.h"
#include "PyUtility.h"
#include "PyPvDataUtility.h"
#include "PyGilManager.h"

namespace bp = boost::python;
namespace epvd = epics::pvData;
namespace epvac = epics::pvaClient;

PvaPyLogger MultiChannel::logger("MultiChannel");
PvaClient MultiChannel::pvaClient;
CaClient MultiChannel::caClient;
const double MultiChannel::DefaultMonitorPollPeriod(1.0);
const double MultiChannel::ShutdownWaitTime(0.1);
epvac::PvaClientPtr MultiChannel::pvaClientPtr(epics::pvaClient::PvaClient::get("pva ca"));

MultiChannel::MultiChannel(const bp::list& channelNames, PvProvider::ProviderType providerType_) 
    : nChannels(0)
    , multiChannelPtr()
    , ntMultiMonitorPtr()
    , doubleMultiMonitorPtr()
    , monitorMutex()
    , monitorThreadExitEvent()
    , monitorPollPeriod()
    , monitorThreadRunning(false)
    , monitorActive(false)
{
    nChannels = bp::len(channelNames);
    epvd::shared_vector<std::string> names(nChannels);
    for (unsigned int i = 0; i < nChannels; i++) {
        names[i] = PyUtility::extractStringFromPyObject(channelNames[i]);
    }
    epvd::shared_vector<const std::string> names2(freeze(names));
    multiChannelPtr = epvac::PvaClientMultiChannel::create(pvaClientPtr, names2, PvProvider::getProviderName(providerType_));
}
    
MultiChannel::MultiChannel(const MultiChannel& mc) 
    : multiChannelPtr()
    , ntMultiMonitorPtr()
    , doubleMultiMonitorPtr()
    , monitorMutex()
    , monitorThreadExitEvent()
    , monitorPollPeriod()
    , monitorThreadRunning(false)
    , monitorActive(false)
{
}

MultiChannel::~MultiChannel()
{
    stopMonitor();
    multiChannelPtr.reset();
    waitForMonitorThreadExit(ShutdownWaitTime);
}

PvObject* MultiChannel::get()
{
    return get(PvaConstants::FieldValueAlarmTimestampRequest);
}

PvObject* MultiChannel::get(const std::string& requestDescriptor)
{
    try {
        multiChannelPtr->connect();
        epvac::PvaClientNTMultiGetPtr mGet(multiChannelPtr->createNTGet(requestDescriptor));
        mGet->get();
        epvac::PvaClientNTMultiDataPtr mData = mGet->getData();
        epvd::PVStructurePtr pvStructure = mData->getNTMultiChannel()->getPVStructure();
        return new PvObject(pvStructure);
    }
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
}

boost::python::list MultiChannel::getAsDoubleArray()
{
    try {
        multiChannelPtr->connect();
        epvac::PvaClientMultiGetDoublePtr mGet(multiChannelPtr->createGet());
        epvd::shared_vector<double> data = mGet->get();
        bp::list pyList;
        for(unsigned int i = 0; i < data.size(); i++) {
            pyList.append(data[i]);
        }
        return pyList;
    }
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
}

void MultiChannel::put(const bp::list& pyList)
{
    try {
        multiChannelPtr->connect();
        epvac::PvaClientNTMultiPutPtr mPut(multiChannelPtr->createNTPut());
        epvd::shared_vector<epvd::PVUnionPtr> data = mPut->getValues();
        unsigned int listSize = bp::len(pyList);
        for (unsigned int i = 0; i < data.size(); i++) {
            if (i > listSize) {
                break;
            }
            bp::object pyObject = pyList[i];
            bp::extract<PvObject> extractPvObject(pyObject);
            if (extractPvObject.check()) {
                PvObject pvObject = extractPvObject();
                std::string keyFrom = PyPvDataUtility::getValueOrSingleFieldName(pvObject.getPvStructurePtr());
                epvd::PVFieldPtr pvFrom = PyPvDataUtility::getSubField(keyFrom, pvObject.getPvStructurePtr());
                PyPvDataUtility::setUnionField(pvFrom, data[i]);
            }
        }
        mPut->put();
    }
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
}

void MultiChannel::putAsDoubleArray(const bp::list& pyList)
{
    try {
        multiChannelPtr->connect();
        epvac::PvaClientMultiPutDoublePtr mPut(multiChannelPtr->createPut());
        epvd::shared_vector<double> data(nChannels, 0);
        unsigned int listSize = bp::len(pyList);
        for (unsigned int i = 0; i < nChannels; i++) {
            if (i > listSize) {
                break;
            }
            bp::object pyObject = pyList[i];
            bp::extract<double> extractDouble(pyObject);
            if (extractDouble.check()) {
                data[i] = extractDouble();
            }
        }
        mPut->put(data);
    }
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
}

void MultiChannel::monitor(const bp::object& pySubscriber)
{
    monitor(pySubscriber, DefaultMonitorPollPeriod, PvaConstants::FieldValueAlarmTimestampRequest);
}

void MultiChannel::monitor(const bp::object& pySubscriber, double pollPeriod)
{
    monitor(pySubscriber, pollPeriod, PvaConstants::FieldValueAlarmTimestampRequest);
}

void MultiChannel::monitor(const bp::object& pySubscriber, double pollPeriod, const std::string& requestDescriptor)
{
    try {
        epvd::Lock lock(monitorMutex);
        if (!monitorThreadRunning) {
            multiChannelPtr->connect();
            ntMultiMonitorPtr = multiChannelPtr->createNTMonitor(requestDescriptor);
            monitorPollPeriod = pollPeriod;
            monitorActive = true;
            this->pySubscriber = pySubscriber;
            epicsThreadCreate("NtMultiChannelMonitorThread", epicsThreadPriorityHigh, epicsThreadGetStackSize(epicsThreadStackSmall), (EPICSTHREADFUNC)ntMonitorThread, this);
        }
        else {
            logger.warn("Monitor is already running.");
        }
    }
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
}

void MultiChannel::monitorAsDoubleArray(const bp::object& pySubscriber)
{
    monitorAsDoubleArray(pySubscriber, DefaultMonitorPollPeriod);
}

void MultiChannel::monitorAsDoubleArray(const bp::object& pySubscriber, double pollPeriod)
{
    try {
        epvd::Lock lock(monitorMutex);
        if (!monitorThreadRunning) {
            multiChannelPtr->connect();
            doubleMultiMonitorPtr = multiChannelPtr->createMonitor();
            monitorPollPeriod = pollPeriod;
            monitorActive = true;
            this->pySubscriber = pySubscriber;
            epicsThreadCreate("DoubleMultiChannelMonitorThread", epicsThreadPriorityHigh, epicsThreadGetStackSize(epicsThreadStackSmall), (EPICSTHREADFUNC)doubleMonitorThread, this);
        }
        else {
            logger.warn("Monitor is already running.");
        }
    }
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
}

void MultiChannel::stopMonitor()
{
    epvd::Lock lock(monitorMutex);
    if (!monitorActive) {
        logger.trace("Monitor is not active.");
        return;
    }
    // Monitor thread should exit after monitorActive is set to false
    monitorActive = false;
}

void MultiChannel::notifyMonitorThreadExit()
{
    monitorThreadExitEvent.signal();
}

void MultiChannel::waitForMonitorThreadExit(double timeout)
{
    if (monitorThreadRunning) {
        logger.debug("Waiting on monitor thread exit, timeout in %f seconds", timeout);
        monitorThreadExitEvent.wait(timeout);
    }
}

void MultiChannel::ntMonitorThread(MultiChannel* multiChannel)
{
    multiChannel->monitorThreadRunning = true;
    logger.debug("Started monitor thread %s", epicsThreadGetNameSelf());
    while (multiChannel->monitorActive) {

        try {
            bool result = multiChannel->ntMultiMonitorPtr->poll();
            if (result) {
                epvac::PvaClientNTMultiDataPtr mData = multiChannel->ntMultiMonitorPtr->getData();
                epvd::PVStructurePtr pvStructure = mData->getNTMultiChannel()->getPVStructure();
                PvObject pvObject(pvStructure);
                multiChannel->callSubscriber(pvObject);
            } 
        }
        catch (const std::exception& ex) {
            // Not good.
            logger.error("Monitor thread caught exception while processing monitor data: %s", ex.what());
        }
        sleep(multiChannel->monitorPollPeriod);
    }

    // Monitor thread done.
    logger.debug("Exiting monitor thread %s", epicsThreadGetNameSelf());
    multiChannel->monitorThreadExitEvent.signal();
    multiChannel->monitorThreadRunning = false;
}

void MultiChannel::doubleMonitorThread(MultiChannel* multiChannel)
{
    multiChannel->monitorThreadRunning = true;
    logger.debug("Started monitor thread %s", epicsThreadGetNameSelf());
    while (multiChannel->monitorActive) {

        try {
            bool result = multiChannel->doubleMultiMonitorPtr->poll();
            if (result) {
                epvd::shared_vector<double> data = multiChannel->doubleMultiMonitorPtr->get();
                bp::list pyList;
                for(unsigned int i = 0; i < data.size(); i++) {
                    pyList.append(data[i]);
                }
                multiChannel->callSubscriber(pyList);
            } 
        }
        catch (const std::exception& ex) {
            // Not good.
            logger.error("Monitor thread caught exception while processing monitor data: %s", ex.what());
        }
        sleep(multiChannel->monitorPollPeriod);
    }

    // Monitor thread done.
    logger.debug("Exiting monitor thread %s", epicsThreadGetNameSelf());
    multiChannel->monitorThreadExitEvent.signal();
    multiChannel->monitorThreadRunning = false;
}

void MultiChannel::callSubscriber(PvObject& pvObject)
{
    // Acquire GIL. This is required because callSubscribers()
    // is called in a monitoring thread. Before monitoring thread
    // is created, one must call PyEval_InitThreads() in the main thread
    // to initialize things properly. If this is not done, code will
    // most likely crash while invoking python from c++, or while
    // attempting to release GIL.
    // PyGILState_STATE gilState = PyGILState_Ensure();
    // logger.trace("Acquiring python GIL for multi-channel subscriber");
    PyGilManager::gilStateEnsure();

    // Call python code
    try {
        pySubscriber(pvObject);
    }
    catch(const bp::error_already_set&) {
        logger.error("MultiChannel subscriber raised python exception.");
        PyErr_Print();
        PyErr_Clear();
    }
    catch (const std::exception& ex) {
        logger.error(ex.what());
    }

    // Release GIL.
    // PyGILState_Release(gilState);
    // logger.trace("Releasing python GIL after processing multi-channel monitor data");
    PyGilManager::gilStateRelease();
}
 
void MultiChannel::callSubscriber(bp::list& pyList)
{
    // Acquire GIL. This is required because callSubscribers()
    // is called in a monitoring thread. Before monitoring thread
    // is created, one must call PyEval_InitThreads() in the main thread
    // to initialize things properly. If this is not done, code will
    // most likely crash while invoking python from c++, or while
    // attempting to release GIL.
    // PyGILState_STATE gilState = PyGILState_Ensure();
    // logger.trace("Acquiring python GIL for multi-channel subscriber");
    PyGilManager::gilStateEnsure();

    // Call python code
    try {
        // Python call will decrease reference for the list
        bp::incref(pyList.ptr());
        pySubscriber(pyList);
    }
    catch(const bp::error_already_set&) {
        logger.error("MultiChannel subscriber raised python exception.");
        PyErr_Print();
        PyErr_Clear();
    }
    catch (const std::exception& ex) {
        logger.error(ex.what());
    }

    // Release GIL.
    // PyGILState_Release(gilState);
    // logger.trace("Releasing python GIL after processing multi-channel monitor data");
    PyGilManager::gilStateRelease();
}
