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
#include "ObjectAlreadyExists.h"
#include "PyGilManager.h"
#include "PvUtility.h"
#include "PyUtility.h"
#include "PyPvDataUtility.h"
#include "PvaConstants.h"
#include "PvaClientUtility.h"

const char* Channel::DefaultRequestDescriptor("field(value)");
const char* Channel::DefaultPutGetRequestDescriptor("putField(value)getField(value)");
const double Channel::DefaultTimeout(3.0);
const int Channel::DefaultMaxPvObjectQueueLength(0);
const double Channel::ShutdownWaitTime(1.0);

PvaPyLogger Channel::logger("Channel");
PvaClient Channel::pvaClient;
CaClient Channel::caClient;
epics::pvaClient::PvaClientPtr Channel::pvaClientPtr(epics::pvaClient::PvaClient::create());


Channel::Channel(const std::string& channelName, PvProvider::ProviderType providerType) :
    pvaClientChannelPtr(pvaClientPtr->createChannel(channelName,PvProvider::getProviderName(providerType))),
    monitorActive(false),
    processingThreadRunning(false),
    pvObjectQueue(DefaultMaxPvObjectQueueLength),
    subscriberMap(),
    subscriberMutex(),
    monitorMutex(),
    processingThreadMutex(),
    processingThreadExitEvent(),
    timeout(DefaultTimeout)
{
    connect();
}
    
Channel::Channel(const Channel& c) :
    pvaClientChannelPtr(c.pvaClientChannelPtr),
    monitorActive(false),
    processingThreadRunning(false),
    pvObjectQueue(DefaultMaxPvObjectQueueLength),
    subscriberMap(),
    subscriberMutex(),
    monitorMutex(),
    processingThreadMutex(),
    processingThreadExitEvent(),
    timeout(DefaultTimeout)
{
    connect();
}

Channel::~Channel()
{
    stopMonitor();
    pvaClientChannelPtr.reset();
    waitForProcessingThreadExit(ShutdownWaitTime);
    epicsThreadSleep(ShutdownWaitTime);
}

