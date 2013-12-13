#include "epicsStdio.h"
#include "InvalidState.h"

InvalidState::InvalidState(const std::string& message) :
    PvaException(message)
{
}

InvalidState::InvalidState(const char* message, ...) :
    PvaException()
{
    va_list messageArgs;
    va_start(messageArgs, message);
    char messageBuffer[MaxMessageLength];
    epicsVsnprintf(messageBuffer, MaxMessageLength, message, messageArgs);
    setMessage(std::string(messageBuffer));
    va_end(messageArgs);
}

