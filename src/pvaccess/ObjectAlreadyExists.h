// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef OBJECT_ALREADY_EXISTS_H
#define OBJECT_ALREADY_EXISTS_H

#include <string>
#include "PvaException.h"

/**
 * Object already exists error.
 */
class ObjectAlreadyExists : public PvaException
{
public:
    static const char* PyExceptionClassName;

    ObjectAlreadyExists(const std::string& message=""); 
    ObjectAlreadyExists(const char* message, ...);
    virtual const char* getPyExceptionClassName() const;
};

inline const char* ObjectAlreadyExists::getPyExceptionClassName() const
{
    return ObjectAlreadyExists::PyExceptionClassName;
}

#endif 
