// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution


#include "boost/python.hpp"
#include <iostream>
#include <sstream>

#include "Channel.h"
#include "epicsThread.h"
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
#include "PvaClientUtility.h"

#include "GetFieldRequesterImpl.h"

using std::tr1::static_pointer_cast;
namespace pvd = epics::pvData;
namespace pvc = epics::pvaClient;
namespace bp = boost::python;

const char* Channel::DefaultSubscriberName("defaultSubscriber");

const double Channel::DefaultTimeout(3.0);
const int Channel::DefaultMaxPvObjectQueueLength(0);
const double Channel::ShutdownWaitTime(0.1);
const double Channel::MonitorStartWaitTime(0.1);
const double Channel::ThreadStartWaitTime(0.1);

PvaPyLogger Channel::logger("Channel");
PvaClient Channel::pvaClient;
CaClient Channel::caClient;
epics::pvaClient::PvaClientPtr Channel::pvaClientPtr(epics::pvaClient::PvaClient::get("pva ca"));


Channel::Channel(const std::string& channelName, PvProvider::ProviderType providerType_) :
    pvaClientChannelPtr(pvaClientPtr->createChannel(channelName,PvProvider::getProviderName(providerType_))),
    monitorActive(false),
    monitorRunning(false),
    processingThreadRunning(false),
    pvObjectQueue(DefaultMaxPvObjectQueueLength),
    subscriberName(),
    subscriber(),
    subscriberMap(),
    subscriberMutex(),
    monitorMutex(),
    processingThreadMutex(),
    processingThreadExitEvent(),
    timeout(DefaultTimeout),
    providerType(providerType_),
    defaultRequestDescriptor(),
    defaultPutGetRequestDescriptor(),
    isConnected(false),
    hasIssuedConnect(false),
    connectionCallback()
{
    PvObject::initializeBoostNumPy();
    PyGilManager::evalInitThreads();
    stateRequester = epics::pvaClient::PvaClientChannelStateChangeRequesterPtr(new ChannelStateRequesterImpl(isConnected, this));
    pvaClientChannelPtr->setStateChangeRequester(stateRequester);
}

Channel::Channel(const Channel& c) :
    pvaClientChannelPtr(pvaClientPtr->createChannel(c.pvaClientChannelPtr->getChannelName(),PvProvider::getProviderName(c.providerType))),
    monitorActive(false),
    monitorRunning(false),
    processingThreadRunning(false),
    pvObjectQueue(DefaultMaxPvObjectQueueLength),
    subscriberName(),
    subscriber(),
    subscriberMap(),
    subscriberMutex(),
    monitorMutex(),
    processingThreadMutex(),
    processingThreadExitEvent(),
    timeout(DefaultTimeout),
    providerType(c.providerType),
    defaultRequestDescriptor(c.defaultRequestDescriptor),
    defaultPutGetRequestDescriptor(c.defaultPutGetRequestDescriptor),
    isConnected(false),
    connectionCallback()
{
    PyGilManager::evalInitThreads();
    stateRequester = epics::pvaClient::PvaClientChannelStateChangeRequesterPtr(new ChannelStateRequesterImpl(isConnected, this));
    pvaClientChannelPtr->setStateChangeRequester(stateRequester);
}

Channel::~Channel()
{
    stopMonitor();
    pvaClientChannelPtr.reset();
    waitForProcessingThreadExit(ShutdownWaitTime);
    //epicsThreadSleep(ShutdownWaitTime);
}

void Channel::connect()
{
    if (isChannelConnected()) {
        return;
    }

    PyThreadState* _pyThreadState;
    _pyThreadState = PyEval_SaveThread();
    try {
        //pvaClientChannelPtr->connect(timeout);
        issueConnect();
        if (!isChannelConnected()) {
            epics::pvData::Status status = pvaClientChannelPtr->waitConnect(timeout);
            if(!status.isOK()) {
                PyEval_RestoreThread(_pyThreadState);
                throw ChannelTimeout("Channel %s timed out.", pvaClientChannelPtr->getChannelName().c_str());
            }
        }
        determineDefaultRequestDescriptor();
    }
    catch (std::runtime_error&) {
        PyEval_RestoreThread(_pyThreadState);
        throw ChannelTimeout("Channel %s timed out.", pvaClientChannelPtr->getChannelName().c_str());
    }
    PyEval_RestoreThread(_pyThreadState);
}

// asyncConnect will be called inside C++ thread only, so there is no need
// for save/restore thread calls
void Channel::asyncConnect()
{
    if (isChannelConnected()) {
        return;
    }

    try {
        //pvaClientChannelPtr->connect(timeout);
        issueConnect();
        if (!isChannelConnected()) {
            epics::pvData::Status status = pvaClientChannelPtr->waitConnect(timeout);
            if(!status.isOK()) {
                throw ChannelTimeout("Channel %s timed out.", pvaClientChannelPtr->getChannelName().c_str());
            }
        }
        determineDefaultRequestDescriptor();
    }
    catch (std::runtime_error&) {
        throw ChannelTimeout("Channel %s timed out.", pvaClientChannelPtr->getChannelName().c_str());
    }
}

