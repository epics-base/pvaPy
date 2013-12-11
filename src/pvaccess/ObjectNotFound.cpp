#include "epicsStdio.h"
#include "ObjectNotFound.h"

ObjectNotFound::ObjectNotFound(const std::string& message) :
    PvaException(message)
{
}

ObjectNotFound::ObjectNotFound(const char* message, ...) :
    PvaException()
{
    va_list messageArgs;
    va_start(messageArgs, message);
    char messageBuffer[MaxMessageLength];
    epicsVsnprintf(messageBuffer, MaxMessageLength, message, messageArgs);
    setMessage(std::string(messageBuffer));
    va_end(messageArgs);
}

