// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include <map>
#include "PvObjectQueue.h"
#include "PyGilManager.h"
#include "PyUtility.h"
#include "QueueEmpty.h"
#include "QueueFull.h"

namespace bp = boost::python;

PvObjectQueue::PvObjectQueue(int maxLength)
    : sQueuePtr(new SynchronizedQueue<PvObject>(maxLength))
{
    PyGilManager::evalInitThreads();
}

PvObjectQueue::PvObjectQueue(const PvObjectQueue& q) 
    : sQueuePtr(q.sQueuePtr)
{
    PyGilManager::evalInitThreads();
}

PvObjectQueue::~PvObjectQueue()
{
}

PvObject PvObjectQueue::get() 
{
    return sQueuePtr->frontAndPop();
}

PvObject PvObjectQueue::get(double timeout) 
{
    PyThreadState *state;
    state = PyEval_SaveThread();
    try {
        PvObject pvObject = sQueuePtr->frontAndPop(timeout);
        PyEval_RestoreThread(state);
        return pvObject;
    }
    catch (const QueueEmpty& ex) {
        PyEval_RestoreThread(state);
        throw;
    }
    catch (...) {
        PyEval_RestoreThread(state);
        throw PvaException("Unexpected error caught in PvObjectQueue::get().");
    }
}

void PvObjectQueue::put(const PvObject& pvObject) 
{
    sQueuePtr->push(pvObject);
}

void PvObjectQueue::put(const PvObject& pvObject, double timeout) 
{
    PyThreadState *state;
    state = PyEval_SaveThread();
    try {
        sQueuePtr->push(pvObject, timeout);
        PyEval_RestoreThread(state);
        return;
    }
    catch (const QueueFull& ex) {
        PyEval_RestoreThread(state);
        throw;
    }
    catch (...) {
        PyEval_RestoreThread(state);
        throw PvaException("Unexpected error caught in PvObjectQueue::put().");
    }
}

void PvObjectQueue::waitForPut(double timeout) 
{
    PyThreadState *state;
    state = PyEval_SaveThread();
    try {
        sQueuePtr->waitForItemPushed(timeout);
        PyEval_RestoreThread(state);
    }
    catch (...) {
        PyEval_RestoreThread(state);
        throw PvaException("Unexpected error caught in PvObjectQueue::waitForPush().");
    }
}

void PvObjectQueue::waitForGet(double timeout) 
{
    PyThreadState *state;
    state = PyEval_SaveThread();
    try {
        sQueuePtr->waitForItemPopped(timeout);
        PyEval_RestoreThread(state);
    }
    catch (...) {
        PyEval_RestoreThread(state);
        throw PvaException("Unexpected error caught in PvObjectQueue::waitForPop().");
    }
}

bp::dict PvObjectQueue::getCounters()
{
    const std::map<std::string,unsigned int> counterMap = sQueuePtr->getCounterMap();
    return PyUtility::mapToDict<std::string,unsigned int>(counterMap);
}

