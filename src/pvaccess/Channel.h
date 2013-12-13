#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include "ChannelGetRequesterImpl.h"
#include "ChannelMonitorRequesterImpl.h"
#include "ChannelRequesterImpl.h"
#include "SynchronizedQueue.h"
#include "PvaClient.h"
#include "PvObject.h"
#include "PvaPyLogger.h"

#define DEFAULT_TIMEOUT 3.0
#define DEFAULT_REQUEST "field(value)"

class Channel
{
public:
    
    Channel(const epics::pvData::String& channelName);
    Channel(const Channel& channel);
    virtual ~Channel();

    std::string getName() const;
    virtual PvObject* get();
    virtual void put(const PvObject& pvObject);

    virtual void subscribe(const std::string& subscriberName, const boost::python::object& pySubscriber);
    virtual void unsubscribe(const std::string& subscriberName);
    virtual void callSubscribers(PvObject& pvObject);
    virtual void startMonitor();
    virtual void stopMonitor();
    virtual bool isMonitorThreadDone() const;

private:
    static PvaPyLogger logger;
    static PvaClient pvaClient;
    static epics::pvData::Requester::shared_pointer requester;
    static epics::pvAccess::ChannelProvider::shared_pointer provider;
    static std::tr1::shared_ptr<ChannelRequesterImpl> requesterImpl;
    static void monitorThread(Channel* channel);

    ChannelMonitorRequesterImpl* getMonitorRequester(); 
    ChannelGetRequesterImpl channelGetRequester;
    epics::pvAccess::Channel::shared_pointer channel;
    epics::pvData::MonitorRequester::shared_pointer monitorRequester;
    bool monitorThreadDone;
    SynchronizedQueue<PvObject> pvObjectMonitorQueue;
    std::map<std::string, boost::python::object> subscriberMap;
    epics::pvData::Mutex subscriberMutex;
};

inline std::string Channel::getName() const
{
    return channelGetRequester.getChannelName();
}

#endif
