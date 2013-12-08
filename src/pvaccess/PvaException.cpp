#include <iostream>
#include "epicsStdio.h"
#include "PvaException.h"

// Constants.
const int PvaException::MaxMessageLength(1024);
const int PvaException::GenericErrorCode(1);

PvaException::PvaException(int errorCode_, const std::string& message) :
    std::exception(),
    error(message),
    errorCode(errorCode_)
{
}

PvaException::PvaException(const std::string& message) :
    std::exception(),
    error(message),
    errorCode(GenericErrorCode)
{
}

PvaException::PvaException(const char* message, ...) :
    std::exception(),
    error(),
    errorCode(GenericErrorCode)
{
    va_list messageArgs;
    va_start(messageArgs, message);
    char messageBuffer[MaxMessageLength];
    epicsVsnprintf(messageBuffer, MaxMessageLength, message, messageArgs);
    error = std::string(messageBuffer);
    va_end(messageArgs);
}

PvaException::PvaException(const char* message, va_list messageArgs) :
    std::exception(),
    error(),
    errorCode(GenericErrorCode)
{
    char messageBuffer[MaxMessageLength];
    epicsVsnprintf(messageBuffer, MaxMessageLength, message, messageArgs);
    error = std::string(messageBuffer);
}

PvaException::~PvaException() throw()
{
}

std::ostream& operator<<(std::ostream& out, const PvaException& ex) 
{
    out << ex.error.c_str();
    return out;
}
