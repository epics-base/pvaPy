#include "epicsStdio.h"
#include "ChannelNotFound.h"

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

