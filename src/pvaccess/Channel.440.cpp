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

Channel::Channel(const std::string& channelName, PvProvider::ProviderType providerType) :
    requester(new RequesterImpl(channelName)),
    requesterImpl(new ChannelRequesterImpl(true)),
    channelGetRequester(channelName),
    provider(epics::pvAccess::getChannelProviderRegistry()->getProvider(PvProvider::getProviderName(providerType))),
    channel(provider->createChannel(channelName, requesterImpl)),
    monitorRequester(new ChannelMonitorRequesterImpl(channelName)),
    monitor(),
    monitorThreadDone(true),
    subscriberMap(),
    subscriberMutex(),
    monitorElementProcessingMutex(),
    monitorThreadMutex(),
    monitorThreadExitEvent(),
    timeout(DefaultTimeout)
{
}
    
Channel::Channel(const Channel& c) :
    channelGetRequester(c.channelGetRequester),
    channel(c.channel),
    monitorRequester(c.monitorRequester),
    monitorThreadDone(true),
    subscriberMap(),
    subscriberMutex(),
    monitorElementProcessingMutex(),
    monitorThreadMutex(),
    monitorThreadExitEvent(),
    timeout(DefaultTimeout)
{
}

Channel::~Channel()
{
    stopMonitor();
    channel->destroy();
    epicsThreadSleep(ShutdownWaitTime);
}
 
PvObject* Channel::get()
{
    return get(DefaultRequestDescriptor);
}

PvObject* Channel::get(const std::string& requestDescriptor) 
{
    epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvData::CreateRequest::create()->createRequest(requestDescriptor);

    std::tr1::shared_ptr<ChannelRequesterImpl> channelRequesterImpl = std::tr1::dynamic_pointer_cast<ChannelRequesterImpl>(channel->getChannelRequester());

    if (channel->getConnectionState() != epics::pvAccess::Channel::CONNECTED) {
        if (!channelRequesterImpl->waitUntilConnected(timeout)) {
            throw ChannelTimeout("Channel %s timed out", channel->getChannelName().c_str());
        }
    }

    std::tr1::shared_ptr<GetFieldRequesterImpl> getFieldRequesterImpl;
    getFieldRequesterImpl.reset(new GetFieldRequesterImpl(channel));
    channel->getField(getFieldRequesterImpl, "");

    if (!getFieldRequesterImpl->waitUntilFieldGet(timeout)) {
        throw ChannelTimeout("Channel %s field get timed out", channel->getChannelName().c_str());
    }
            
    if (!getFieldRequesterImpl.get()) {
        throw PvaException("Failed to get introspection data for channel %s", channel->getChannelName().c_str());
    }

	epics::pvData::Structure::const_shared_pointer structure =
	    std::tr1::dynamic_pointer_cast<const epics::pvData::Structure>(getFieldRequesterImpl->getField());
	if (structure.get() == 0 || structure->getField("value").get() == 0) {
	    // fallback to structure
		pvRequest = epics::pvData::CreateRequest::create()->createRequest("field()");
    }

	std::tr1::shared_ptr<ChannelGetRequesterImpl> getRequesterImpl(new ChannelGetRequesterImpl(channel->getChannelName()));
	epics::pvAccess::ChannelGet::shared_pointer channelGet = channel->createChannelGet(getRequesterImpl, pvRequest);
	if (getRequesterImpl->waitUntilGet(timeout)) {
	    return new PvObject(getRequesterImpl->getPVStructure());
    }
    throw ChannelTimeout("Channel %s get request timed out", channel->getChannelName().c_str());
}

void Channel::put(const PvObject& pvObject)
{
    put(pvObject, DefaultRequestDescriptor);
}

