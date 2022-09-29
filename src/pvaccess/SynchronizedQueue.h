// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef SYNCHRONIZED_QUEUE_H
#define SYNCHRONIZED_QUEUE_H

#include <queue>
#include <string>
#include <map>
#include <epicsEvent.h>
#include <epicsTime.h>
#include <pv/pvData.h>
#include "QueueEmpty.h"
#include "QueueFull.h"
#include "PvaPyConstants.h"

template <class T>
class SynchronizedQueue : public std::queue<T>
{
public:
    POINTER_DEFINITIONS(SynchronizedQueue<T>);

    static const int Unlimited = -1;

    SynchronizedQueue(int maxLength=Unlimited);
    SynchronizedQueue(const SynchronizedQueue& q);
    virtual ~SynchronizedQueue();
    void setMaxLength(int maxLength);
    int getMaxLength();
    bool isFull();
    bool isEmpty();
    unsigned int size();
    T back();
    T front();
    T frontAndPop();
    T frontAndPop(double timeout);
    void pop();
    void push(const T& t);
    void push(const T& t, double timeout);

    // No exception, return true if operation succeeded
    bool popIfNotEmpty();
    bool pushIfNotFull(const T& t);

    void waitForItemPushed(double timeout);
    void waitForItemPushedIfEmpty(double timeout);
    void waitForItemPopped(double timeout);
    void waitForItemPoppedIfFull(double timeout);
    void cancelWaitForItemPushed();
    void cancelWaitForItemPopped();
    void clear();

    // Statistics
    void resetCounters();
    const std::map<std::string,unsigned int>& getCounterMap();
    void setCounter(const std::string& key, unsigned int value);
    void addToCounter(const std::string& key, unsigned int value);
    double getTimeSinceLastPush();
    double getTimeSinceLastPop();

private:
    void throwQueueEmptyIfEmpty() ;
    T frontAndPopUnsynchronized();
    void pushUnsynchronized(const T& t);

    epics::pvData::Mutex mutex;
    epicsEvent itemPushedEvent;
    epicsTimeStamp lastPushedTime;
    epicsEvent itemPoppedEvent;
    epicsTimeStamp lastPoppedTime;
    int maxLength;

    // Statistics counters
    std::map<std::string, unsigned int> counterMap;
    unsigned int nReceived;
    unsigned int nRejected;
    unsigned int nDelivered;
};

template <class T>
SynchronizedQueue<T>::SynchronizedQueue(int maxLength_) 
    : std::queue<T>()
    , mutex()
    , itemPushedEvent()
    , lastPushedTime()
    , itemPoppedEvent()
    , lastPoppedTime()
    , maxLength(maxLength_)
    , counterMap()
    , nReceived(0)
    , nRejected(0)
    , nDelivered(0)
{
}

template <class T>
SynchronizedQueue<T>::SynchronizedQueue(const SynchronizedQueue<T>& q) 
    : std::queue<T>(q)
    , mutex()
    , itemPushedEvent()
    , itemPoppedEvent()
    , maxLength(q.maxLength)
    , counterMap(q.counterMap)
    , nReceived(q.nReceived)
    , nRejected(q.nRejected)
    , nDelivered(q.nDelivered)
{
}

template <class T>
SynchronizedQueue<T>::~SynchronizedQueue()
{
    itemPushedEvent.signal();
    itemPoppedEvent.signal();
}

template <class T>
int SynchronizedQueue<T>::getMaxLength() 
{
    return maxLength; 
}

template <class T>
void SynchronizedQueue<T>::setMaxLength(int maxLength)
{
    epics::pvData::Lock lock(mutex);
    this->maxLength = maxLength; 
}

template <class T>
bool SynchronizedQueue<T>::isFull() 
{
    epics::pvData::Lock lock(mutex);
    int size = std::queue<T>::size();
    if (maxLength > 0 && size >= maxLength) {
        return true;
    }
    return false;
}

template <class T>
bool SynchronizedQueue<T>::isEmpty() 
{
    epics::pvData::Lock lock(mutex);
    return std::queue<T>::empty();
}

