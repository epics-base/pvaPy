// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef INVALID_STATE_H
#define INVALID_STATE_H

#include <string>
#include "PvaException.h"

/**
 * Invalid state error.
 */
class InvalidState : public PvaException
{
public:
    static const char* PyExceptionClassName;

    InvalidState(const std::string& message=""); 
    InvalidState(const char* message, ...);
    virtual const char* getPyExceptionClassName() const;
};

inline const char* InvalidState::getPyExceptionClassName() const
{
    return InvalidState::PyExceptionClassName;
}

#endif 
