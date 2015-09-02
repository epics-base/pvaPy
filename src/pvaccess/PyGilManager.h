// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PY_GIL_MANAGER_H
#define PY_GIL_MANAGER_H

#include "boost/python.hpp"

class PyGilManager
{
public:
    static void evalInitThreads();
    static void gilStateEnsure();
    static void gilStateRelease();
private:
    static bool threadsInitialized;
    static PyGILState_STATE gilState;
};

#endif // #ifndef PY_GIL_MANAGER_H
