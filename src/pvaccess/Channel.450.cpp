// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include <iostream>

#include "boost/python/extract.hpp"

#include "Channel.h"
#include "epicsThread.h"
#include "ChannelGetRequesterImpl.h"
#include "ChannelPutRequesterImpl.h"
#include "GetFieldRequesterImpl.h"
#include "RequesterImpl.h"
#include "pv/clientFactory.h"
#include "pv/logger.h"
#include "pv/convert.h"
#include "ChannelTimeout.h"
#include "InvalidRequest.h"
#include "InvalidArgument.h"
#include "ObjectNotFound.h"
#include "PyGilManager.h"
#include "PvUtility.h"
#include "PyUtility.h"

const char* Channel::DefaultRequestDescriptor("field(value)");
const double Channel::DefaultTimeout(3.0);
const double Channel::ShutdownWaitTime(0.1);

PvaPyLogger Channel::logger("Channel");
PvaClient Channel::pvaClient;
CaClient Channel::caClient;
epics::pvaClient::PvaClientPtr Channel::pvaClientPtr(epics::pvaClient::PvaClient::create());


Channel::Channel(const std::string& channelName, PvProvider::ProviderType providerType) :
    pvaClientChannelPtr(pvaClientPtr->createChannel(channelName,PvProvider::getProviderName(providerType))),
    pvObjectMonitorQueue(),
    monitorThreadDone(true),
    subscriberMap(),
    subscriberMutex(),
    monitorElementProcessingMutex(),
    monitorThreadMutex(),
    monitorThreadExitEvent(),
    timeout(DefaultTimeout)
{
    connect();
}
    
Channel::Channel(const Channel& c) :
    pvaClientChannelPtr(c.pvaClientChannelPtr),
    pvObjectMonitorQueue(),
    monitorThreadDone(true),
    subscriberMap(),
    subscriberMutex(),
    monitorThreadExitEvent(),
    timeout(DefaultTimeout)
{
    connect();
}

Channel::~Channel()
{
    stopMonitor();
    epicsThreadSleep(ShutdownWaitTime);
    pvaClientChannelPtr.reset();
}

void Channel::connect() 
{
    try {
        pvaClientChannelPtr->connect(timeout);
    } 
    catch (std::runtime_error e) {
        throw ChannelTimeout("Channel %s get request timed out", pvaClientChannelPtr->getChannelName().c_str());
    }
}

PvObject* Channel::get()
{
    return get(DefaultRequestDescriptor);
}

PvObject* Channel::get(const std::string& requestDescriptor) 
{
    try {
        epics::pvaClient::PvaClientGetPtr pvaGet = pvaClientChannelPtr->createGet(requestDescriptor);
        pvaGet->get();
        epics::pvData::PVStructurePtr pvStructure = pvaGet->getData()->getPVStructure();
        return new PvObject(pvStructure);
    }
    catch (std::runtime_error e) {
        throw PvaException(e.what());
    }
}

void Channel::put(const PvObject& pvObject)
{
    put(pvObject, DefaultRequestDescriptor);
}

void Channel::put(const PvObject& pvObject, const std::string& requestDescriptor) 
{
    try {
        epics::pvaClient::PvaClientPutPtr pvaPut = pvaClientChannelPtr->put(requestDescriptor);
        epics::pvData::PVStructurePtr pvSend = pvaPut->getData()->getPVStructure();
        pvSend << pvObject;
        pvaPut->put();
    } 
    catch (std::runtime_error e) {
        throw PvaException(e.what());
    }
}

void Channel::put(const std::vector<std::string>& values)
{
    put(values, DefaultRequestDescriptor);
}

void Channel::put(const std::vector<std::string>& values, const std::string& requestDescriptor) 
{
    try {
        epics::pvaClient::PvaClientPutPtr pvaPut = pvaClientChannelPtr->put(requestDescriptor);
        epics::pvaClient::PvaClientPutDataPtr pvaData = pvaPut->getData();
        pvaData->putStringArray(values);
        pvaPut->put();
    } 
    catch (std::runtime_error e) {
        throw PvaException(e.what());
    }
}