void Channel::issueConnect()
{
    if (hasIssuedConnect) {
        return;
    }
    try {
        hasIssuedConnect = true;
        pvaClientChannelPtr->issueConnect();
    }
    catch (std::runtime_error& ex) {
        logger.warn("Could not issue connect for channel %s: %s.", pvaClientChannelPtr->getChannelName().c_str(), ex.what());
    }
}

bool Channel::isChannelConnected()
{
    return isConnected;
}

PvObject* Channel::get()
{
    return get(PvaConstants::DefaultKey);
}

PvObject* Channel::get(const std::string& requestDescriptor)
{
    connect();
    PyThreadState* _pyThreadState;
    _pyThreadState = PyEval_SaveThread();
    try {
        epics::pvaClient::PvaClientGetPtr pvaGet = createGetPtr(requestDescriptor);
        pvaGet->get();
        epics::pvData::PVStructurePtr pvStructure = pvaGet->getData()->getPVStructure();
        PyEval_RestoreThread(_pyThreadState);
        return new PvObject(pvStructure);
    }
    catch (std::runtime_error& ex) {
        PyEval_RestoreThread(_pyThreadState);
        throw PvaException(ex.what());
    }
}


void Channel::put(const PvObject& pvObject)
{
    put(pvObject, PvaConstants::DefaultKey);
}

void Channel::preparePut(const PvObject& pvObject, pvc::PvaClientPutPtr& pvaPut)
{
    pvd::PVStructurePtr pvSend = pvaPut->getData()->getPVStructure();
    pvd::PVStructurePtr pvSrc = pvObject.getPvStructurePtr();
    bool structuresMatch = false;
    if (*(pvSrc->getStructure()) == *(pvSend->getStructure())) {
        // Structures match
        structuresMatch = true;
        pvSend->copyUnchecked(*pvSrc);
    }
    else {
        // PV Database record fields like PvTimeStamp or PvAlarm
        // are not at the top level
        const pvd::PVFieldPtrArray& pvSendFields = pvSend->getPVFields();
        if (pvSendFields.size() == 1 && pvSendFields[0]->getField()->getType() == pvd::structure) {
            pvd::PVStructurePtr pvSend2 = static_pointer_cast<pvd::PVStructure>(pvSendFields[0]);
            if (*(pvSrc->getStructure()) == *(pvSend2->getStructure())) {
                pvSend2->copyUnchecked(*pvSrc);
                structuresMatch = true;
            }
        }
    }

    if (!structuresMatch) {
        // Try to copy fields that are present both in source and destination
        PyPvDataUtility::copyStructureToStructure2(pvSrc, pvSend);
    }
}

void Channel::put(const PvObject& pvObject, const std::string& requestDescriptor)
{
    connect();
    PyThreadState* _pyThreadState = NULL;
    try {
        pvc::PvaClientPutPtr pvaPut = createPutPtr(requestDescriptor);
        preparePut(pvObject, pvaPut);
        _pyThreadState = PyEval_SaveThread();
        pvaPut->put();
    }
    catch (std::runtime_error& ex) {
        if (_pyThreadState) {
            PyEval_RestoreThread(_pyThreadState);
        }
        throw PvaException(ex.what());
    }
    PyEval_RestoreThread(_pyThreadState);
}

void Channel::put(const std::vector<std::string>& values)
{
    put(values, PvaConstants::DefaultKey);
}

void Channel::put(const std::vector<std::string>& values, const std::string& requestDescriptor)
{
    connect();
    PyThreadState* _pyThreadState = NULL;
    try {
        epics::pvaClient::PvaClientPutPtr pvaPut = createPutPtr(requestDescriptor);
        epics::pvaClient::PvaClientPutDataPtr pvaData = pvaPut->getData();
        pvaData->putStringArray(values);
        _pyThreadState = PyEval_SaveThread();
        pvaPut->put();
    }
    catch (std::runtime_error& ex) {
        if (_pyThreadState) {
            PyEval_RestoreThread(_pyThreadState);
        }
        throw PvaException(ex.what());
    }
    PyEval_RestoreThread(_pyThreadState);
}

void Channel::put(const std::string& value)
{
    put(value, PvaConstants::DefaultKey);
}

void Channel::put(const std::string& value, const std::string& requestDescriptor)
{
    connect();
    PyThreadState* _pyThreadState = NULL;
    try {
        epics::pvaClient::PvaClientPutPtr pvaPut = createPutPtr(requestDescriptor);
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
        _pyThreadState = PyEval_SaveThread();
        pvaPut->put();
    }
    catch (std::runtime_error& ex) {
        if (_pyThreadState) {
            PyEval_RestoreThread(_pyThreadState);
        }
        throw PvaException(ex.what());
    }
    PyEval_RestoreThread(_pyThreadState);
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
    put(pyList, PvaConstants::DefaultKey);
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
    put(value, PvaConstants::DefaultKey);
}

