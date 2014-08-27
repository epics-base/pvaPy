#include <iostream>
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
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
    provider(epics::pvAccess::getChannelAccess()->getProvider(PvProvider::getProviderName(providerType))),
#else
    provider(epics::pvAccess::getChannelProviderRegistry()->getProvider(PvProvider::getProviderName(providerType))),
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
    channel(provider->createChannel(channelName, requesterImpl)),
    monitorRequester(new ChannelMonitorRequesterImpl(channelName)),
    monitorThreadDone(true),
    pvObjectMonitorQueue(),
    subscriberMap(),
    subscriberMutex(),
    timeout(DefaultTimeout)
{
}
    
Channel::Channel(const Channel& c) :
    channelGetRequester(c.channelGetRequester),
    channel(c.channel),
    monitorRequester(c.monitorRequester),
    monitorThreadDone(true),
    pvObjectMonitorQueue(),
    subscriberMap(),
    subscriberMutex(),
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
    // API change for getCreateRequest()
    // old: epics::pvAccess::getCreateRequest()->createRequest(xxx,requester);
    // new: epics::pvAccess::CreateRequest::create()->createRequest(xxx); 
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
    epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvAccess::getCreateRequest()->createRequest(requestDescriptor, requester);
#else
    epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvData::CreateRequest::create()->createRequest(requestDescriptor);
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430

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
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
		pvRequest = epics::pvAccess::getCreateRequest()->createRequest("field()", requester);
#else
		pvRequest = epics::pvData::CreateRequest::create()->createRequest("field()");
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
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
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
    epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvAccess::getCreateRequest()->createRequest(requestDescriptor, requester);
#else
    epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvData::CreateRequest::create()->createRequest(requestDescriptor);
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
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
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
        channelPut->put(false);
#else
        putRequesterImpl->resetEvent();
        channelPut->put(pvStructurePtr, putRequesterImpl->getBitSet());
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
	    if (putRequesterImpl->waitUntilDone(timeout)) {
	        return;
        }
    }
    throw ChannelTimeout("Channel %s put request timed out", channel->getChannelName().c_str());
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
}

void Channel::callSubscribers(PvObject& pvObject)
{
    epics::pvData::Lock lock(subscriberMutex);

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
        logger.trace("Acquiring python GIL");
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
    if (monitorThreadDone) {
        monitorThreadDone = false;

        // One must call PyEval_InitThreads() in the main thread
        // to initialize thread state, which is needed for proper functioning
        // of PyGILState_Ensure()/PyGILState_Release().
        // PyEval_InitThreads();
        PyGilManager::evalInitThreads();
#if defined PVA_API_VERSION && PVA_API_VERSION == 430
        epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvAccess::getCreateRequest()->createRequest(requestDescriptor, requester);
#else
        epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvData::CreateRequest::create()->createRequest(requestDescriptor);
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430
        channel->createMonitor(monitorRequester, pvRequest);
        epicsThreadCreate("ChannelMonitorThread", epicsThreadPriorityLow, epicsThreadGetStackSize(epicsThreadStackSmall), (EPICSTHREADFUNC)monitorThread, this);
    }
}

void Channel::stopMonitor()
{
    ChannelMonitorRequesterImpl* monitorRequester = getMonitorRequester();
    monitorRequester->cancelGetQueuedPvObject();
    monitorThreadDone = true;
}

bool Channel::isMonitorThreadDone() const
{
    return monitorThreadDone;
}

void Channel::monitorThread(Channel* channel)
{
    logger.debug("Started monitor thread %s", epicsThreadGetNameSelf());
    ChannelMonitorRequesterImpl* monitorRequester = channel->getMonitorRequester();
    while (true) {
        if (channel->isMonitorThreadDone()) {
            logger.debug("Monitor thread %s is done", epicsThreadGetNameSelf());
            break;
        }
        
        // Handle possible exceptions while retrieving data from empty queue.
        try {
            PvObject pvObject = monitorRequester->getQueuedPvObject(channel->getTimeout());
            channel->callSubscribers(pvObject);
        }
        catch (const ChannelTimeout& ex) {
            // Ignore, no changes received.
        }
        catch (const std::exception& ex) {
            // Not good.
            logger.error("Exception caught in monitor thread %s: %s", epicsThreadGetNameSelf(), ex.what());
        }
    }
    logger.debug("Exiting monitor thread %s", epicsThreadGetNameSelf());
}


