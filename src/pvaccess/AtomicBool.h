// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef ATOMIC_BOOL_H
#define ATOMIC_BOOL_H
#include <epicsMutex.h>

// TODO: detect & use std::atomic<bool> 

class AtomicBool {

public:

    AtomicBool(bool v=false)
        : _value(v)
    {}

    AtomicBool& operator=(bool v)
    {
        _mutex.lock();
        _value = v;
        _mutex.unlock();
        return *this;
    }

    bool operator==(bool v)
    {
        bool r;
        _mutex.lock();
        r = (_value == v);
        _mutex.unlock();
        return r;
    }

    bool operator!=(bool v)
    {
        return !operator==(v);
    }

    operator bool() const {
        return _value;
    }   

private:
    bool _value;
    epicsMutex _mutex;
};

#endif // ATOMIC_BOOL_H