template <class T>
void SynchronizedQueue<T>::throwQueueEmptyIfEmpty() 
{
    if (std::queue<T>::empty()) {
        throw QueueEmpty("Queue is empty.");
    }
}

template <class T>
unsigned int SynchronizedQueue<T>::size() 
{
    epics::pvData::Lock lock(mutex);
    return std::queue<T>::size();
}

template <class T>
T SynchronizedQueue<T>::back() 
{
    epics::pvData::Lock lock(mutex);
    throwQueueEmptyIfEmpty();
    return std::queue<T>::back();
}

template <class T>
T SynchronizedQueue<T>::front() 
{
    epics::pvData::Lock lock(mutex);
    throwQueueEmptyIfEmpty();
    return std::queue<T>::front();
}

template <class T>
T SynchronizedQueue<T>::frontAndPopUnsynchronized() 
{
    bool isFull = (maxLength > 0 && std::queue<T>::size() >= maxLength);
    T t = std::queue<T>::front();
    std::queue<T>::pop();
    epicsTimeGetCurrent(&lastPoppedTime);
    nDelivered++;
    if (isFull) {
        // Signal pop when queue was full
        itemPoppedEvent.signal();
    }
    return t;
}

template <class T>
void SynchronizedQueue<T>::pushUnsynchronized(const T& t) 
{
    bool isEmpty = std::queue<T>::empty();
    std::queue<T>::push(t);
    epicsTimeGetCurrent(&lastPushedTime);
    nReceived++;
    if (isEmpty) {
        // Signal push when queue was empty
        itemPushedEvent.signal();
    }
}

template <class T>
T SynchronizedQueue<T>::frontAndPop() 
{
    epics::pvData::Lock lock(mutex);
    throwQueueEmptyIfEmpty();
    return frontAndPopUnsynchronized();
}

template <class T>
T SynchronizedQueue<T>::frontAndPop(double timeout) 
{
    {
        epics::pvData::Lock lock(mutex);
        if (!std::queue<T>::empty()) {
            return frontAndPopUnsynchronized();
        }
        // Clear push event.
        itemPushedEvent.tryWait();
    }
    waitForItemPushed(timeout);
    return frontAndPop();
}

template <class T>
void SynchronizedQueue<T>::pop()
{
    epics::pvData::Lock lock(mutex);
    int size = std::queue<T>::size();
    if (size > 0) {
        bool isFull = (maxLength > 0 && size >= maxLength); 
        std::queue<T>::pop();
        epicsTimeGetCurrent(&lastPoppedTime);
        nDelivered++;
        if (isFull) {
            // Signal pop when queue was full
            itemPoppedEvent.signal();
        }
    }
    else {
        throw QueueEmpty("Queue is empty.");
    }
}

template <class T>
bool SynchronizedQueue<T>::popIfNotEmpty()
{
    epics::pvData::Lock lock(mutex);
    int size = std::queue<T>::size();
    if (size > 0) {
        bool isFull = (maxLength > 0 && size >= maxLength); 
        std::queue<T>::pop();
        epicsTimeGetCurrent(&lastPoppedTime);
        nDelivered++;
        if (isFull) {
            // Signal pop when queue was full
            itemPoppedEvent.signal();
        }
        return true;
    }
    return false;
}

template <class T>
void SynchronizedQueue<T>::push(const T& t) 
{
    epics::pvData::Lock lock(mutex);
    int size = std::queue<T>::size();
    if (maxLength > 0 && size >= maxLength) {
        // We are full, throw exception
        nRejected++;
        throw QueueFull("Queue is full.");
    }
    pushUnsynchronized(t);
}

template <class T>
void SynchronizedQueue<T>::push(const T& t, double timeout) 
{
    {
        epics::pvData::Lock lock(mutex);
        int size = std::queue<T>::size();
        if (maxLength <= 0 || size < maxLength) {
            pushUnsynchronized(t);
        }
        // Clear pop event.
        itemPoppedEvent.tryWait();
    }
    waitForItemPopped(timeout);
    push(t);
}

