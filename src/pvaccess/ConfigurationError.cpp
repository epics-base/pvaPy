// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "epicsStdio.h"
#include "ConfigurationError.h"

const char* ConfigurationError::PyExceptionClassName = "ConfigurationError";

ConfigurationError::ConfigurationError(const std::string& message) :
    PvaException(message)
{
}

ConfigurationError::ConfigurationError(const char* message, ...) :
    PvaException()
{
    va_list messageArgs;
    va_start(messageArgs, message);
    char messageBuffer[MAX_MESSAGE_LENGTH];
    epicsVsnprintf(messageBuffer, MAX_MESSAGE_LENGTH, message, messageArgs);
    setMessage(std::string(messageBuffer));
    va_end(messageArgs);
}

