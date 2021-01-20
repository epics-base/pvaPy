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
    : multiChannelPtr()
    , multiMonitorPtr()
    , monitorMutex()
    , monitorThreadExitEvent()
    , monitorPollPeriod()
    , monitorThreadRunning(false)
    , monitorActive(false)
{
    unsigned int listSize = bp::len(channelNames);
    epvd::shared_vector<std::string> names(listSize);
    for (unsigned int i = 0; i < listSize; i++) {
        names[i] = PyUtility::extractStringFromPyObject(channelNames[i]);
    }
    epvd::shared_vector<const std::string> names2(freeze(names));
    multiChannelPtr = epvac::PvaClientMultiChannel::create(pvaClientPtr, names2, PvProvider::getProviderName(providerType_));
}
    
MultiChannel::MultiChannel(const MultiChannel& mc) 
    : multiChannelPtr()
    , multiMonitorPtr()
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
    multiChannelPtr->connect();
    try {
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

void MultiChannel::put(const bp::list& pyList)
{
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
    epvd::Lock lock(monitorMutex);
    if (!monitorThreadRunning) {
        multiMonitorPtr = multiChannelPtr->createNTMonitor(requestDescriptor);
        monitorPollPeriod = pollPeriod;
        monitorActive = true;
        this->pySubscriber = pySubscriber;
        epicsThreadCreate("MultiChannelMonitorThread", epicsThreadPriorityHigh, epicsThreadGetStackSize(epicsThreadStackSmall), (EPICSTHREADFUNC)monitorThread, this);
    }
    else {
        logger.warn("Monitor is already running.");
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

void MultiChannel::monitorThread(MultiChannel* multiChannel)
{
    multiChannel->monitorThreadRunning = true;
    logger.debug("Started monitor thread %s", epicsThreadGetNameSelf());
    while (multiChannel->monitorActive) {

        try {
            bool result = multiChannel->multiMonitorPtr->poll();
            if (result) {
                epvac::PvaClientNTMultiDataPtr mData = multiChannel->multiMonitorPtr->getData();
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
 
