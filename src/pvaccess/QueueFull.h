// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef QUEUE_FULL_H
#define QUEUE_FULL_H

#include <string>
#include "PvaException.h"

/**
 * Queue full error.
 */
class QueueFull : public PvaException
{
public:
    static const char* PyExceptionClassName;

    QueueFull(const std::string& message=""); 
    QueueFull(const char* message, ...);
    virtual const char* getPyExceptionClassName() const;
};

inline const char* QueueFull::getPyExceptionClassName() const
{
        return QueueFull::PyExceptionClassName;
}
#endif 
