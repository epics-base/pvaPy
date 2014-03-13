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

class Channel
{
public:

    static const char* DefaultRequestDescriptor;
    static const double DefaultTimeout;
        
    Channel(const epics::pvData::String& channelName);
    Channel(const Channel& channel);
    virtual ~Channel();

    std::string getName() const;
    virtual PvObject* get(const std::string& requestDescriptor);
    virtual PvObject* get();
    virtual void put(const PvObject& pvObject, const std::string& requestDescriptor);
    virtual void put(const PvObject& pvObject);

    virtual void subscribe(const std::string& subscriberName, const boost::python::object& pySubscriber);
    virtual void unsubscribe(const std::string& subscriberName);
    virtual void callSubscribers(PvObject& pvObject);
    virtual void startMonitor(const std::string& requestDescriptor);
    virtual void startMonitor();
    virtual void stopMonitor();
    virtual bool isMonitorThreadDone() const;
    virtual void setTimeout(double timeout);
    virtual double getTimeout() const;

private:
    static PvaPyLogger logger;
    static PvaClient pvaClient;
    static void monitorThread(Channel* channel);

    ChannelMonitorRequesterImpl* getMonitorRequester(); 

    epics::pvData::Requester::shared_pointer requester;
    std::tr1::shared_ptr<ChannelRequesterImpl> requesterImpl;
    ChannelGetRequesterImpl channelGetRequester;
    epics::pvAccess::ChannelProvider::shared_pointer provider;
    epics::pvAccess::Channel::shared_pointer channel;
    epics::pvData::MonitorRequester::shared_pointer monitorRequester;
    bool monitorThreadDone;
    SynchronizedQueue<PvObject> pvObjectMonitorQueue;
    std::map<std::string, boost::python::object> subscriberMap;
    epics::pvData::Mutex subscriberMutex;
    double timeout;
};

inline std::string Channel::getName() const
{
    return channelGetRequester.getChannelName();
}

inline void Channel::setTimeout(double timeout) 
{
    this->timeout = timeout;
}

inline double Channel::getTimeout() const
{
    return timeout;
}

#endif