void Channel::put(const std::string& value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(const std::string& value, const std::string& requestDescriptor) 
{
    //std::vector<std::string> values;
    //values.push_back(value);
    //put(values, requestDescriptor);
    try {
        epics::pvaClient::PvaClientPutPtr pvaPut = pvaClientChannelPtr->put(requestDescriptor);
        epics::pvData::PVScalarPtr pvScalar = pvaPut->getData()->getScalarValue();
        epics::pvData::getConvert()->fromString(pvScalar,value);
        pvaPut->put();
    } 
    catch (std::runtime_error e) {
        throw PvaException(e.what());
    }
}

void Channel::put(const boost::python::list& pyList, const std::string& requestDescriptor) 
{
    int listSize = boost::python::len(pyList);
    std::vector<std::string> values(listSize);
    for (int i = 0; i < listSize; i++) {
        values[i] = PyUtility::extractStringFromPyObject(pyList[i]);
    }
    put(values, requestDescriptor);
}

void Channel::put(const boost::python::list& pyList)
{
    put(pyList, DefaultRequestDescriptor);
}

void Channel::put(bool value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<bool>(value), requestDescriptor);
}

void Channel::put(bool value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(char value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<int>(static_cast<int>(value)), requestDescriptor);
}

void Channel::put(char value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(unsigned char value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<int>(static_cast<int>(value)), requestDescriptor);
}

void Channel::put(unsigned char value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(short value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<short>(value), requestDescriptor);
}

void Channel::put(short value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(unsigned short value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<unsigned short>(value), requestDescriptor);
}

void Channel::put(unsigned short value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(int value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<int>(value), requestDescriptor);
}

void Channel::put(int value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(unsigned int value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<unsigned int>(value), requestDescriptor);
}

void Channel::put(unsigned int value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(long long value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<long long>(value), requestDescriptor);
}

void Channel::put(long long value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(unsigned long long value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<unsigned long long>(value), requestDescriptor);
}

void Channel::put(unsigned long long value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(float value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<float>(value), requestDescriptor);
}

void Channel::put(float value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(double value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<double>(value), requestDescriptor);
}