void Channel::put(char value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<int>(static_cast<int>(value)), requestDescriptor);
    //PvaClientUtility::put<char, epics::pvData::PVByte, epics::pvData::PVBytePtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(char value)
{
    put(value, PvaConstants::DefaultKey);
}

void Channel::put(unsigned char value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<int>(static_cast<int>(value)), requestDescriptor);
    //PvaClientUtility::put<unsigned char, epics::pvData::PVUByte, epics::pvData::PVUBytePtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(unsigned char value)
{
    put(value, PvaConstants::DefaultKey);
}

void Channel::put(short value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<short>(value), requestDescriptor);
    //PvaClientUtility::put<short, epics::pvData::PVShort, epics::pvData::PVShortPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(short value)
{
    put(value, PvaConstants::DefaultKey);
}

void Channel::put(unsigned short value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<unsigned short>(value), requestDescriptor);
    //PvaClientUtility::put<unsigned short, epics::pvData::PVUShort, epics::pvData::PVUShortPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(unsigned short value)
{
    put(value, PvaConstants::DefaultKey);
}

void Channel::put(long int value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<int>(value), requestDescriptor);
    //PvaClientUtility::put<int, epics::pvData::PVInt, epics::pvData::PVIntPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(long int value)
{
    put(value, PvaConstants::DefaultKey);
}

void Channel::put(unsigned long int value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<unsigned int>(value), requestDescriptor);
    //PvaClientUtility::put<unsigned int, epics::pvData::PVUInt, epics::pvData::PVUIntPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(unsigned long int value)
{
    put(value, PvaConstants::DefaultKey);
}

void Channel::put(long long value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<long long>(value), requestDescriptor);
    //PvaClientUtility::put<long long, epics::pvData::PVLong, epics::pvData::PVLongPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(long long value)
{
    put(value, PvaConstants::DefaultKey);
}

void Channel::put(unsigned long long value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<unsigned long long>(value), requestDescriptor);
    //PvaClientUtility::put<unsigned long long, epics::pvData::PVULong, epics::pvData::PVULongPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(unsigned long long value)
{
    put(value, PvaConstants::DefaultKey);
}

void Channel::put(float value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<float>(value), requestDescriptor);
    //PvaClientUtility::put<float, epics::pvData::PVFloat, epics::pvData::PVFloatPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(float value)
{
    put(value, PvaConstants::DefaultKey);
}

void Channel::put(double value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<double>(value), requestDescriptor);
    //PvaClientUtility::put<double, epics::pvData::PVDouble, epics::pvData::PVDoublePtr>(pvaClientChannelPtr, value, requestDescriptor);
}

void Channel::put(double value)
{
    put(value, PvaConstants::DefaultKey);
}

void Channel::parsePut(
    const boost::python::list& pyList, const std::string& requestDescriptor,bool zeroArrayLength)
{
    connect();
    PyThreadState* _pyThreadState = NULL;
    int listSize = boost::python::len(pyList);
    std::vector<std::string> args(listSize);
    for (int i = 0; i < listSize; i++) {
        args[i] = PyUtility::extractStringFromPyObject(pyList[i]);
    }
    try {
        epics::pvaClient::PvaClientPutPtr pvaPut = createPutPtr(requestDescriptor);
        epics::pvaClient::PvaClientPutDataPtr pvaData = pvaPut->getData();
        if(zeroArrayLength) pvaData->zeroArrayLength();
        pvaData->parse(args);
        _pyThreadState = PyEval_SaveThread();
        pvaPut->put();
    }
    catch (std::runtime_error& ex) {
        if (_pyThreadState) {
            PyEval_RestoreThread(_pyThreadState);
        }
        throw PvaException(ex.what());
    }
    PyEval_RestoreThread(_pyThreadState);
}


PvObject* Channel::parsePutGet(
    const boost::python::list& pyList, const std::string& requestDescriptor,bool zeroArrayLength)
{
    connect();
    PyThreadState* _pyThreadState = NULL;
    int listSize = boost::python::len(pyList);
    std::vector<std::string> args(listSize);
    for (int i = 0; i < listSize; i++) {
        args[i] = PyUtility::extractStringFromPyObject(pyList[i]);
    }
    try {
        epics::pvaClient::PvaClientPutGetPtr pvaPutGet = createPutGetPtr(requestDescriptor);
        epics::pvaClient::PvaClientPutDataPtr pvaData = pvaPutGet->getPutData();
        if(zeroArrayLength) pvaData->zeroArrayLength();
        pvaData->parse(args);
        _pyThreadState = PyEval_SaveThread();
        pvaPutGet->putGet();
        epics::pvData::PVStructurePtr pvGet = pvaPutGet->getGetData()->getPVStructure();
        PyEval_RestoreThread(_pyThreadState);
        return new PvObject(pvGet);
    }
    catch (std::runtime_error& ex) {
        if (_pyThreadState) {
            PyEval_RestoreThread(_pyThreadState);
        }
        throw PvaException(ex.what());
    }
}


// PutGet methods

