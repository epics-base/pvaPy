#include "epicsStdio.h"
#include "InvalidArgument.h"

InvalidArgument::InvalidArgument(const std::string& message) :
    PvaException(message)
{
}

InvalidArgument::InvalidArgument(const char* message, ...) :
    PvaException()
{
    va_list messageArgs;
    va_start(messageArgs, message);
    char messageBuffer[MaxMessageLength];
    epicsVsnprintf(messageBuffer, MaxMessageLength, message, messageArgs);
    setMessage(std::string(messageBuffer));
    va_end(messageArgs);
}

