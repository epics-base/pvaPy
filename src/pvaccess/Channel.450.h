// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <vector>

#include "boost/python/list.hpp"
#include "pv/pvaClient.h"

#include "ChannelGetRequesterImpl.h"
#include "ChannelMonitorRequesterImpl.h"
#include "ChannelRequesterImpl.h"
#include "SynchronizedQueue.h"
#include "PvaClient.h"
#include "CaClient.h"
#include "epicsEvent.h"
#include "PvObject.h"
#include "PvProvider.h"
#include "PvaPyLogger.h"

class Channel
{
public:

    static const char* DefaultRequestDescriptor;
    static const double DefaultTimeout;
        
    Channel(const std::string& channelName, PvProvider::ProviderType providerType=PvProvider::PvaProviderType);
    Channel(const Channel& channel);
    virtual ~Channel();

    std::string getName() const;
    virtual PvObject* get(const std::string& requestDescriptor);
    virtual PvObject* get();
    virtual void put(const PvObject& pvObject, const std::string& requestDescriptor);
    virtual void put(const PvObject& pvObject);
    virtual void put(const std::vector<std::string>& values, const std::string& requestDescriptor);
    virtual void put(const std::vector<std::string>& values);
    virtual void put(const std::string& value, const std::string& requestDescriptor);
    virtual void put(const std::string& value);
    virtual void put(const boost::python::list& pyList, const std::string& requestDescriptor);
    virtual void put(const boost::python::list& pyList);

    virtual void put(bool value, const std::string& requestDescriptor);
    virtual void put(bool value);
    virtual void put(char value, const std::string& requestDescriptor);
    virtual void put(char value);
    virtual void put(unsigned char value, const std::string& requestDescriptor);
    virtual void put(unsigned char value);
    virtual void put(short value, const std::string& requestDescriptor);
    virtual void put(short value);
    virtual void put(unsigned short value, const std::string& requestDescriptor);
    virtual void put(unsigned short value);
    virtual void put(int value, const std::string& requestDescriptor);
    virtual void put(int value);
    virtual void put(unsigned int value, const std::string& requestDescriptor);
    virtual void put(unsigned int value);
    virtual void put(long long value, const std::string& requestDescriptor);
    virtual void put(long long value);
    virtual void put(unsigned long long value, const std::string& requestDescriptor);
    virtual void put(unsigned long long value);
    virtual void put(float value, const std::string& requestDescriptor);
    virtual void put(float value);
    virtual void put(double value, const std::string& requestDescriptor);
    virtual void put(double value);

    virtual void subscribe(const std::string& subscriberName, const boost::python::object& pySubscriber);
    virtual void unsubscribe(const std::string& subscriberName);
    virtual void callSubscribers(PvObject& pvObject);
    virtual void startMonitor(const std::string& requestDescriptor);
    virtual void startMonitor();
    virtual void stopMonitor();
    virtual bool isMonitorThreadDone() const;
    virtual void setTimeout(double timeout);
    virtual double getTimeout() const;
    virtual void setMonitorMaxQueueLength(int maxLength);
    virtual int getMonitorMaxQueueLength();

private:
    static const double ShutdownWaitTime;

    static PvaPyLogger logger;
    static PvaClient pvaClient;
    static CaClient caClient;
    static void monitorThread(Channel* channel);
    static void processingThread(Channel* channel);

    void connect();
    epics::pvaClient::PvaClientMonitorPtr getMonitor();
    void queueMonitorData(PvObject& pvObject);

    bool processMonitorElement();
    void notifyMonitorThreadExit();

    static epics::pvaClient::PvaClientPtr pvaClientPtr;
    epics::pvaClient::PvaClientChannelPtr  pvaClientChannelPtr;
    epics::pvaClient::PvaClientMonitorPtr pvaClientMonitorPtr;
    SynchronizedQueue<PvObject> pvObjectMonitorQueue;

    bool monitorThreadDone;
    std::map<std::string, boost::python::object> subscriberMap;
    epics::pvData::Mutex subscriberMutex;
    epics::pvData::Mutex monitorElementProcessingMutex;
    epics::pvData::Mutex monitorThreadMutex;
    epicsEvent monitorThreadExitEvent;
    double timeout;
};

inline std::string Channel::getName() const
{
    return pvaClientChannelPtr->getChannelName();
}

inline void Channel::setTimeout(double timeout) 
{
    this->timeout = timeout;
}

inline double Channel::getTimeout() const
{
    return timeout;
}

inline void Channel::setMonitorMaxQueueLength(int maxLength) 
{
    pvObjectMonitorQueue.setMaxLength(maxLength);
}

inline int Channel::getMonitorMaxQueueLength() 
{
    return pvObjectMonitorQueue.getMaxLength();
}

inline void Channel::notifyMonitorThreadExit() 
{
    monitorThreadExitEvent.signal();
}

#endif
