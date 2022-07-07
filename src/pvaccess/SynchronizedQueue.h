// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef SYNCHRONIZED_QUEUE_H
#define SYNCHRONIZED_QUEUE_H

#include <queue>
#include <string>
#include <map>
#include <epicsEvent.h>
#include <pv/pvData.h>
#include "InvalidRequest.h"

template <class T>
class SynchronizedQueue : public std::queue<T>
{
public:
    POINTER_DEFINITIONS(SynchronizedQueue<T>);

    static const int Unlimited = -1;
    static const char* NumReceivedCounterKey;
    static const char* NumRejectedCounterKey;
    static const char* NumDeliveredCounterKey;

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

private:
    void throwInvalidRequestIfEmpty() ;
    T frontAndPopUnsynchronized();
    void pushUnsynchronized(const T& t);

    epics::pvData::Mutex mutex;
    epicsEvent itemPushedEvent;
    epicsEvent itemPoppedEvent;
    int maxLength;

    // Statistics counters
    std::map<std::string, unsigned int> counterMap;
    unsigned int nReceived;
    unsigned int nRejected;
    unsigned int nDelivered;
};

template <class T>
const char* SynchronizedQueue<T>::NumReceivedCounterKey("nReceived");
template <class T>
const char* SynchronizedQueue<T>::NumRejectedCounterKey("nRejected");
template <class T>
const char* SynchronizedQueue<T>::NumDeliveredCounterKey("nDelivered");

template <class T>
SynchronizedQueue<T>::SynchronizedQueue(int maxLength_) 
    : std::queue<T>()
    , mutex()
    , itemPushedEvent()
    , itemPoppedEvent()
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
void SynchronizedQueue<T>::throwInvalidRequestIfEmpty() 
{
    if (std::queue<T>::empty()) {
        throw InvalidRequest("Queue is empty.");
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
    throwInvalidRequestIfEmpty();
    return std::queue<T>::back();
}

template <class T>
T SynchronizedQueue<T>::front() 
{
    epics::pvData::Lock lock(mutex);
    throwInvalidRequestIfEmpty();
    return std::queue<T>::front();
}

template <class T>
T SynchronizedQueue<T>::frontAndPopUnsynchronized() 
{
    T t = std::queue<T>::front();
    std::queue<T>::pop();
    nDelivered++;
    itemPoppedEvent.signal();
    return t;
}

template <class T>
void SynchronizedQueue<T>::pushUnsynchronized(const T& t) 
{
    std::queue<T>::push(t);
    nReceived++;
    itemPushedEvent.signal();
}

template <class T>
T SynchronizedQueue<T>::frontAndPop() 
{
    epics::pvData::Lock lock(mutex);
    throwInvalidRequestIfEmpty();
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
    }
    waitForItemPushed(timeout);
    return frontAndPop();
}

template <class T>
void SynchronizedQueue<T>::pop()
{
    epics::pvData::Lock lock(mutex);
    if (!std::queue<T>::empty()) {
        std::queue<T>::pop();
        nDelivered++;
        itemPoppedEvent.signal();
    }
    else {
        throw InvalidRequest("Queue is empty.");
    }
}

template <class T>
bool SynchronizedQueue<T>::popIfNotEmpty()
{
    epics::pvData::Lock lock(mutex);
    if (!std::queue<T>::empty()) {
        std::queue<T>::pop();
        nDelivered++;
        itemPoppedEvent.signal();
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
        throw InvalidRequest("Queue is full.");
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
    nReceived++;
    itemPushedEvent.signal();
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
        itemPoppedEvent.signal();
    }
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
    counterMap[NumReceivedCounterKey] = nReceived;
    counterMap[NumRejectedCounterKey] = nRejected;
    counterMap[NumDeliveredCounterKey] = nDelivered;
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

typedef std::tr1::shared_ptr<SynchronizedQueue<std::string> > StringQueuePtr;

#endif
