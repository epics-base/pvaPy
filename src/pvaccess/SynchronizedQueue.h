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
    SynchronizedQueue();
    virtual ~SynchronizedQueue();
    void setMaxLength(int maxLength);
    int getMaxLength();
    T back() throw(InvalidState);
    T front() throw(InvalidState);
    T frontAndPop() throw(InvalidState);
    T frontAndPop(double timeout) throw(InvalidState);
    void pop();
    void push(const T& t);
    void waitForItem(double timeout);
    void cancelWaitForItem();
    void clear();

private:
    void throwInvalidStateIfEmpty() throw(InvalidState);
    T frontAndPopUnsynchronized();

    epics::pvData::Mutex mutex;
    epicsEvent event;
    int maxLength;
};

template <class T>
SynchronizedQueue<T>::SynchronizedQueue() :
    std::queue<T>(),
    mutex(),
    event(),
    maxLength(Unlimited)
{
}

template <class T>
SynchronizedQueue<T>::~SynchronizedQueue()
{
    event.signal();
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
    waitForItem(timeout);
    return frontAndPop();
}

template <class T>
void SynchronizedQueue<T>::pop()
{
    epics::pvData::Lock lock(mutex);
    if (!std::queue<T>::empty()) {
        std::queue<T>::pop();
    }
}

template <class T>
void SynchronizedQueue<T>::push(const T& t)
{
    epics::pvData::Lock lock(mutex);
    if (maxLength > 0) {
        int nPop = std::queue<T>::size()-maxLength+1;
        for (int i = 0; i < nPop; i++) {
            std::queue<T>::pop();
        }
    }
    std::queue<T>::push(t);
    event.signal();
}

template <class T>
void SynchronizedQueue<T>::waitForItem(double timeout) 
{
    event.wait(timeout);
}

template <class T>
void SynchronizedQueue<T>::cancelWaitForItem() 
{
    event.signal();
}

template <class T>
void SynchronizedQueue<T>::clear() 
{
    epics::pvData::Lock lock(mutex);
    while (!std::queue<T>::empty()) {
        std::queue<T>::pop();
    }
    event.signal();
}

#endif
