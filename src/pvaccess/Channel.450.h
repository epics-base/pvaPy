// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <vector>
#include <map>

#include "boost/python/list.hpp"
#include "pv/pvaClient.h"

#include "ChannelGetRequesterImpl.h"
#include "ChannelMonitorRequesterImpl.h"
#include "ChannelMonitorDataProcessor.h"
#include "ChannelRequesterImpl.h"
#include "SynchronizedQueue.h"
#include "PvaClient.h"
#include "CaClient.h"
#include "PvObject.h"
#include "PvProvider.h"
#include "PvaPyLogger.h"

class Channel : public ChannelMonitorDataProcessor
{
public:

    static const char* DefaultRequestDescriptor;
    static const char* DefaultPutGetRequestDescriptor;
    static const double DefaultTimeout;
    static const int DefaultMaxPvObjectQueueLength;
        
    Channel(const std::string& channelName, PvProvider::ProviderType providerType=PvProvider::PvaProviderType);
    Channel(const Channel& channel);
    virtual ~Channel();

    std::string getName() const;

    // Get methods
    virtual PvObject* get(const std::string& requestDescriptor);
    virtual PvObject* get();

    // Put methods
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
    virtual void put(long int value, const std::string& requestDescriptor);
    virtual void put(long int value);
    virtual void put(unsigned long int value, const std::string& requestDescriptor);
    virtual void put(unsigned long int value);
    virtual void put(long long value, const std::string& requestDescriptor);
    virtual void put(long long value);
    virtual void put(unsigned long long value, const std::string& requestDescriptor);
    virtual void put(unsigned long long value);
    virtual void put(float value, const std::string& requestDescriptor);
    virtual void put(float value);
    virtual void put(double value, const std::string& requestDescriptor);
    virtual void put(double value);

    // PutGet methods
    virtual PvObject* putGet(const PvObject& pvObject, const std::string& requestDescriptor);
    virtual PvObject* putGet(const PvObject& pvObject);
    virtual PvObject* putGet(const std::vector<std::string>& values, const std::string& requestDescriptor);
    virtual PvObject* putGet(const std::vector<std::string>& values);
    virtual PvObject* putGet(const std::string& value, const std::string& requestDescriptor);
    virtual PvObject* putGet(const std::string& value);
    virtual PvObject* putGet(const boost::python::list& pyList, const std::string& requestDescriptor);
    virtual PvObject* putGet(const boost::python::list& pyList);

    virtual PvObject* putGet(bool value, const std::string& requestDescriptor);
    virtual PvObject* putGet(bool value);
    virtual PvObject* putGet(char value, const std::string& requestDescriptor);
    virtual PvObject* putGet(char value);
    virtual PvObject* putGet(unsigned char value, const std::string& requestDescriptor);
    virtual PvObject* putGet(unsigned char value);
    virtual PvObject* putGet(short value, const std::string& requestDescriptor);
    virtual PvObject* putGet(short value);
    virtual PvObject* putGet(unsigned short value, const std::string& requestDescriptor);
    virtual PvObject* putGet(unsigned short value);
    virtual PvObject* putGet(long int value, const std::string& requestDescriptor);
    virtual PvObject* putGet(long int value);
    virtual PvObject* putGet(unsigned long int value, const std::string& requestDescriptor);
    virtual PvObject* putGet(unsigned long int value);
    virtual PvObject* putGet(long long value, const std::string& requestDescriptor);
    virtual PvObject* putGet(long long value);
    virtual PvObject* putGet(unsigned long long value, const std::string& requestDescriptor);
    virtual PvObject* putGet(unsigned long long value);
    virtual PvObject* putGet(float value, const std::string& requestDescriptor);
    virtual PvObject* putGet(float value);
    virtual PvObject* putGet(double value, const std::string& requestDescriptor);
    virtual PvObject* putGet(double value);

    // GetPut methods
    virtual PvObject* getPut(const std::string& requestDescriptor);
    virtual PvObject* getPut();

    // Monitor methods
    virtual void subscribe(const std::string& subscriberName, const boost::python::object& pySubscriber);
    virtual void unsubscribe(const std::string& subscriberName);

    virtual void callSubscribers(PvObject& pvObject);
    virtual void startMonitor(const std::string& requestDescriptor);
    virtual void startMonitor();
    virtual void stopMonitor();
    virtual void setTimeout(double timeout);
    virtual double getTimeout() const;
    virtual void setMonitorMaxQueueLength(int maxLength);
    virtual int getMonitorMaxQueueLength();

    // Monitor data processing interface
    virtual void processMonitorData(epics::pvData::PVStructurePtr pvStructurePtr);
private:
    static const double ShutdownWaitTime;

    static PvaPyLogger logger;
    static PvaClient pvaClient;
    static CaClient caClient;

    static void processingThread(Channel* channel);
    void startProcessingThread();
    void waitForProcessingThreadExit(double timeout);
    void notifyProcessingThreadExit();

    void connect();

    static epics::pvaClient::PvaClientPtr pvaClientPtr;
    epics::pvaClient::PvaClientChannelPtr  pvaClientChannelPtr;
    epics::pvaClient::PvaClientMonitorRequesterPtr monitorRequester;
    epics::pvaClient::PvaClientMonitorPtr monitor;

    bool monitorActive;
    bool processingThreadRunning;
    SynchronizedQueue<PvObject> pvObjectQueue;

    std::map<std::string, boost::python::object> subscriberMap;
    epics::pvData::Mutex subscriberMutex;
    epics::pvData::Mutex monitorMutex;
    epics::pvData::Mutex processingThreadMutex;
    epicsEvent processingThreadExitEvent;
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

inline void Channel::notifyProcessingThreadExit()
{
    processingThreadExitEvent.signal();
}

#endif