PvObject* Channel::putGet(const PvObject& pvObject, const std::string& requestDescriptor)
{
    connect();
    PyThreadState* _pyThreadState = NULL;
    try {
        epics::pvaClient::PvaClientPutGetPtr pvaPutGet = createPutGetPtr(requestDescriptor);
        epics::pvData::PVStructurePtr pvPut = pvaPutGet->getPutData()->getPVStructure();
        pvPut << pvObject;
        _pyThreadState = PyEval_SaveThread();
        pvaPutGet->putGet();
        epics::pvData::PVStructurePtr pvGet = pvaPutGet->getGetData()->getPVStructure();
        PyEval_RestoreThread(_pyThreadState);
        return new PvObject(pvGet);
    }
    catch (std::runtime_error& ex) {
        if (_pyThreadState) {
            PyEval_RestoreThread(_pyThreadState);
        }
        throw PvaException(ex.what());
    }
}

PvObject* Channel::putGet(const PvObject& pvObject)
{
    return putGet(pvObject, PvaConstants::DefaultKey);
}

PvObject* Channel::putGet(const std::vector<std::string>& values, const std::string& requestDescriptor)
{
    connect();
    PyThreadState* _pyThreadState = NULL;
    try {
        epics::pvaClient::PvaClientPutGetPtr pvaPutGet = createPutGetPtr(requestDescriptor);
        epics::pvaClient::PvaClientPutDataPtr pvaData = pvaPutGet->getPutData();
        pvaData->putStringArray(values);
        _pyThreadState = PyEval_SaveThread();
        pvaPutGet->putGet();
        PyEval_RestoreThread(_pyThreadState);
        return new PvObject(pvaPutGet->getGetData()->getPVStructure());
    }
    catch (std::runtime_error& ex) {
        if (_pyThreadState) {
            PyEval_RestoreThread(_pyThreadState);
        }
        throw PvaException(ex.what());
    }
}

PvObject* Channel::putGet(const std::vector<std::string>& values)
{
    return putGet(values, PvaConstants::DefaultKey);
}

PvObject* Channel::putGet(const std::string& value, const std::string& requestDescriptor)
{
    connect();
    PyThreadState* _pyThreadState = NULL;
    try {
        epics::pvaClient::PvaClientPutGetPtr pvaPutGet = createPutGetPtr(requestDescriptor);
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
        _pyThreadState = PyEval_SaveThread();
        pvaPutGet->putGet();
        PyEval_RestoreThread(_pyThreadState);
        return new PvObject(pvaPutGet->getGetData()->getPVStructure());
    }
    catch (std::runtime_error& ex) {
        if (_pyThreadState) {
            PyEval_RestoreThread(_pyThreadState);
        }
        throw PvaException(ex.what());
    }
}

PvObject* Channel::putGet(const std::string& value)
{
    return putGet(value, PvaConstants::DefaultKey);
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
    return putGet(pyList, PvaConstants::DefaultKey);
}

PvObject* Channel::putGet(bool value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString(value), requestDescriptor);
    //return PvaClientUtility::putGet<bool, epics::pvData::PVBoolean, epics::pvData::PVBooleanPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(bool value)
{
    return putGet(value, PvaConstants::DefaultKey);
}

PvObject* Channel::putGet(char value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<int>(static_cast<int>(value)), requestDescriptor);
    //return PvaClientUtility::putGet<char, epics::pvData::PVByte, epics::pvData::PVBytePtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(char value)
{
    return putGet(value, PvaConstants::DefaultKey);
}

PvObject* Channel::putGet(unsigned char value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<int>(static_cast<int>(value)), requestDescriptor);
    //return PvaClientUtility::putGet<unsigned char, epics::pvData::PVUByte, epics::pvData::PVUBytePtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(unsigned char value)
{
    return putGet(value, PvaConstants::DefaultKey);
}

PvObject* Channel::putGet(short value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<short>(value), requestDescriptor);
    //return PvaClientUtility::putGet<short, epics::pvData::PVShort, epics::pvData::PVShortPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(short value)
{
    return putGet(value, PvaConstants::DefaultKey);
}

PvObject* Channel::putGet(unsigned short value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<unsigned short>(value), requestDescriptor);
    //return PvaClientUtility::putGet<unsigned short, epics::pvData::PVUShort, epics::pvData::PVUShortPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(unsigned short value)
{
    return putGet(value, PvaConstants::DefaultKey);
}

PvObject* Channel::putGet(long int value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<int>(value), requestDescriptor);
    //return PvaClientUtility::putGet<int, epics::pvData::PVInt, epics::pvData::PVIntPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(long int value)
{
    return putGet(value, PvaConstants::DefaultKey);
}

PvObject* Channel::putGet(unsigned long int value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<unsigned int>(value), requestDescriptor);
    //return PvaClientUtility::putGet<unsigned int, epics::pvData::PVUInt, epics::pvData::PVUIntPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(unsigned long int value)
{
    return putGet(value, PvaConstants::DefaultKey);
}

