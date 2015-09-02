// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef OBJECT_NOT_FOUND_H
#define OBJECT_NOT_FOUND_H

#include <string>
#include "PvaException.h"

/**
 * Object not found error.
 */
class ObjectNotFound : public PvaException
{
public:
    static const char* PyExceptionClassName;

    ObjectNotFound(const std::string& message=""); 
    ObjectNotFound(const char* message, ...);
    virtual const char* getPyExceptionClassName() const;
};

inline const char* ObjectNotFound::getPyExceptionClassName() const
{
    return ObjectNotFound::PyExceptionClassName;
}

#endif 
