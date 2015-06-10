#include "epicsStdio.h"
#include "FieldNotFound.h"

const char* FieldNotFound::PyExceptionClassName = "FieldNotFound";

FieldNotFound::FieldNotFound(const std::string& message) :
    PvaException(message)
{
}

FieldNotFound::FieldNotFound(const char* message, ...) :
    PvaException()
{
    va_list messageArgs;
    va_start(messageArgs, message);
    char messageBuffer[MaxMessageLength];
    epicsVsnprintf(messageBuffer, MaxMessageLength, message, messageArgs);
    setMessage(std::string(messageBuffer));
    va_end(messageArgs);
}

