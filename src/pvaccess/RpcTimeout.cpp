#include "epicsStdio.h"
#include "RpcTimeout.h"

RpcTimeout::RpcTimeout(const std::string& message) :
    PvaException(message)
{
}

RpcTimeout::RpcTimeout(const char* message, ...) :
    PvaException()
{
    va_list messageArgs;
    va_start(messageArgs, message);
    char messageBuffer[MaxMessageLength];
    epicsVsnprintf(messageBuffer, MaxMessageLength, message, messageArgs);
    setMessage(std::string(messageBuffer));
    va_end(messageArgs);
}