void Channel::put(const PvObject& pvObject, const std::string& requestDescriptor) 
{
    epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvData::CreateRequest::create()->createRequest(requestDescriptor);
    std::tr1::shared_ptr<ChannelRequesterImpl> channelRequesterImpl = std::tr1::dynamic_pointer_cast<ChannelRequesterImpl>(channel->getChannelRequester());

    if (channel->getConnectionState() != epics::pvAccess::Channel::CONNECTED) {
        if (!channelRequesterImpl->waitUntilConnected(timeout)) {
            throw ChannelTimeout("Channel %s timed out", channel->getChannelName().c_str());
        }
    }

	std::tr1::shared_ptr<ChannelPutRequesterImpl> putRequesterImpl(new ChannelPutRequesterImpl(channel->getChannelName()));
	epics::pvAccess::ChannelPut::shared_pointer channelPut = channel->createChannelPut(putRequesterImpl, pvRequest);
	if (putRequesterImpl->waitUntilDone(timeout)) {
        epics::pvData::PVStructurePtr pvStructurePtr = putRequesterImpl->getStructure();
        pvStructurePtr << pvObject;
        putRequesterImpl->resetEvent();
        channelPut->put(pvStructurePtr, putRequesterImpl->getBitSet());
	    if (putRequesterImpl->waitUntilDone(timeout)) {
	        return;
        }
    }
    throw ChannelTimeout("Channel %s put request timed out", channel->getChannelName().c_str());
}

void Channel::put(const std::vector<std::string>& values)
{
    put(values, DefaultRequestDescriptor);
}

void Channel::put(const std::vector<std::string>& values, const std::string& requestDescriptor) 
{
    epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvData::CreateRequest::create()->createRequest(requestDescriptor);
    std::tr1::shared_ptr<ChannelRequesterImpl> channelRequesterImpl = std::tr1::dynamic_pointer_cast<ChannelRequesterImpl>(channel->getChannelRequester());

    if (channel->getConnectionState() != epics::pvAccess::Channel::CONNECTED) {
        if (!channelRequesterImpl->waitUntilConnected(timeout)) {
            throw ChannelTimeout("Channel %s timed out", channel->getChannelName().c_str());
        }
    }

	std::tr1::shared_ptr<ChannelPutRequesterImpl> putRequesterImpl(new ChannelPutRequesterImpl(channel->getChannelName()));
	epics::pvAccess::ChannelPut::shared_pointer channelPut = channel->createChannelPut(putRequesterImpl, pvRequest);
	if (putRequesterImpl->waitUntilDone(timeout)) {
        epics::pvData::PVStructurePtr pvStructurePtr = putRequesterImpl->getStructure();
        PvUtility::fromString(pvStructurePtr, values);

        putRequesterImpl->resetEvent();
        channelPut->put(pvStructurePtr, putRequesterImpl->getBitSet());
	    if (putRequesterImpl->waitUntilDone(timeout)) {
	        return;
        }
    }
    throw ChannelTimeout("Channel %s put request timed out", channel->getChannelName().c_str());
}