void Channel::connect() 
{
    try {
        pvaClientChannelPtr->connect(timeout);
    } 
    catch (std::runtime_error&) {
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
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
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
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
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
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
}

void Channel::put(const std::string& value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(const std::string& value, const std::string& requestDescriptor) 
{
    try {
        epics::pvaClient::PvaClientPutPtr pvaPut = pvaClientChannelPtr->put(requestDescriptor);
        epics::pvaClient::PvaClientPutDataPtr pvaData = pvaPut->getData();
        if (pvaData->isValueScalar()) {
            // value is scalar
            epics::pvData::PVScalarPtr pvScalar = pvaData->getScalarValue();
            epics::pvData::getConvert()->fromString(pvScalar,value);
        }
        else {
            // value is not scalar, try object
            epics::pvData::PVStructurePtr pvStructure = pvaData->getPVStructure();
            std::vector<std::string> values;
            values.push_back(value);
            PvUtility::fromString(pvStructure, values);
        }
        pvaPut->put();
    } 
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
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

//
// Given that python cannot distinguish between some data types
// like short vs int, use generic put method that utilizes strings
// for all scalar types
//

void Channel::put(bool value, const std::string& requestDescriptor)
{
    put(StringUtility::toString(value), requestDescriptor);
    //PvaClientUtility::put<bool, epics::pvData::PVBoolean, epics::pvData::PVBooleanPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(bool value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(char value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<int>(static_cast<int>(value)), requestDescriptor);
    //PvaClientUtility::put<char, epics::pvData::PVByte, epics::pvData::PVBytePtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(char value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(unsigned char value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<int>(static_cast<int>(value)), requestDescriptor);
    //PvaClientUtility::put<unsigned char, epics::pvData::PVUByte, epics::pvData::PVUBytePtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(unsigned char value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(short value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<short>(value), requestDescriptor);
    //PvaClientUtility::put<short, epics::pvData::PVShort, epics::pvData::PVShortPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(short value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(unsigned short value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<unsigned short>(value), requestDescriptor);
    //PvaClientUtility::put<unsigned short, epics::pvData::PVUShort, epics::pvData::PVUShortPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(unsigned short value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(long int value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<int>(value), requestDescriptor);
    //PvaClientUtility::put<int, epics::pvData::PVInt, epics::pvData::PVIntPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(long int value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(unsigned long int value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<unsigned int>(value), requestDescriptor);
    //PvaClientUtility::put<unsigned int, epics::pvData::PVUInt, epics::pvData::PVUIntPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(unsigned long int value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(long long value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<long long>(value), requestDescriptor);
    //PvaClientUtility::put<long long, epics::pvData::PVLong, epics::pvData::PVLongPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(long long value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(unsigned long long value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<unsigned long long>(value), requestDescriptor);
    //PvaClientUtility::put<unsigned long long, epics::pvData::PVULong, epics::pvData::PVULongPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(unsigned long long value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(float value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<float>(value), requestDescriptor);
    //PvaClientUtility::put<float, epics::pvData::PVFloat, epics::pvData::PVFloatPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(float value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(double value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<double>(value), requestDescriptor);
    //PvaClientUtility::put<double, epics::pvData::PVDouble, epics::pvData::PVDoublePtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(double value)
{
    put(value, DefaultRequestDescriptor);
}

// PutGet methods

PvObject* Channel::putGet(const PvObject& pvObject, const std::string& requestDescriptor) 
{
    try {
        epics::pvaClient::PvaClientPutGetPtr pvaPutGet = pvaClientChannelPtr->createPutGet(requestDescriptor);
        epics::pvData::PVStructurePtr pvPut = pvaPutGet->getPutData()->getPVStructure();
        pvPut << pvObject;
        pvaPutGet->putGet();
        epics::pvData::PVStructurePtr pvGet = pvaPutGet->getGetData()->getPVStructure();
        return new PvObject(pvGet);
    } 
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
}

PvObject* Channel::putGet(const PvObject& pvObject)
{
    return putGet(pvObject, DefaultPutGetRequestDescriptor);
}

PvObject* Channel::putGet(const std::vector<std::string>& values, const std::string& requestDescriptor) 
{
    try {
        epics::pvaClient::PvaClientPutGetPtr pvaPutGet = pvaClientChannelPtr->createPutGet(requestDescriptor);
        epics::pvaClient::PvaClientPutDataPtr pvaData = pvaPutGet->getPutData();
        pvaData->putStringArray(values);
        pvaPutGet->putGet();
        return new PvObject(pvaPutGet->getGetData()->getPVStructure());
    } 
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
}

PvObject* Channel::putGet(const std::vector<std::string>& values)
{
    return putGet(values, DefaultPutGetRequestDescriptor);
}

PvObject* Channel::putGet(const std::string& value, const std::string& requestDescriptor) 
{
    try {
        epics::pvaClient::PvaClientPutGetPtr pvaPutGet = pvaClientChannelPtr->createPutGet(requestDescriptor);
        epics::pvaClient::PvaClientPutDataPtr pvaData = pvaPutGet->getPutData();
        if (pvaData->isValueScalar()) {
            // value is scalar
            epics::pvData::PVScalarPtr pvScalar = pvaData->getScalarValue();
            epics::pvData::getConvert()->fromString(pvScalar,value);
        }
        else {
            // value is not scalar, try object
            epics::pvData::PVStructurePtr pvStructure = pvaData->getPVStructure();
            std::vector<std::string> values;
            values.push_back(value);
            PvUtility::fromString(pvStructure, values);
        }
        pvaPutGet->putGet();
        return new PvObject(pvaPutGet->getGetData()->getPVStructure());
    } 
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
}

PvObject* Channel::putGet(const std::string& value)
{
    return putGet(value, DefaultPutGetRequestDescriptor);
}

PvObject* Channel::putGet(const boost::python::list& pyList, const std::string& requestDescriptor) 
{
    int listSize = boost::python::len(pyList);
    std::vector<std::string> values(listSize);
    for (int i = 0; i < listSize; i++) {
        values[i] = PyUtility::extractStringFromPyObject(pyList[i]);
    }
    return putGet(values, requestDescriptor);
}

PvObject* Channel::putGet(const boost::python::list& pyList)
{
    return putGet(pyList, DefaultPutGetRequestDescriptor);
}

PvObject* Channel::putGet(bool value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString(value), requestDescriptor);
    //return PvaClientUtility::putGet<bool, epics::pvData::PVBoolean, epics::pvData::PVBooleanPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(bool value)
{
    return putGet(value, DefaultPutGetRequestDescriptor);
}

PvObject* Channel::putGet(char value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<int>(static_cast<int>(value)), requestDescriptor);
    //return PvaClientUtility::putGet<char, epics::pvData::PVByte, epics::pvData::PVBytePtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(char value)
{
    return putGet(value, DefaultPutGetRequestDescriptor);
}

PvObject* Channel::putGet(unsigned char value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<int>(static_cast<int>(value)), requestDescriptor);
    //return PvaClientUtility::putGet<unsigned char, epics::pvData::PVUByte, epics::pvData::PVUBytePtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(unsigned char value)
{
    return putGet(value, DefaultPutGetRequestDescriptor);
}

PvObject* Channel::putGet(short value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<short>(value), requestDescriptor);
    //return PvaClientUtility::putGet<short, epics::pvData::PVShort, epics::pvData::PVShortPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(short value)
{
    return putGet(value, DefaultPutGetRequestDescriptor);
}

PvObject* Channel::putGet(unsigned short value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<unsigned short>(value), requestDescriptor);
    //return PvaClientUtility::putGet<unsigned short, epics::pvData::PVUShort, epics::pvData::PVUShortPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(unsigned short value)
{
    return putGet(value, DefaultPutGetRequestDescriptor);
}

PvObject* Channel::putGet(long int value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<int>(value), requestDescriptor);
    //return PvaClientUtility::putGet<int, epics::pvData::PVInt, epics::pvData::PVIntPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(long int value)
{
    return putGet(value, DefaultPutGetRequestDescriptor);
}

PvObject* Channel::putGet(unsigned long int value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<unsigned int>(value), requestDescriptor);
    //return PvaClientUtility::putGet<unsigned int, epics::pvData::PVUInt, epics::pvData::PVUIntPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(unsigned long int value)
{
    return putGet(value, DefaultPutGetRequestDescriptor);
}

PvObject* Channel::putGet(long long value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<long long>(value), requestDescriptor);
    //return PvaClientUtility::putGet<long long, epics::pvData::PVLong, epics::pvData::PVLongPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(long long value)
{
    return putGet(value, DefaultPutGetRequestDescriptor);
}

PvObject* Channel::putGet(unsigned long long value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<unsigned long long>(value), requestDescriptor);
    //return PvaClientUtility::putGet<unsigned long long, epics::pvData::PVULong, epics::pvData::PVULongPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(unsigned long long value)
{
    return putGet(value, DefaultPutGetRequestDescriptor);
}

PvObject* Channel::putGet(float value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<float>(value), requestDescriptor);
    //return PvaClientUtility::putGet<float, epics::pvData::PVFloat, epics::pvData::PVFloatPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(float value)
{
    return putGet(value, DefaultPutGetRequestDescriptor);
}

PvObject* Channel::putGet(double value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<double>(value), requestDescriptor);
    //return PvaClientUtility::putGet<double, epics::pvData::PVDouble, epics::pvData::PVDoublePtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(double value)
{
    return putGet(value, DefaultPutGetRequestDescriptor);
}

//
// GetPut methods
//
PvObject* Channel::getPut()
{
    return getPut(DefaultRequestDescriptor);
}

PvObject* Channel::getPut(const std::string& requestDescriptor) 
{
    try {
        epics::pvaClient::PvaClientPutGetPtr pvaPutGet = pvaClientChannelPtr->createPutGet(requestDescriptor);
        pvaPutGet->getPut();
        return new PvObject(pvaPutGet->getPutData()->getPVStructure());
    } 
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
}

//
// Monitor methods
//
void Channel::subscribe(const std::string& subscriberName, const boost::python::object& pySubscriber)
{
    epics::pvData::Lock lock(subscriberMutex);
    std::map<std::string,boost::python::object>::const_iterator iterator = subscriberMap.find(subscriberName);
    if (iterator != subscriberMap.end()) {
        throw ObjectAlreadyExists("Subscriber " + subscriberName + " is already registered for channel " + getName() + ".");
    }
    subscriberMap[subscriberName] = pySubscriber;
    logger.trace("Subscribed " + subscriberName + " to monitor channel " + getName() + ".");

    //boost::python::incref(pySubscriber.ptr());
    //PyObject* pyObject = pySubscriber.ptr();
    //int refCount = pyObject->ob_refcnt;
    //boost::python::decref(pySubscriber.ptr());
}

void Channel::unsubscribe(const std::string& subscriberName)
{
    epics::pvData::Lock lock(subscriberMutex);
    std::map<std::string,boost::python::object>::const_iterator iterator = subscriberMap.find(subscriberName);
    if (iterator == subscriberMap.end()) {
        throw ObjectNotFound("Subscriber " + subscriberName + " is not registered for channel " + getName() + ".");
    }
    boost::python::object pySubscriber = subscriberMap[subscriberName];
    subscriberMap.erase(subscriberName);
    logger.trace("Unsubscribed " + subscriberName + " from channel " + getName() + ".");
}

void Channel::callSubscribers(PvObject& pvObject)
{
    std::map<std::string,boost::python::object> subscriberMap2;
    {
        // Cannot lock entire call, as subscribers may be added/deleted at
        // any time, causing deadlock
        epics::pvData::Lock lock(subscriberMutex);
        subscriberMap2 = subscriberMap;
    }

    std::map<std::string,boost::python::object>::iterator mIter;
    for (mIter = subscriberMap2.begin(); mIter != subscriberMap2.end(); mIter++) {
        std::string subscriberName = mIter->first;
        boost::python::object pySubscriber = mIter->second;

        // Acquire GIL. This is required because callSubscribers()
        // is called in a monitoring thread. Before monitoring thread
        // is created, one must call PyEval_InitThreads() in the main thread
        // to initialize things properly. If this is not done, code will
        // most likely crash while invoking python from c++, or while
        // attempting to release GIL.
        // PyGILState_STATE gilState = PyGILState_Ensure();
        logger.trace("Acquiring python GIL for subscriber " + subscriberName);
        PyGilManager::gilStateEnsure();

        // Call python code
        try {
            pySubscriber(pvObject);
        }
        catch(const boost::python::error_already_set&) {
            logger.error("Channel subscriber " + subscriberName + " raised python exception.");
            PyErr_Print();
            PyErr_Clear();
        }
        catch (const std::exception& ex) {
            logger.error(ex.what());
        }

        // Release GIL. 
        // PyGILState_Release(gilState);
        logger.trace("Releasing python GIL after processing monitor data with " + subscriberName);
        PyGilManager::gilStateRelease();
    }
}

void Channel::setMonitorMaxQueueLength(int maxLength)
{
    pvObjectQueue.setMaxLength(maxLength);
    if (maxLength != 0 && !processingThreadRunning) {
        startProcessingThread();
    }
}

int Channel::getMonitorMaxQueueLength()
{
    return pvObjectQueue.getMaxLength();
}

void Channel::startMonitor()
{
    startMonitor(DefaultRequestDescriptor);
}

void Channel::startMonitor(const std::string& requestDescriptor)
{
    epics::pvData::Lock lock(monitorMutex);
    if (monitorActive) {
        logger.warn("Monitor is already active.");
        return;
    }
    monitorActive = true;

    // One must call PyEval_InitThreads() in the main thread
    // to initialize thread state, which is needed for proper functioning
    // of PyGILState_Ensure()/PyGILState_Release().
    // PyEval_InitThreads();
    PyGilManager::evalInitThreads();
    try {
        monitorRequester = epics::pvaClient::PvaClientMonitorRequesterPtr(new ChannelMonitorRequesterImpl(getName(), this));

        monitor = pvaClientChannelPtr->monitor(requestDescriptor, monitorRequester);
    } 
    catch (std::runtime_error& ex) {
        logger.error(ex.what());
        throw PvaException(ex.what());
    }
        
    // If queue length is zero, there is no need for processing thread.
    if (pvObjectQueue.getMaxLength() != 0) {
        startProcessingThread();
    }
}

void Channel::startProcessingThread() 
{
    epics::pvData::Lock lock(processingThreadMutex);
    if (!processingThreadRunning) {
        epicsThreadCreate("ProcessingThread", epicsThreadPriorityHigh, epicsThreadGetStackSize(epicsThreadStackSmall), (EPICSTHREADFUNC)processingThread, this);
    }
    else {
        logger.warn("Processing thread is already running.");
    }
}

void Channel::waitForProcessingThreadExit(double timeout) 
{
    logger.debug("Waiting on processing thread exit, timeout in %f seconds", timeout);
    processingThreadExitEvent.wait(timeout);
}

void Channel::stopMonitor()
{
    epics::pvData::Lock lock(monitorMutex);
    if (!monitorActive) {
        logger.trace("Monitor is not active.");
        return;
    }

    // Processing thread should exit after monitorActive is set to false
    monitorActive = false;
    logger.debug("Stopping monitor");
    monitorRequester->unlisten();
    try {
        monitor->stop();
    } 
    catch (std::runtime_error& ex) {
        logger.error("Caught exception while trying to stop monitor: %s", ex.what());
    }
    pvObjectQueue.cancelWaitForItemPushed();
}

void Channel::processingThread(Channel* channel)
{
    channel->processingThreadRunning = true;
    logger.debug("Started monitor data processing thread %s", epicsThreadGetNameSelf());
    while (true) {
        if (!channel->monitorActive) {
            break;
        }
    
        // Handle possible exceptions while retrieving data from empty queue.
        try {
            PvObject pvObject = channel->pvObjectQueue.frontAndPop(channel->timeout);
            if (!channel->monitorActive) {
                break;
            }
            channel->callSubscribers(pvObject);
        }
        catch (InvalidState& ex) {
            // Queue empty, no PV changes received.
        }
        catch (const std::exception& ex) {
            // Not good.
            logger.error("Monitor data processing thread caught exception: %s", ex.what());
        }
    }

    // Processing thread done.
    logger.debug("Exiting monitor data processing thread %s", epicsThreadGetNameSelf());
    channel->pvObjectQueue.clear();
    channel->notifyProcessingThreadExit();
    channel->processingThreadRunning = false;
}

//
// Monitor data processing interface
//
void Channel::processMonitorData(epics::pvData::PVStructurePtr pvStructurePtr)
{
    if (pvObjectQueue.getMaxLength() == 0) {
        // Process object directly
        try {
            PvObject pvObject(pvStructurePtr);
            callSubscribers(pvObject);
        }
        catch (const std::exception& ex) {
            // Not good.
            logger.error("Exception caught while processing monitor data: %s", ex.what());
        }
    }
    else {
        // Loop until object is queued, or monitor becomes inactive.
        while (true) {
            if (!monitorActive) {
                break;
            }

            if (pvObjectQueue.isFull()) {
                // Cannot queue object
                pvObjectQueue.waitForItemPopped(timeout);
            }
            else {
                // Copy and queue object.
                epics::pvData::PVStructurePtr pvStructurePtr2(epics::pvData::getPVDataCreate()->createPVStructure(pvStructurePtr)); // copy
                PvObject pvObject(pvStructurePtr2);
                pvObjectQueue.pushIfNotFull(pvObject);
                logger.trace("Pushed new monitor element into the queue: %d elements have not been processed.", pvObjectQueue.size());
                break;
            }
        }
    }
}