PvObject* Channel::putGet(long long value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<long long>(value), requestDescriptor);
    //return PvaClientUtility::putGet<long long, epics::pvData::PVLong, epics::pvData::PVLongPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(long long value)
{
    return putGet(value, PvaConstants::DefaultKey);
}

PvObject* Channel::putGet(unsigned long long value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<unsigned long long>(value), requestDescriptor);
    //return PvaClientUtility::putGet<unsigned long long, epics::pvData::PVULong, epics::pvData::PVULongPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(unsigned long long value)
{
    return putGet(value, PvaConstants::DefaultKey);
}

PvObject* Channel::putGet(float value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<float>(value), requestDescriptor);
    //return PvaClientUtility::putGet<float, epics::pvData::PVFloat, epics::pvData::PVFloatPtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(float value)
{
    return putGet(value, PvaConstants::DefaultKey);
}

PvObject* Channel::putGet(double value, const std::string& requestDescriptor)
{
    return putGet(StringUtility::toString<double>(value), requestDescriptor);
    //return PvaClientUtility::putGet<double, epics::pvData::PVDouble, epics::pvData::PVDoublePtr>(pvaClientChannelPtr, value, requestDescriptor);
}

PvObject* Channel::putGet(double value)
{
    return putGet(value, PvaConstants::DefaultKey);
}

//
// GetPut methods
//
PvObject* Channel::getPut()
{
    return getPut(PvaConstants::DefaultKey);
}

PvObject* Channel::getPut(const std::string& requestDescriptor)
{
    connect();
    PyThreadState* _pyThreadState = NULL;
    try {
        epics::pvaClient::PvaClientPutGetPtr pvaPutGet = createPutGetPtr(requestDescriptor);
        _pyThreadState = PyEval_SaveThread();
        pvaPutGet->getPut();
        PyEval_RestoreThread(_pyThreadState);
        return new PvObject(pvaPutGet->getPutData()->getPVStructure());
    }
    catch (std::runtime_error& ex) {
        if (_pyThreadState) {
            PyEval_RestoreThread(_pyThreadState);
        }
        throw PvaException(ex.what());
    }
}

//
// Monitor methods
//
void Channel::subscribe(const std::string& subscriberName, const boost::python::object& pySubscriber)
{
    epics::pvData::Lock lock(subscriberMutex);

    // Subscriber map holds subscribers if there are more than 1. Otherwise,
    // we do not use it.
    if (!this->subscriberName.size() && subscriberMap.size() == 0) {
        // Single subscriber.
        this->subscriberName = subscriberName;
        this->subscriber = pySubscriber;
    }
    else {
        if (this->subscriberName.size() > 0) {
            // Second subscriber; start using map.
            if (this->subscriberName == subscriberName) {
                throw ObjectAlreadyExists("Subscriber " + subscriberName + " is already registered for channel " + getName() + ".");
            }
            subscriberMap[this->subscriberName] = this->subscriber;
            this->subscriberName = "";
            subscriberMap[subscriberName] = pySubscriber;
        }
        else {
            // More than one subscribers already
            std::map<std::string,boost::python::object>::const_iterator iterator = subscriberMap.find(subscriberName);
            if (iterator != subscriberMap.end()) {
                throw ObjectAlreadyExists("Subscriber " + subscriberName + " is already registered for channel " + getName() + ".");
            }
            subscriberMap[subscriberName] = pySubscriber;
        }
    }
    logger.trace("Subscribed " + subscriberName + " to monitor channel " + getName() + ".");

    //boost::python::incref(pySubscriber.ptr());
    //PyObject* pyObject = pySubscriber.ptr();
    //int refCount = pyObject->ob_refcnt;
    //boost::python::decref(pySubscriber.ptr());
}

void Channel::unsubscribe(const std::string& subscriberName)
{
    epics::pvData::Lock lock(subscriberMutex);

    // Subscriber map holds subscribers if there are more than 1. Otherwise,
    // we do not use it.
    if (this->subscriberName.size()) {
        // Single subscriber.
        if (this->subscriberName != subscriberName) {
            throw ObjectNotFound("Subscriber " + subscriberName + " is not registered for channel " + getName() + ".");
        }
        this->subscriberName = "";
    }
    else {
        // More than 1 subscriber.
        std::map<std::string,boost::python::object>::const_iterator iterator = subscriberMap.find(subscriberName);
        if (iterator == subscriberMap.end()) {
            throw ObjectNotFound("Subscriber " + subscriberName + " is not registered for channel " + getName() + ".");
        }
        boost::python::object pySubscriber = subscriberMap[subscriberName];
        subscriberMap.erase(subscriberName);
    }
    logger.trace("Unsubscribed " + subscriberName + " from channel " + getName() + ".");

    // If there is only one subscriber left, stop using map.
    if (subscriberMap.size() == 1) {
        std::map<std::string,boost::python::object>::iterator it = subscriberMap.begin();
        this->subscriberName = it->first;
        this->subscriber = it->second;
        subscriberMap.erase(this->subscriberName);
    }
}

