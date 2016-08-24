// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef SYNCHRONIZED_QUEUE_H
#define SYNCHRONIZED_QUEUE_H

#include <queue>
#include "epicsEvent.h"
#include "pv/pvData.h"
#include "InvalidState.h"


template <class T>
class SynchronizedQueue : public std::queue<T>
{
public:
    static const int Unlimited = -1;
    SynchronizedQueue(int maxLength=Unlimited);
    virtual ~SynchronizedQueue();
    void setMaxLength(int maxLength);
    int getMaxLength();
    bool isFull();
    T back() throw(InvalidState);
    T front() throw(InvalidState);
    T frontAndPop() throw(InvalidState);
    T frontAndPop(double timeout) throw(InvalidState);
    void pop();
    void push(const T& t) throw(InvalidState);
    void pushIfNotFull(const T& t);
    void waitForItemPushed(double timeout);
    void waitForItemPopped(double timeout);
    void waitForItemPoppedIfFull(double timeout);
    void cancelWaitForItemPushed();
    void cancelWaitForItemPopped();
    void clear();

private:
    void throwInvalidStateIfEmpty() throw(InvalidState);
    T frontAndPopUnsynchronized();

    epics::pvData::Mutex mutex;
    epicsEvent itemPushedEvent;
    epicsEvent itemPoppedEvent;
    int maxLength;
};

template <class T>
SynchronizedQueue<T>::SynchronizedQueue(int maxLength_) :
    std::queue<T>(),
    mutex(),
    itemPushedEvent(),
    itemPoppedEvent(),
    maxLength(maxLength_)
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
    int size = std::queue<T>::size();
    if (maxLength > 0 && size >= maxLength) {
        return true;
    }
    return false;
}

template <class T>
void SynchronizedQueue<T>::throwInvalidStateIfEmpty() throw(InvalidState)
{
    if (std::queue<T>::empty()) {
        throw InvalidState("Invalid state: queue is empty.");
    }
}

template <class T>
T SynchronizedQueue<T>::back() throw(InvalidState)
{
    epics::pvData::Lock lock(mutex);
    throwInvalidStateIfEmpty();
    return std::queue<T>::back();
}

template <class T>
T SynchronizedQueue<T>::front() throw(InvalidState)
{
    epics::pvData::Lock lock(mutex);
    throwInvalidStateIfEmpty();
    return std::queue<T>::front();
}

template <class T>
T SynchronizedQueue<T>::frontAndPopUnsynchronized() 
{
    T t = std::queue<T>::front();
    std::queue<T>::pop();
    itemPoppedEvent.signal();
    return t;
}

template <class T>
T SynchronizedQueue<T>::frontAndPop() throw(InvalidState)
{
    epics::pvData::Lock lock(mutex);
    throwInvalidStateIfEmpty();
    return frontAndPopUnsynchronized();
}

template <class T>
T SynchronizedQueue<T>::frontAndPop(double timeout) throw(InvalidState)
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
        itemPoppedEvent.signal();
    }
}

template <class T>
void SynchronizedQueue<T>::push(const T& t) throw(InvalidState)
{
    epics::pvData::Lock lock(mutex);
    int size = std::queue<T>::size();
    if (maxLength > 0 && size >= maxLength) {
        // We are full, throw exception
        throw InvalidState("Invalid state: queue is full.");
        return;
    }
    std::queue<T>::push(t);
    itemPushedEvent.signal();
}

template <class T>
void SynchronizedQueue<T>::pushIfNotFull(const T& t)
{
    epics::pvData::Lock lock(mutex);
    int size = std::queue<T>::size();
    if (maxLength > 0 && size >= maxLength) {
        // We are full, ignore push request
        return;
    }
    std::queue<T>::push(t);
    itemPushedEvent.signal();
}

template <class T>
void SynchronizedQueue<T>::waitForItemPushed(double timeout) 
{
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
    // Wait for pop only if we are full
    int size = std::queue<T>::size();
    if (maxLength > 0 && size >= maxLength) {
        itemPoppedEvent.wait(timeout);
    }
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

#endif
