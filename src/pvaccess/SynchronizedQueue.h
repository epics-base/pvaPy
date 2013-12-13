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
    SynchronizedQueue();
    virtual ~SynchronizedQueue();
    T back() throw(InvalidState);
    T front() throw(InvalidState);
    T frontAndPop() throw(InvalidState);
    T frontAndPop(double timeout) throw(InvalidState);
    void pop();
    void push(const T& t);
    void waitForItem(double timeout);
    void cancelWaitForItem();
private:
    void throwInvalidStateIfEmpty() throw(InvalidState);
    T frontAndPopUnsynchronized();

    epics::pvData::Mutex mutex;
    epicsEvent event;
};

template <class T>
SynchronizedQueue<T>::SynchronizedQueue() :
    std::queue<T>(),
    mutex(),
    event()
{
}

template <class T>
SynchronizedQueue<T>::~SynchronizedQueue()
{
    event.signal();
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

#endif