void Channel::callSubscribers(PvObject& pvObject)
{
    std::string pySubscriberName = this->subscriberName;
    if (pySubscriberName.size()) {
        // Single subscriber
        boost::python::object pySubscriber = this->subscriber;
        callSubscriber(pySubscriberName, pySubscriber, pvObject);
    }
    else {
        // Multiple subscribers
        std::map<std::string,boost::python::object> subscriberMap2;
        {
            // Cannot lock entire call, as subscribers may be added/deleted at
            // any time, causing deadlock
            epics::pvData::Lock lock(subscriberMutex);
            subscriberMap2 = subscriberMap;
        }

        std::map<std::string,boost::python::object>::iterator mIter;
        for (mIter = subscriberMap2.begin(); mIter != subscriberMap2.end(); mIter++) {
            std::string pySubscriberName = mIter->first;
            boost::python::object pySubscriber = mIter->second;
            callSubscriber(pySubscriberName, pySubscriber, pvObject);
        }
    }
}

void Channel::callSubscriber(const std::string& pySubscriberName, boost::python::object& pySubscriber, PvObject& pvObject)
{
    // Acquire GIL. This is required because callSubscribers()
    // is called in a monitoring thread. Before monitoring thread
    // is created, one must call PyEval_InitThreads() in the main thread
    // to initialize things properly. If this is not done, code will
    // most likely crash while invoking python from c++, or while
    // attempting to release GIL.
    // PyGILState_STATE gilState = PyGILState_Ensure();
    // logger.trace("Acquiring python GIL for subscriber " + pySubscriberName);
    PyGilManager::gilStateEnsure();

    // Call python code
    try {
        pySubscriber(pvObject);
    }
    catch(const boost::python::error_already_set&) {
        logger.error("Channel subscriber " + pySubscriberName + " raised python exception.");
        PyErr_Print();
        PyErr_Clear();
    }
    catch (const std::exception& ex) {
        logger.error(ex.what());
    }

    // Release GIL.
    // PyGILState_Release(gilState);
    // logger.trace("Releasing python GIL after processing monitor data with " + pySubscriberName);
    PyGilManager::gilStateRelease();
}

void Channel::callConnectionCallback(bool isConnected)
{
    PyGilManager::gilStateEnsure();
    try {
        connectionCallback(isConnected);
    }
    catch(const boost::python::error_already_set&) {
        logger.error("Connection callback raised python exception.");
        PyErr_Print();
        PyErr_Clear();
    }
    catch (const std::exception& ex) {
        logger.error(ex.what());
    }
    PyGilManager::gilStateRelease();
}

void Channel::asyncGet(const bp::object& pyCallback, const bp::object& pyErrorCallback)
{
    asyncGet(pyCallback, pyErrorCallback, PvaConstants::DefaultKey);
}

void Channel::asyncGet(const bp::object& pyCallback, const bp::object& pyErrorCallback, const std::string& requestDescriptor)
{
    asyncGetPyCallback = pyCallback;
    asyncGetPyErrorCallback = pyErrorCallback;
    asyncGetRequestDescriptor = requestDescriptor;
    epicsThreadCreate("AsyncGetThread", epicsThreadPriorityHigh, epicsThreadGetStackSize(epicsThreadStackSmall), (EPICSTHREADFUNC)asyncGetThread, this);
}

void Channel::asyncGetThread(Channel* channel)
{
    bp::object pyCallback = channel->asyncGetPyCallback;
    bp::object pyErrorCallback = channel->asyncGetPyErrorCallback;
    std::string requestDescriptor = channel->asyncGetRequestDescriptor;
    try {
        channel->asyncConnect();
        pvc::PvaClientGetPtr pvaGet = channel->createGetPtr(requestDescriptor);
        pvaGet->get();
        pvd::PVStructurePtr pvStructure = pvaGet->getData()->getPVStructure();
        PvObject pvObject(pvStructure);
        channel->invokePyCallback(pyCallback, pvObject);
    }
    catch (const std::exception& ex) {
        if (!PyUtility::isPyNone(pyErrorCallback)) {
            channel->invokePyCallback(pyErrorCallback, ex.what());
        }
        else {
            logger.error(ex.what());
        }
    }
}

void Channel::asyncPut(const PvObject& pvObject, const bp::object& pyCallback, const bp::object& pyErrorCallback)
{
    asyncPut(pvObject, pyCallback, pyErrorCallback, PvaConstants::DefaultKey);
}

void Channel::asyncPut(const PvObject& pvObject, const bp::object& pyCallback, const bp::object& pyErrorCallback, const std::string& requestDescriptor)
{
    asyncPutPyCallback = pyCallback;
    asyncPutPyErrorCallback = pyErrorCallback;
    try {
        asyncPvaPut = createPutPtr(requestDescriptor);
        preparePut(pvObject, asyncPvaPut);
    }
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
    epicsThreadCreate("AsyncPutThread", epicsThreadPriorityHigh, epicsThreadGetStackSize(epicsThreadStackSmall), (EPICSTHREADFUNC)asyncPutThread, this);
}