template <class T>
bool SynchronizedQueue<T>::pushIfNotFull(const T& t)
{
    epics::pvData::Lock lock(mutex);
    int size = std::queue<T>::size();
    if (maxLength > 0 && size >= maxLength) {
        // We are full, ignore push request
        nRejected++;
        return false;
    }
    std::queue<T>::push(t);
    epicsTimeGetCurrent(&lastPushedTime);
    nReceived++;
    if (size == 0) {
        // Signal push when queue was empty
        itemPushedEvent.signal();
    }
    return true;
}

template <class T>
void SynchronizedQueue<T>::waitForItemPushed(double timeout) 
{
    itemPushedEvent.wait(timeout);
}

template <class T>
void SynchronizedQueue<T>::waitForItemPushedIfEmpty(double timeout) 
{
    {
        epics::pvData::Lock lock(mutex);
        int size = std::queue<T>::size();
        if (size > 0) {
            return;
        }
        // Clear push event.
        itemPushedEvent.tryWait();
    }
    // Queue is empty, wait for push 
    itemPushedEvent.wait(timeout);
}

template <class T>
void SynchronizedQueue<T>::waitForItemPopped(double timeout) 
{
    itemPoppedEvent.wait(timeout);
}

template <class T>
void SynchronizedQueue<T>::waitForItemPoppedIfFull(double timeout) 
{
    {
        epics::pvData::Lock lock(mutex);
        int size = std::queue<T>::size();
        if (maxLength <= 0 || size < maxLength) {
            return;
        }
        // Clear pop event.
        itemPoppedEvent.tryWait();
    }
    // Queue is full, wait for pop
    itemPoppedEvent.wait(timeout);
}

template <class T>
void SynchronizedQueue<T>::cancelWaitForItemPushed() 
{
    itemPushedEvent.signal();
}

template <class T>
void SynchronizedQueue<T>::cancelWaitForItemPopped() 
{
    itemPoppedEvent.signal();
}

template <class T>
void SynchronizedQueue<T>::clear() 
{
    epics::pvData::Lock lock(mutex);
    while (!std::queue<T>::empty()) {
        std::queue<T>::pop();
    }
    itemPoppedEvent.signal();
}

template <class T>
void SynchronizedQueue<T>::resetCounters() 
{
    epics::pvData::Lock lock(mutex);
    typedef std::map<std::string, unsigned int>::iterator MI;
    for (MI it = counterMap.begin(); it != counterMap.end(); it++) {
        it->second = 0;
    }
    nReceived = 0;
    nRejected = 0;
    nDelivered = 0;
}

template <class T>
const std::map<std::string,unsigned int>& SynchronizedQueue<T>::getCounterMap()
{
    epics::pvData::Lock lock(mutex);
    counterMap[PvaPyConstants::NumReceivedCounterKey] = nReceived;
    counterMap[PvaPyConstants::NumRejectedCounterKey] = nRejected;
    counterMap[PvaPyConstants::NumDeliveredCounterKey] = nDelivered;
    counterMap[PvaPyConstants::NumQueuedCounterKey] = std::queue<T>::size();
    return counterMap;
}

template <class T>
void SynchronizedQueue<T>::setCounter(const std::string& key, unsigned int value) 
{
    epics::pvData::Lock lock(mutex);
    counterMap[key] = value;
}

template <class T>
void SynchronizedQueue<T>::addToCounter(const std::string& key, unsigned int value) 
{
    epics::pvData::Lock lock(mutex);
    std::map<std::string,unsigned int>::iterator it = counterMap.find(key);
    if (it != counterMap.end()) {
        it->second = it->second + value;
    }
    else {
        counterMap[key] = value;
    }
}

template <class T>
double SynchronizedQueue<T>::getTimeSinceLastPush()
{
    epicsTimeStamp ts;
    epicsTimeGetCurrent(&ts);
    return epicsTimeDiffInSeconds(&ts, &lastPushedTime);
}

template <class T>
double SynchronizedQueue<T>::getTimeSinceLastPop()
{
    epicsTimeStamp ts;
    epicsTimeGetCurrent(&ts);
    return epicsTimeDiffInSeconds(&ts, &lastPoppedTime);
}

typedef std::tr1::shared_ptr<SynchronizedQueue<std::string> > StringQueuePtr;

#endif
