// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <vector>
#include <map>

#include "boost/python/list.hpp"
#include "boost/python/dict.hpp"

#include "pv/pvaClient.h"

#include "ChannelGetRequesterImpl.h"
#include "ChannelMonitorRequesterImpl.h"
#include "ChannelStateRequesterImpl.h"
#include "ChannelMonitorDataProcessor.h"
#include "ChannelRequesterImpl.h"
#include "SynchronizedQueue.h"
#include "PvObjectQueue.h"
#include "PvaClient.h"
#include "CaClient.h"
#include "PvObject.h"
#include "PvProvider.h"
#include "PvaPyLogger.h"
#include "PvaConstants.h"

#ifndef Py_REFCNT
#define Py_REFCNT(o)  (((PyObject*)(o))->ob_refcnt)
#endif

class Channel : public ChannelMonitorDataProcessor
{
public:

    static const char* DefaultSubscriberName;
    static const double DefaultTimeout;
    static const int DefaultMaxPvObjectQueueLength;
    static const int MaxAsyncRequestQueueLength;
    static const int MaxAsyncRequestWaitTimeout;
    static const int AsyncRequestThreadWaitTimeout;

    Channel(const std::string& channelName, PvProvider::ProviderType providerType=PvProvider::PvaProviderType);
    Channel(const Channel& channel);
    virtual ~Channel();
    virtual void setConnectionCallback(const boost::python::object& callback);

    std::string getName() const;

    // Get methods
    virtual PvObject* get(const std::string& requestDescriptor);
    virtual PvObject* get();
    virtual void asyncGet(const boost::python::object& pyCallback, const boost::python::object& pyErrorCallback, const std::string& requestDescriptor);
    virtual void asyncGet(const boost::python::object& pyCallback, const boost::python::object& pyErrorCallback);

    // Put methods
    virtual void put(const PvObject& pvObject, const std::string& requestDescriptor);
    virtual void put(const PvObject& pvObject);
    virtual void asyncPut(const PvObject& pvObject, const boost::python::object& pyCallback, const boost::python::object& pyErrorCallback, const std::string& requestDescriptor);
    virtual void asyncPut(const PvObject& pvObject, const boost::python::object& pyCallback, const boost::python::object& pyErrorCallback);

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

    virtual void parsePut(
        const boost::python::list& args,
        const std::string& requestDescriptor,
        bool zeroArrayLength = true);

    virtual PvObject* parsePutGet(
        const boost::python::list& args,
        const std::string& requestDescriptor,
        bool zeroArrayLength = true);

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
#ifndef WINDOWS
    virtual void monitor(const boost::python::object& pySubscriber, const std::string& requestDescriptor=PvaConstants::DefaultKey);
#else
    virtual void monitor(const boost::python::object& pySubscriber, const std::string& requestDescriptor);
#endif
    virtual void monitor(PvObjectQueue& pyObjectQueue);
    virtual void monitor(PvObjectQueue& pyObjectQueue, const std::string& requestDescriptor);
    virtual void stopMonitor();
    virtual bool isMonitorActive() const;
    virtual void resetMonitorCounters();
    virtual boost::python::dict getMonitorCounters();

    virtual void setTimeout(double timeout);
    virtual double getTimeout() const;
    virtual void setDefaultRequestDescriptor(const std::string& requestDescriptor);
    virtual std::string getDefaultRequestDescriptor() const;
    virtual void setDefaultPutGetRequestDescriptor(const std::string& requestDescriptor);
    virtual std::string getDefaultPutGetRequestDescriptor() const;
    virtual void setMonitorMaxQueueLength(int maxLength);
    virtual int getMonitorMaxQueueLength();

    // Monitor data processing interface
    virtual void processMonitorData(epics::pvData::PVStructurePtr pvStructurePtr);
    virtual void onChannelConnect();
    virtual void onChannelDisconnect();
    virtual void onMonitorOverrun(epics::pvData::BitSetPtr bitSetPtr);
    virtual void callConnectionCallback(bool isConnected);
    virtual bool isChannelConnected();

    // Introspection
    virtual boost::python::dict getIntrospectionDict();

private:
    static const double ShutdownWaitTime;
    static const double MonitorStartWaitTime;
    static const double ThreadStartWaitTime;

    static PvaPyLogger logger;
    static PvaClient pvaClient;
    static CaClient caClient;

    static void processingThread(Channel* channel);
    static void issueConnectThread(Channel* channel);

    void startProcessingThread();
    void startIssueConnectThread();
    void waitForProcessingThreadExit(double timeout);
    void notifyProcessingThreadExit();

    void connect();
    void issueConnect();
    void determineDefaultRequestDescriptor();
    epics::pvaClient::PvaClientGetPtr createGetPtr(const std::string& requestDescriptor);
    epics::pvaClient::PvaClientPutPtr createPutPtr(const std::string& requestDescriptor);
    epics::pvaClient::PvaClientPutGetPtr createPutGetPtr(const std::string& requestDescriptor);

    void callSubscriber(const std::string& pySubscriberName, boost::python::object& pySubscriber, PvObject& pvObject);
    void invokePyCallback(boost::python::object& pyCallback, PvObject& pvObject);
    void invokePyCallback(boost::python::object& pyCallback, std::string errorMsg);

