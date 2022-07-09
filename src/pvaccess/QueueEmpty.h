// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef QUEUE_EMPTY_H
#define QUEUE_EMPTY_H

#include <string>
#include "PvaException.h"

/**
 * Queue empty error.
 */
class QueueEmpty : public PvaException
{
public:
    static const char* PyExceptionClassName;

    QueueEmpty(const std::string& message=""); 
    QueueEmpty(const char* message, ...);
    virtual const char* getPyExceptionClassName() const;
};

inline const char* QueueEmpty::getPyExceptionClassName() const
{
        return QueueEmpty::PyExceptionClassName;
}
#endif 
