// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "epicsStdio.h"
#include "InvalidDataType.h"

const char* InvalidDataType::PyExceptionClassName = "InvalidDataType";

InvalidDataType::InvalidDataType(const std::string& message) :
    PvaException(message)
{
}

InvalidDataType::InvalidDataType(const char* message, ...) :
    PvaException()
{
    va_list messageArgs;
    va_start(messageArgs, message);
    char messageBuffer[MaxMessageLength];
    epicsVsnprintf(messageBuffer, MaxMessageLength, message, messageArgs);
    setMessage(std::string(messageBuffer));
    va_end(messageArgs);
}

