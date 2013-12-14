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
#include "ObjectNotFound.h"

PvaPyLogger Channel::logger("Channel");
PvaClient Channel::pvaClient;
epics::pvData::Requester::shared_pointer Channel::requester(new RequesterImpl("Channel"));
epics::pvAccess::ChannelProvider::shared_pointer Channel::provider = epics::pvAccess::getChannelAccess()->getProvider("pva");
std::tr1::shared_ptr<ChannelRequesterImpl> Channel::requesterImpl(new ChannelRequesterImpl(true));


Channel::Channel(const epics::pvData::String& channelName) :
    channelGetRequester(channelName),
    channel(provider->createChannel(channelName, requesterImpl)),
    monitorRequester(new ChannelMonitorRequesterImpl(channelName)),
    monitorThreadDone(true),
    pvObjectMonitorQueue(),
    subscriberMap(),
    subscriberMutex()
{
}
    
Channel::Channel(const Channel& c) :
    channelGetRequester(c.channelGetRequester),
    channel(c.channel),
    monitorRequester(c.monitorRequester),
    monitorThreadDone(true),
    pvObjectMonitorQueue(),
    subscriberMap(),
    subscriberMutex()
{
}

Channel::~Channel()
{
    stopMonitor();
    channel->destroy();
}
 
PvObject* Channel::get() 
{
    epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvAccess::getCreateRequest()->createRequest(DEFAULT_REQUEST, requester);
    std::tr1::shared_ptr<ChannelRequesterImpl> channelRequesterImpl = std::tr1::dynamic_pointer_cast<ChannelRequesterImpl>(channel->getChannelRequester());

    if (channel->getConnectionState() != epics::pvAccess::Channel::CONNECTED) {
        if (!channelRequesterImpl->waitUntilConnected(DEFAULT_TIMEOUT)) {
            throw ChannelTimeout("Channel %s timed out", channel->getChannelName().c_str());
        }
    }

    std::tr1::shared_ptr<GetFieldRequesterImpl> getFieldRequesterImpl;
    getFieldRequesterImpl.reset(new GetFieldRequesterImpl(channel));
    channel->getField(getFieldRequesterImpl, "");

    if (!getFieldRequesterImpl->waitUntilFieldGet(DEFAULT_TIMEOUT)) {
        throw ChannelTimeout("Channel %s field get timed out", channel->getChannelName().c_str());
    }
            
    if (!getFieldRequesterImpl.get()) {
        throw PvaException("Failed to get introspection data for channel %s", channel->getChannelName().c_str());
    }

	epics::pvData::Structure::const_shared_pointer structure =
	    std::tr1::dynamic_pointer_cast<const epics::pvData::Structure>(getFieldRequesterImpl->getField());
	if (structure.get() == 0 || structure->getField("value").get() == 0) {
	    // fallback to structure
		pvRequest = epics::pvAccess::getCreateRequest()->createRequest("field()", requester);
    }

	std::tr1::shared_ptr<ChannelGetRequesterImpl> getRequesterImpl(new ChannelGetRequesterImpl(channel->getChannelName()));
	epics::pvAccess::ChannelGet::shared_pointer channelGet = channel->createChannelGet(getRequesterImpl, pvRequest);
	if (getRequesterImpl->waitUntilGet(DEFAULT_TIMEOUT)) {
	    return new PvObject(getRequesterImpl->getPVStructure());
    }
    throw ChannelTimeout("Channel %s get request timed out", channel->getChannelName().c_str());
}

void Channel::put(const PvObject& pvObject) 
{
    epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvAccess::getCreateRequest()->createRequest(DEFAULT_REQUEST, requester);
    std::tr1::shared_ptr<ChannelRequesterImpl> channelRequesterImpl = std::tr1::dynamic_pointer_cast<ChannelRequesterImpl>(channel->getChannelRequester());

    if (channel->getConnectionState() != epics::pvAccess::Channel::CONNECTED) {
        if (!channelRequesterImpl->waitUntilConnected(DEFAULT_TIMEOUT)) {
            throw ChannelTimeout("Channel %s timed out", channel->getChannelName().c_str());
        }
    }

	std::tr1::shared_ptr<ChannelPutRequesterImpl> putRequesterImpl(new ChannelPutRequesterImpl(channel->getChannelName()));
	epics::pvAccess::ChannelPut::shared_pointer channelPut = channel->createChannelPut(putRequesterImpl, pvRequest);
	if (putRequesterImpl->waitUntilDone(DEFAULT_TIMEOUT)) {
        epics::pvData::PVStructurePtr pvStructurePtr = putRequesterImpl->getStructure();
        pvStructurePtr << pvObject;
        channelPut->put(false);
	    if (putRequesterImpl->waitUntilDone(DEFAULT_TIMEOUT)) {
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
        logger.trace("Acquiring python GIL");
        PyGILState_STATE gilState = PyGILState_Ensure();
        logger.trace("Python GIL state: %d", gilState);

        try {
            logger.debug("Invoking subscriber: " + subscriberName);

            // Call python code
            pySubscriber(pvObject);

        }
        catch(const boost::python::error_already_set&) {
            logger.error("Channel subscriber " + subscriberName + " error");
        }

        // Release GIL. 
        logger.trace("Releasing python GIL");
        PyGILState_Release(gilState);
    }
    logger.trace("Done calling subscribers");
}

void Channel::startMonitor()
{
    if (monitorThreadDone) {
        monitorThreadDone = false;

        // One must call PyEval_InitThreads() in the main thread
        // to initialize thread state, which is needed for proper functioning
        // of PyGILState_Ensure()/PyGILState_Release().
        PyEval_InitThreads();
        epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvAccess::getCreateRequest()->createRequest(DEFAULT_REQUEST, requester);
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
            PvObject pvObject = monitorRequester->getQueuedPvObject(DEFAULT_TIMEOUT);
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
