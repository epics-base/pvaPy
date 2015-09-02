// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "epicsStdio.h"
#include "ChannelNotFound.h"

const char* ChannelNotFound::PyExceptionClassName = "ChannelNotFound";

ChannelNotFound::ChannelNotFound(const std::string& message) :
    ChannelAccessError(message)
{
}

ChannelNotFound::ChannelNotFound(const char* message, ...) :
    ChannelAccessError()
{
    va_list messageArgs;
    va_start(messageArgs, message);
    char messageBuffer[MaxMessageLength];
    epicsVsnprintf(messageBuffer, MaxMessageLength, message, messageArgs);
    setMessage(std::string(messageBuffer));
    va_end(messageArgs);
}

