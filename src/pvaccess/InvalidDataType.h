// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef INVALID_DATA_TYPE_H
#define INVALID_DATA_TYPE_H

#include <string>
#include "PvaException.h"

/**
 * Invalid data type error.
 */
class InvalidDataType : public PvaException
{
public:
    static const char* PyExceptionClassName;

    InvalidDataType(const std::string& message=""); 
    InvalidDataType(const char* message, ...);
    virtual const char* getPyExceptionClassName() const;
};

inline const char* InvalidDataType::getPyExceptionClassName() const
{
    return InvalidDataType::PyExceptionClassName;
}

#endif 
