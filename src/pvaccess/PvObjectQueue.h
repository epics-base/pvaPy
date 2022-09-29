// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PV_OBJECT_QUEUE_H
#define PV_OBJECT_QUEUE_H

#include <string>

#include "boost/python/dict.hpp"
#include "PvObject.h"
#include "SynchronizedQueue.h"

// Wrapper around SynchronizedQueue<PvObject>
// We cannot use inheritance because this object
// may be created in python and passed to the C++ layer, and the same
// events/mutexes must work both in python and C++
class PvObjectQueue 
{
public:
    POINTER_DEFINITIONS(PvObjectQueue);

    PvObjectQueue(int maxLength=SynchronizedQueue<PvObject>::Unlimited);
    PvObjectQueue(const PvObjectQueue& pvObjectQueue);
    virtual ~PvObjectQueue();

    void setMaxLength(int maxLength) { sQueuePtr->setMaxLength(maxLength); }
    int getMaxLength() { return sQueuePtr->getMaxLength(); }
    bool isFull() { return sQueuePtr->isFull(); }
    bool isEmpty() { return sQueuePtr->isEmpty(); }
    unsigned int size() { return sQueuePtr->size(); }
    PvObject back() { return sQueuePtr->back(); } 
    PvObject front() { return sQueuePtr->front(); } 
    PvObject frontAndPop() { return sQueuePtr->frontAndPop(); } 
    PvObject frontAndPop(double timeout) { return sQueuePtr->frontAndPop(timeout); } 
    void pop() { sQueuePtr->pop(); } 
    void push(const PvObject& pvObject) { sQueuePtr->push(pvObject); } 
    void push(const PvObject& pvObject, double timeout) { sQueuePtr->push(pvObject, timeout); } 
    bool popIfNotEmpty() { return sQueuePtr->popIfNotEmpty(); } 
    bool pushIfNotFull(const PvObject& pvObject) { return sQueuePtr->pushIfNotFull(pvObject); } 

    void waitForItemPushed(double timeout) { sQueuePtr->waitForItemPushed(timeout); }
    void waitForItemPushedIfEmpty(double timeout) { sQueuePtr->waitForItemPushedIfEmpty(timeout); }
    void waitForItemPopped(double timeout) { sQueuePtr->waitForItemPopped(timeout); }
    void waitForItemPoppedIfFull(double timeout) { sQueuePtr->waitForItemPoppedIfFull(timeout); }
    void cancelWaitForItemPushed() { sQueuePtr->cancelWaitForItemPushed(); }
    void cancelWaitForItemPopped() { sQueuePtr->cancelWaitForItemPopped(); }
    void clear() { sQueuePtr->clear(); }

    void resetCounters() { sQueuePtr->resetCounters(); }
    const std::map<std::string,unsigned int>& getCounterMap() { return sQueuePtr->getCounterMap(); }
    void setCounter(const std::string& key, unsigned int value) { sQueuePtr->setCounter(key, value); }
    void addToCounter(const std::string& key, unsigned int value) { sQueuePtr->addToCounter(key, value); }
    double getTimeSinceLastPush() { return sQueuePtr->getTimeSinceLastPush(); }
    double getTimeSinceLastPop() { return sQueuePtr->getTimeSinceLastPop(); }

    // Python interface
    PvObject get();
    PvObject get(double timeout);
    void put(const PvObject& pvObject);
    void put(const PvObject& pvObject, double timeout);
    virtual void waitForPut(double timeout);
    virtual void waitForGet(double timeout);
    void cancelWaitForPut() { sQueuePtr->cancelWaitForItemPushed(); }
    void cancelWaitForGet() { sQueuePtr->cancelWaitForItemPopped(); }
    virtual boost::python::dict getCounters();
private:
    std::tr1::shared_ptr<SynchronizedQueue<PvObject> > sQueuePtr;
};

#endif