void Channel::asyncPutThread(Channel* channel)
{
    bp::object pyCallback = channel->asyncPutPyCallback;
    bp::object pyErrorCallback = channel->asyncPutPyErrorCallback;
    try {
        channel->asyncConnect();
        channel->asyncPvaPut->put();
        pvd::PVStructurePtr pvStructure = channel->asyncPvaPut->getData()->getPVStructure();
        PvObject pvObject(pvStructure);
        channel->invokePyCallback(pyCallback, pvObject);
    }
    catch (const std::exception& ex) {
        if (!PyUtility::isPyNone(pyErrorCallback)) {
            channel->invokePyCallback(pyErrorCallback, ex.what());
        }
        else {
            logger.error(ex.what());
        }
    }
}

void Channel::invokePyCallback(boost::python::object& pyCallback, PvObject& pvObject)
{
    if(PyUtility::isPyNone(pyCallback)) {
        return;
    }
    PyGilManager::gilStateEnsure();
    try {
        pyCallback(pvObject);
    }
    catch(const boost::python::error_already_set&) {
        logger.error("Python callback raised exception.");
        PyErr_Print();
        PyErr_Clear();
    }
    catch (const std::exception& ex) {
        logger.error(ex.what());
    }
    PyGilManager::gilStateRelease();
}

void Channel::invokePyCallback(boost::python::object& pyCallback, std::string errorMsg)
{
    if(PyUtility::isPyNone(pyCallback)) {
        return;
    }
    PyGilManager::gilStateEnsure();
    try {
        pyCallback(errorMsg);
    }
    catch(const boost::python::error_already_set&) {
        logger.error("Python callback raised exception.");
        PyErr_Print();
        PyErr_Clear();
    }
    catch (const std::exception& ex) {
        logger.error(ex.what());
    }
    PyGilManager::gilStateRelease();
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
    startMonitor(defaultRequestDescriptor);
}

void Channel::startMonitor(const std::string& requestDescriptor)
{
    epics::pvData::Lock lock(monitorMutex);
    if (monitorActive) {
        logger.warn("Monitor is already active.");
        return;
    }

    // One must call PyEval_InitThreads() in the main thread
    // to initialize thread state, which is needed for proper functioning
    // of PyGILState_Ensure()/PyGILState_Release().
    // PyEval_InitThreads();
    PyGilManager::evalInitThreads();
    this->monitorRequestDescriptor = requestDescriptor;

    // If queue length is zero, there is no need for processing thread.
    if (pvObjectQueue.getMaxLength() != 0) {
        startProcessingThread();
    }

    // Issue connect if channel is not connected and onChannelConnect() will
    // be called when channel gets connected; otherwise,
    // call onChannelConnect()
    monitorActive = true;
    if (isChannelConnected()) {
        onChannelConnect();
    }
    else {
        issueConnect();
    }
}

