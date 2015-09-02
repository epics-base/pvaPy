// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef INVALID_REQUEST_H
#define INVALID_REQUEST_H

#include <string>
#include "PvaException.h"

/**
 * Invalid request error.
 */
class InvalidRequest : public PvaException
{
public:
    static const char* PyExceptionClassName;

    InvalidRequest(const std::string& message=""); 
    InvalidRequest(const char* message, ...);
    virtual const char* getPyExceptionClassName() const;
};

inline const char* InvalidRequest::getPyExceptionClassName() const
{
        return InvalidRequest::PyExceptionClassName;
}
#endif 
