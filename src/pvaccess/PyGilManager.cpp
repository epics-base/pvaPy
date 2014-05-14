#include "PyGilManager.h"

bool PyGilManager::threadsInitialized(false);
PyGILState_STATE PyGilManager::gilState;

void PyGilManager::evalInitThreads()
{
    if (!threadsInitialized) {
        threadsInitialized = true;
        PyEval_InitThreads();
    }
}

void PyGilManager::gilStateEnsure()
{
    if (threadsInitialized) {
        gilState = PyGILState_Ensure();
    }
}

void PyGilManager::gilStateRelease()
{
    if (threadsInitialized) {
        PyGILState_Release(gilState);
    }
}