void Channel::monitor(const boost::python::object& pySubscriber, const std::string& requestDescriptor)
{
    // Unsubscribe default subscriber.
    try {
        unsubscribe(DefaultSubscriberName);
    }
    catch (ObjectNotFound&) {
        // ok
    }

    subscribe(DefaultSubscriberName, pySubscriber);
    if (requestDescriptor == PvaConstants::DefaultKey) {
        startMonitor();
    }
    else {
        startMonitor(requestDescriptor);
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

void Channel::startIssueConnectThread()
{
    if (hasIssuedConnect) {
        return;
    }
    epicsThreadCreate("IssueConnectThread", epicsThreadPriorityHigh, epicsThreadGetStackSize(epicsThreadStackSmall), (EPICSTHREADFUNC)issueConnectThread, this);
}

void Channel::waitForProcessingThreadExit(double timeout)
{
    if (processingThreadRunning) {
        logger.debug("Waiting on processing thread exit, timeout in %f seconds", timeout);
        processingThreadExitEvent.wait(timeout);
    }
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
    monitorRunning = false;
    logger.debug("Stopping monitor");
    if (pvaClientMonitorRequesterPtr) {
        pvaClientMonitorRequesterPtr->unlisten();
    }
    if (pvaClientMonitorPtr) {
        try {
            pvaClientMonitorPtr->stop();
        }
        catch (std::runtime_error& ex) {
            logger.error("Caught exception while trying to stop monitor: %s", ex.what());
        }
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

void Channel::issueConnectThread(Channel* channel)
{
    logger.debug("About to issue channel connect in a thread %s", epicsThreadGetNameSelf());
    channel->callConnectionCallback(false);
    channel->issueConnect();
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

void Channel::onChannelConnect()
{
    logger.debug("On channel connect called for %s", getName().c_str());
    if (monitorActive && !monitorRunning) {
        try {
            pvaClientMonitorRequesterPtr = epics::pvaClient::PvaClientMonitorRequesterPtr(new ChannelMonitorRequesterImpl(getName(), this));

            //pvaClientMonitorPtr = pvaClientChannelPtr->monitor(monitorRequestDescriptor, pvaClientMonitorRequesterPtr);
            //pvaClientMonitorPtr = epics::pvaClient::PvaClientMonitor::create(pvaClientPtr, getName(), PvProvider::getProviderName(providerType), monitorRequestDescriptor, epics::pvaClient::PvaClientChannelStateChangeRequesterPtr(), pvaClientMonitorRequesterPtr);
            pvaClientMonitorPtr =  pvaClientChannelPtr->createMonitor(monitorRequestDescriptor);
            pvaClientMonitorPtr->setRequester(pvaClientMonitorRequesterPtr);
            pvaClientMonitorPtr->issueConnect();
            monitorRunning = true;
        }
        catch (PvaException& ex) {
            monitorActive = false;
            logger.error(ex.what());
        }
        catch (std::runtime_error& ex) {
            monitorActive = false;
            logger.error(ex.what());
        }
    }

    // Call user callback
    if(!PyUtility::isPyNone(connectionCallback)) {
        callConnectionCallback(true);
    }
}

void Channel::onChannelDisconnect()
{
    logger.debug("On channel disconnect called for %s", getName().c_str());
    // Call user callback
    if(!PyUtility::isPyNone(connectionCallback)) {
        callConnectionCallback(false);
    }
}

// Introspection
boost::python::dict Channel::getIntrospectionDict()
{
    connect();
    epics::pvAccess::Channel::shared_pointer channelPtr = pvaClientChannelPtr->getChannel();
    std::tr1::shared_ptr<GetFieldRequesterImpl> getFieldRequesterImpl;
    getFieldRequesterImpl.reset(new GetFieldRequesterImpl(channelPtr));
    channelPtr->getField(getFieldRequesterImpl, "");

    if (!getFieldRequesterImpl->waitUntilFieldGet(timeout)) {
        throw ChannelTimeout("Channel %s field get timed out", getName().c_str());
    }

    if (!getFieldRequesterImpl.get()) {
        throw PvaException("Failed to get introspection data for channel %s", getName().c_str());
    }

    epics::pvData::Structure::const_shared_pointer structurePtr =
        std::tr1::dynamic_pointer_cast<const epics::pvData::Structure>(getFieldRequesterImpl->getField());

    boost::python::dict pyDict;
    PyPvDataUtility::structureToPyDict(structurePtr, pyDict);
    return pyDict;
}

void Channel::determineDefaultRequestDescriptor()
{
    if (defaultRequestDescriptor.size()) {
        return;
    }

    epics::pvAccess::Channel::shared_pointer channelPtr = pvaClientChannelPtr->getChannel();
    std::tr1::shared_ptr<GetFieldRequesterImpl> getFieldRequesterImpl;
    getFieldRequesterImpl.reset(new GetFieldRequesterImpl(channelPtr));
    channelPtr->getField(getFieldRequesterImpl, "");

    if (!getFieldRequesterImpl->waitUntilFieldGet(timeout)) {
        throw ChannelTimeout("Channel %s field get timed out", getName().c_str());
    }

    if (!getFieldRequesterImpl.get()) {
        throw PvaException("Failed to get introspection data for channel %s", getName().c_str());
    }

    epics::pvData::Structure::const_shared_pointer structurePtr =
        std::tr1::dynamic_pointer_cast<const epics::pvData::Structure>(getFieldRequesterImpl->getField());

    epics::pvData::FieldConstPtr fieldPtr = structurePtr->getField(PvaConstants::ValueFieldKey);
    if (!fieldPtr) {
        defaultRequestDescriptor = PvaConstants::AllFieldsRequest;
        defaultPutGetRequestDescriptor = PvaConstants::PutGetAllFieldsRequest;
    }
    else {
        defaultRequestDescriptor = PvaConstants::FieldValueRequest;
        defaultPutGetRequestDescriptor = PvaConstants::PutGetFieldValueRequest;
    }
}

epics::pvaClient::PvaClientGetPtr Channel::createGetPtr(const std::string& requestDescriptor)
{
    if (requestDescriptor == PvaConstants::DefaultKey) {
        return pvaClientChannelPtr->createGet(defaultRequestDescriptor);
    }
    else {
        return pvaClientChannelPtr->createGet(requestDescriptor);
    }
}

epics::pvaClient::PvaClientPutPtr Channel::createPutPtr(const std::string& requestDescriptor)
{
    if (requestDescriptor == PvaConstants::DefaultKey) {
        return pvaClientChannelPtr->createPut(defaultRequestDescriptor);
    }
    else {
        return pvaClientChannelPtr->createPut(requestDescriptor);
    }
}

epics::pvaClient::PvaClientPutGetPtr Channel::createPutGetPtr(const std::string& requestDescriptor)
{
    if (requestDescriptor == PvaConstants::DefaultKey) {
        return pvaClientChannelPtr->createPutGet(defaultPutGetRequestDescriptor);
    }
    else {
        return pvaClientChannelPtr->createPutGet(requestDescriptor);
    }
}

void Channel::setConnectionCallback(const boost::python::object& callback)
{
    connectionCallback = callback;
    startIssueConnectThread();
    epicsThreadSleep(ThreadStartWaitTime);
}
