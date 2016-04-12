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
#include "PyPvDataUtility.h"
#include "PvaConstants.h"
#include "PvaClientUtility.h"

const char* Channel::DefaultRequestDescriptor("field(value)");
const char* Channel::DefaultPutGetRequestDescriptor("putField(value)getField(value)");
const double Channel::DefaultTimeout(3.0);
const double Channel::ShutdownWaitTime(0.1);

PvaPyLogger Channel::logger("Channel");
PvaClient Channel::pvaClient;
CaClient Channel::caClient;
epics::pvaClient::PvaClientPtr Channel::pvaClientPtr(epics::pvaClient::PvaClient::create());


Channel::Channel(const std::string& channelName, PvProvider::ProviderType providerType) :
    pvaClientChannelPtr(pvaClientPtr->createChannel(channelName,PvProvider::getProviderName(providerType))),
    pvObjectMonitorQueue(),
    shutdownThreads(true),
    monitorThreadDone(true),
    processingThreadDone(true),
    subscriberMap(),
    subscriberMutex(),
    monitorElementProcessingMutex(),
    monitorThreadMutex(),
    timeout(DefaultTimeout)
{
    connect();
}
    
Channel::Channel(const Channel& c) :
    pvaClientChannelPtr(c.pvaClientChannelPtr),
    pvObjectMonitorQueue(),
    shutdownThreads(true),
    monitorThreadDone(true),
    processingThreadDone(true),
    subscriberMap(),
    subscriberMutex(),
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
    catch (std::runtime_error e) {
        throw PvaException(e.what());
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
    catch (std::runtime_error e) {
        throw PvaException(e.what());
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
        epics::pvData::PVScalarPtr pvScalar = pvaData->getScalarValue();
        epics::pvData::getConvert()->fromString(pvScalar,value);
        pvaPutGet->putGet();
        return new PvObject(pvaPutGet->getGetData()->getPVStructure());
    } 
    catch (std::runtime_error e) {
        throw PvaException(e.what());
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
    catch (std::runtime_error e) {
        throw PvaException(e.what());
    }
}

//
// Monitor methods
//
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
            PyErr_Print();
            PyErr_Clear();
            logger.error("Channel subscriber " + subscriberName + " throws python exception.  unsubscribing.");
            unsubscribe(subscriberName);
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
    if (shutdownThreads == true) {

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
        shutdownThreads = false;
        epicsThreadCreate("ChannelMonitorThread", epicsThreadPriorityLow, epicsThreadGetStackSize(epicsThreadStackSmall), (EPICSTHREADFUNC)monitorThread, this);
        epicsThreadCreate("ChannelProcessingThread", epicsThreadPriorityHigh, epicsThreadGetStackSize(epicsThreadStackSmall), (EPICSTHREADFUNC)processingThread, this);
    }
}

void Channel::stopMonitor()
{
    epics::pvData::Lock lock(monitorThreadMutex);
    if (shutdownThreads == true) {
        logger.debug("Monitor thread is not running");
        return;
    }
    logger.debug("Stopping monitor");
    try {
        pvaClientMonitorPtr->stop();
    } 
    catch (std::runtime_error e) {
        logger.error(e.what());
        throw PvaException(e.what());
    }
    shutdownThreads = true;

    while (monitorThreadDone == false || processingThreadDone == false) {
        PyThreadState *state = PyEval_SaveThread();
        // Give threads GIL and time to evaluate shutdownThreads
        PyEval_RestoreThread(state);
    }
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
    channel->monitorThreadDone = false;
    logger.debug("Started monitor thread %s", epicsThreadGetNameSelf());
    epics::pvaClient::PvaClientMonitorPtr monitor = channel->getMonitor();
    epics::pvaClient::PvaClientMonitorDataPtr pvaData = monitor->getData();
    epics::pvData::PVDataCreatePtr create(epics::pvData::getPVDataCreate());
    while (channel->shutdownThreads == false) {
        monitor->waitEvent();
        epics::pvData::PVStructurePtr C(create->createPVStructure(pvaData->getPVStructure())); // copy
        PvObject pvObject(C);
        channel->queueMonitorData(pvObject);
        monitor->releaseEvent();
    }
    logger.debug("Exiting monitor thread %s", epicsThreadGetNameSelf());
    channel->monitorThreadDone = true;
}


void Channel::processingThread(Channel* channel)
{
    channel->processingThreadDone = false;
    logger.debug("Started processing thread %s", epicsThreadGetNameSelf());
    epics::pvaClient::PvaClientMonitorPtr monitor = channel->getMonitor();
    epics::pvaClient::PvaClientMonitorDataPtr pvaData = monitor->getData();
    while (channel->shutdownThreads == false) {
        channel->processMonitorElement();
    }
    logger.debug("Exiting processing thread %s", epicsThreadGetNameSelf());
    channel->processingThreadDone = true;
}

epics::pvaClient::PvaClientMonitorPtr Channel::getMonitor() 
{
    return pvaClientChannelPtr->monitor("");
}

void Channel::queueMonitorData(PvObject& pvObject) 
{
    pvObjectMonitorQueue.push(pvObject);
}