void Channel::put(const std::string& value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(const std::string& value, const std::string& requestDescriptor) 
{
    std::vector<std::string> values;
    values.push_back(value);
    put(values, requestDescriptor);
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
    put(StringUtility::toString(value), requestDescriptor);
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

void Channel::put(long int value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<long int>(value), requestDescriptor);
}

void Channel::put(long int value)
{
    put(value, DefaultRequestDescriptor);
}

void Channel::put(unsigned long int value, const std::string& requestDescriptor)
{
    put(StringUtility::toString<unsigned long int>(value), requestDescriptor);
}

void Channel::put(unsigned long int value)
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

// PutGet methods are not supported

PvObject* Channel::putGet(const PvObject& pvObject, const std::string& requestDescriptor) 
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(const PvObject& pvObject)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(const std::vector<std::string>& values, const std::string& requestDescriptor) 
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(const std::vector<std::string>& values)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(const std::string& value, const std::string& requestDescriptor) 
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(const std::string& value)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(const boost::python::list& pyList, const std::string& requestDescriptor) 
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(const boost::python::list& pyList)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(bool value, const std::string& requestDescriptor)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(bool value)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(char value, const std::string& requestDescriptor)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(char value)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(unsigned char value, const std::string& requestDescriptor)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(unsigned char value)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(short value, const std::string& requestDescriptor)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(short value)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(unsigned short value, const std::string& requestDescriptor)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(unsigned short value)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(long int value, const std::string& requestDescriptor)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(long int value)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(unsigned long int value, const std::string& requestDescriptor)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(unsigned long int value)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(long long value, const std::string& requestDescriptor)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(long long value)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(unsigned long long value, const std::string& requestDescriptor)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(unsigned long long value)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(float value, const std::string& requestDescriptor)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(float value)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(double value, const std::string& requestDescriptor)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::putGet(double value)
{
    throw PvaException("PutGet() is not supported for 4.4.0 release.");
}

PvObject* Channel::getPut()
{
    throw PvaException("GetPut() is not supported for 4.4.0 release.");
}

PvObject* Channel::getPut(const std::string& requestDescriptor)
{
    throw PvaException("GetPut() is not supported for 4.4.0 release.");
}

ChannelMonitorRequesterImpl* Channel::getMonitorRequester()
{
    return static_cast<ChannelMonitorRequesterImpl*>(monitorRequester.get());
}

void Channel::subscribe(const std::string& subscriberName, const boost::python::object& pySubscriber)
{
    epics::pvData::Lock lock(subscriberMutex);
    subscriberMap[subscriberName] = pySubscriber;
}

void Channel::unsubscribe(const std::string& subscriberName)
{
    epics::pvData::Lock lock(subscriberMutex);
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
    std::map<std::string,boost::python::object> subscriberMap2;
    {
        epics::pvData::Lock lock(subscriberMutex);
        subscriberMap2 = subscriberMap;
    }


    std::map<std::string,boost::python::object>::iterator iter;
    for (iter = subscriberMap2.begin(); iter != subscriberMap2.end(); iter++) {
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
    if (monitorThreadDone) {
        monitorThreadDone = false;
        int maxQueueLength = getMonitorRequester()->getPvObjectQueueMaxLength(); 
        monitorRequester = epics::pvData::MonitorRequester::shared_pointer(new ChannelMonitorRequesterImpl(getName()));
        getMonitorRequester()->setPvObjectQueueMaxLength(maxQueueLength); 

        // One must call PyEval_InitThreads() in the main thread
        // to initialize thread state, which is needed for proper functioning
        // of PyGILState_Ensure()/PyGILState_Release().
        // PyEval_InitThreads();
        PyGilManager::evalInitThreads();
        epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvData::CreateRequest::create()->createRequest(requestDescriptor);
        monitor = channel->createMonitor(monitorRequester, pvRequest);
        epicsThreadCreate("ChannelMonitorThread", epicsThreadPriorityLow, epicsThreadGetStackSize(epicsThreadStackSmall), (EPICSTHREADFUNC)monitorThread, this);
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
    monitor->stop();
    logger.debug("Monitor stopped, waiting for thread exit");
    ChannelMonitorRequesterImpl* monitorRequester = getMonitorRequester();
    logger.debug("Stopping requester");
    monitorRequester->stop();
    monitorThreadExitEvent.wait(getTimeout());
}

bool Channel::isMonitorThreadDone() const
{
    return monitorThreadDone;
}

bool Channel::processMonitorElement() 
{
    //epics::pvData::Lock lock(monitorElementProcessingMutex);
    if (monitorThreadDone) {
        return true;
    }

    // Handle possible exceptions while retrieving data from empty queue.
    try {
        PvObject pvObject = getMonitorRequester()->getQueuedPvObject(getTimeout());
        callSubscribers(pvObject);
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
    while (true) {
        if (channel->processMonitorElement()) {
            break;
        }
    }
    logger.debug("Exiting monitor thread %s", epicsThreadGetNameSelf());
    channel->notifyMonitorThreadExit();
}


