// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef INVALID_ARGUMENT_H
#define INVALID_ARGUMENT_H

#include <string>
#include "PvaException.h"

/**
 * Invalid argument error.
 */
class InvalidArgument : public PvaException
{
public:
    static const char* PyExceptionClassName;

    InvalidArgument(const std::string& message=""); 
    InvalidArgument(const char* message, ...);
    virtual const char* getPyExceptionClassName() const;
};

inline const char* InvalidArgument::getPyExceptionClassName() const
{
    return InvalidArgument::PyExceptionClassName;
}

#endif 