    void preparePut(const PvObject& pvObject, epics::pvaClient::PvaClientPutPtr& pvaPut);

    static epics::pvaClient::PvaClientPtr pvaClientPtr;
    epics::pvaClient::PvaClientChannelPtr pvaClientChannelPtr;
    epics::pvaClient::PvaClientMonitorRequesterPtr pvaClientMonitorRequesterPtr;
    epics::pvaClient::PvaClientMonitorPtr pvaClientMonitorPtr;
    std::string monitorRequestDescriptor;
    epics::pvData::StructureConstPtr monitorStructurePtr;

    bool monitorActive;
    bool monitorRunning;
    bool processingThreadRunning;
    PvObjectQueue pvObjectQueue;
    bool useInternalPvObjectQueue;

    // Use for single subscriber only
    std::string subscriberName;
    boost::python::object subscriber;

    // Use for multiple subscribers
    std::map<std::string, boost::python::object> subscriberMap;
    epics::pvData::Mutex subscriberMutex;

    epics::pvData::Mutex monitorMutex;
    epics::pvData::Mutex processingThreadMutex;
    epicsEvent processingThreadExitEvent;
    double timeout;
    PvProvider::ProviderType providerType;
    std::string defaultRequestDescriptor;
    std::string defaultPutGetRequestDescriptor;

    bool isConnected;
    bool hasIssuedConnect;
    epics::pvaClient::PvaClientChannelStateChangeRequesterPtr stateRequester;
    bool connectionCallbackRequiresThread;
    boost::python::object connectionCallback;

    // Async functionality
    void asyncConnect();
    static void asyncGetThread(Channel* channel);
    void startAsyncGetThread();
    void notifyAsyncGetThreadExit();
    void waitForAsyncGetThreadExit(double timeout);

    static void asyncPutThread(Channel* channel);
    void startAsyncPutThread();
    void notifyAsyncPutThreadExit();
    void waitForAsyncPutThreadExit(double timeout);

    bool asyncGetThreadRunning;
    epics::pvData::Mutex asyncGetThreadMutex;
    epicsEvent asyncGetThreadExitEvent;

    bool asyncPutThreadRunning;
    epics::pvData::Mutex asyncPutThreadMutex;
    epicsEvent asyncPutThreadExitEvent;

    struct AsyncRequest {
        boost::python::object pyCallback;
        boost::python::object pyErrorCallback;
        std::string requestDescriptor;
        epics::pvData::PVStructurePtr pvStructurePtr;
        AsyncRequest(boost::python::object pyCallback_, boost::python::object pyErrorCallback_, const std::string& requestDescriptor_) 
        : pyCallback(pyCallback_) 
        , pyErrorCallback(pyErrorCallback_) 
        , requestDescriptor(requestDescriptor_) 
        {}
        AsyncRequest(const epics::pvData::PVStructurePtr& pvStructurePtr_, boost::python::object pyCallback_, boost::python::object pyErrorCallback_, const std::string& requestDescriptor_) 
        : pyCallback(pyCallback_) 
        , pyErrorCallback(pyErrorCallback_) 
        , requestDescriptor(requestDescriptor_) 
        , pvStructurePtr(pvStructurePtr_) 
        {}

        // In some cases it seems like boost python object reference counts
        // go below the minimum before destructor is called
        ~AsyncRequest() {
            if (Py_REFCNT(pyCallback.ptr()) <= 1) {
                boost::python::incref(pyCallback.ptr());
            }
            if (Py_REFCNT(pyErrorCallback.ptr()) <= 1) {
                boost::python::incref(pyErrorCallback.ptr());
            }
        }
    };

    typedef std::tr1::shared_ptr<AsyncRequest> AsyncRequestPtr;
    SynchronizedQueue<AsyncRequestPtr> asyncGetRequestQueue;
    SynchronizedQueue<AsyncRequestPtr> asyncPutRequestQueue;

    bool shutdownInProgress;
};

inline std::string Channel::getName() const
{
    return pvaClientChannelPtr->getChannelName();
}

inline bool Channel::isMonitorActive() const
{
    return monitorActive;
}

inline void Channel::setTimeout(double timeout)
{
    this->timeout = timeout;
}

inline double Channel::getTimeout() const
{
    return timeout;
}

inline void Channel::setDefaultRequestDescriptor(const std::string& requestDescriptor)
{
    this->defaultRequestDescriptor = requestDescriptor;
}

inline std::string Channel::getDefaultRequestDescriptor() const
{
    return defaultRequestDescriptor;
}

inline void Channel::setDefaultPutGetRequestDescriptor(const std::string& requestDescriptor)
{
    this->defaultPutGetRequestDescriptor = requestDescriptor;
}

inline std::string Channel::getDefaultPutGetRequestDescriptor() const
{
    return defaultPutGetRequestDescriptor;
}

inline void Channel::notifyProcessingThreadExit()
{
    processingThreadExitEvent.signal();
}

inline void Channel::notifyAsyncGetThreadExit()
{
    asyncGetThreadExitEvent.signal();
}

inline void Channel::notifyAsyncPutThreadExit()
{
    asyncPutThreadExitEvent.signal();
}

#endif
