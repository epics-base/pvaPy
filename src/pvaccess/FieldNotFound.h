// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef FIELD_NOT_FOUND_H
#define FIELD_NOT_FOUND_H

#include <string>
#include "PvaException.h"

/**
 * Field not found error.
 */
class FieldNotFound : public PvaException
{
public:
    static const char* PyExceptionClassName;

    FieldNotFound(const std::string& message=""); 
    FieldNotFound(const char* message, ...);
    virtual const char* getPyExceptionClassName() const;
};

inline const char* FieldNotFound::getPyExceptionClassName() const
{
        return FieldNotFound::PyExceptionClassName;
}

#endif 