void Channel::put(double value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::subscribe(const std::string& subscriberName, const boost::python::object& pySubscriber)
{
    //epics::pvData::Lock lock(subscriberMutex);
    subscriberMap[subscriberName] = pySubscriber;
}

void Channel::unsubscribe(const std::string& subscriberName)
{
    //epics::pvData::Lock lock(subscriberMutex);
    boost::python::object pySubscriber = subscriberMap[subscriberName];
    std::map<std::string,boost::python::object>::const_iterator iterator = subscriberMap.find(subscriberName);
    if (iterator == subscriberMap.end()) {
        throw ObjectNotFound("Subscriber " + subscriberName + " is not registered.");
    }
    subscriberMap.erase(subscriberName);
    logger.trace("Unsubscribed monitor " + subscriberName);
}

void Channel::callSubscribers(PvObject& pvObject)
{
    //epics::pvData::Lock lock(subscriberMutex);

    std::map<std::string,boost::python::object>::iterator iter;
    for (iter = subscriberMap.begin(); iter != subscriberMap.end(); iter++) {
        std::string subscriberName = iter->first;
        boost::python::object pySubscriber = iter->second;

        // Acquire GIL. This is required because callSubscribers()
        // is called in a monitoring thread. Before monitoring thread
        // is created, one must call PyEval_InitThreads() in the main thread
        // to initialize things properly. If this is not done, code will
        // most likely crash while invoking python from c++, or while
        // attempting to release GIL.
        // PyGILState_STATE gilState = PyGILState_Ensure();
        logger.trace("Acquiring python GIL for subscriber " + subscriberName);
        PyGilManager::gilStateEnsure();

        try {
            logger.debug("Invoking subscriber: " + subscriberName);

            // Call python code
            pySubscriber(pvObject);
        }
        catch(const boost::python::error_already_set&) {
            logger.error("Channel subscriber " + subscriberName + " error");
        }

        // Release GIL. 
        // PyGILState_Release(gilState);
        logger.trace("Releasing python GIL");
        PyGilManager::gilStateRelease();
    }
    logger.trace("Done calling subscribers");
}

void Channel::startMonitor()
{
    startMonitor(DefaultRequestDescriptor);
}

void Channel::startMonitor(const std::string& requestDescriptor)
{
    epics::pvData::Lock lock(monitorThreadMutex);
    if (monitorThreadDone) {
        monitorThreadDone = false;

        // One must call PyEval_InitThreads() in the main thread
        // to initialize thread state, which is needed for proper functioning
        // of PyGILState_Ensure()/PyGILState_Release().
        // PyEval_InitThreads();
        PyGilManager::evalInitThreads();
        try {
            pvaClientMonitorPtr = pvaClientChannelPtr->createMonitor(requestDescriptor);
            pvaClientMonitorPtr->connect();
            pvaClientMonitorPtr->start();
        } 
        catch (std::runtime_error e) {
            logger.error(e.what());
            throw PvaException(e.what());
        }
        epicsThreadCreate("ChannelMonitorThread", epicsThreadPriorityLow, epicsThreadGetStackSize(epicsThreadStackSmall), (EPICSTHREADFUNC)monitorThread, this);
        epicsThreadCreate("ChannelProcessingThread", epicsThreadPriorityHigh, epicsThreadGetStackSize(epicsThreadStackSmall), (EPICSTHREADFUNC)processingThread, this);
    }
}

void Channel::stopMonitor()
{
    epics::pvData::Lock lock(monitorThreadMutex);
    if (monitorThreadDone) {
        logger.debug("Monitor thread is not running");
        return;
    }
    monitorThreadDone = true;
    logger.debug("Stopping monitor");
    try {
        pvaClientMonitorPtr->stop();
    } 
    catch (std::runtime_error e) {
        logger.error(e.what());
        throw PvaException(e.what());
    }
    logger.debug("Monitor stopped, waiting for thread exit");
    monitorThreadExitEvent.wait(getTimeout());
}

bool Channel::isMonitorThreadDone() const
{
    return monitorThreadDone;
}

bool Channel::processMonitorElement() 
{
    //epics::pvData::Lock lock(monitorElementProcessingMutex);

    // Handle possible exceptions while retrieving data from empty queue.
    try {
        try {
            PvObject pvObject = pvObjectMonitorQueue.frontAndPop(timeout);
            callSubscribers(pvObject);
        }
        catch (InvalidState& ex) {
            throw ChannelTimeout("No PV changes received.");
        }
    }
    catch (const ChannelTimeout& ex) {
        // Ignore, no changes received.
    }
    catch (const std::exception& ex) {
        // Not good.
        logger.error("Exception caught in monitor thread: %s", ex.what());
    }
    return false;
}

void Channel::monitorThread(Channel* channel)
{
    logger.debug("Started monitor thread %s", epicsThreadGetNameSelf());
    epics::pvaClient::PvaClientMonitorPtr monitor = channel->getMonitor();
    epics::pvaClient::PvaClientMonitorDataPtr pvaData = monitor->getData();
    while (true) {
        if (channel->isMonitorThreadDone()) {
            break;
        }

        monitor->waitEvent();
        PvObject pvObject(pvaData->getPVStructure());
        channel->queueMonitorData(pvObject);
        monitor->releaseEvent();
    }
    logger.debug("Exiting monitor thread %s", epicsThreadGetNameSelf());
    channel->notifyMonitorThreadExit();
}

void Channel::processingThread(Channel* channel)
{
    logger.debug("Started processing thread %s", epicsThreadGetNameSelf());
    epics::pvaClient::PvaClientMonitorPtr monitor = channel->getMonitor();
    epics::pvaClient::PvaClientMonitorDataPtr pvaData = monitor->getData();
    while (true) {
        if (channel->isMonitorThreadDone()) {
            break;
        }
        channel->processMonitorElement();
    }
    logger.debug("Exiting processing thread %s", epicsThreadGetNameSelf());
}

epics::pvaClient::PvaClientMonitorPtr Channel::getMonitor() 
{
    return pvaClientChannelPtr->monitor("");
}

void Channel::queueMonitorData(PvObject& pvObject) 
{
    pvObjectMonitorQueue.push(pvObject);
}

