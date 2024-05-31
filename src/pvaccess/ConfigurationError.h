// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef CONFIGURATION_ERROR_H
#define CONFIGURATION_ERROR_H

#include <string>
#include "PvaException.h"

/**
 * Configuration error.
 */
class ConfigurationError : public PvaException
{
public:
    static const char* PyExceptionClassName;

    ConfigurationError(const std::string& message=""); 
    ConfigurationError(const char* message, ...);
    virtual const char* getPyExceptionClassName() const;
};

inline const char* ConfigurationError::getPyExceptionClassName() const
{
        return ConfigurationError::PyExceptionClassName;
}
#endif 
