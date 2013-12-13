
#include <stdlib.h>
#include <stdio.h>

#include "epicsTime.h"
#include "errlog.h"
#include "PvaPyLogger.h"

// Constants.
const char* PvaPyLogger::LogLevelCritical("CRITICAL");
const char* PvaPyLogger::LogLevelError("ERROR");
const char* PvaPyLogger::LogLevelWarn("WARN");
const char* PvaPyLogger::LogLevelInfo("INFO");
const char* PvaPyLogger::LogLevelDebug("DEBUG");
const char* PvaPyLogger::LogLevelTrace("TRACE");

const int PvaPyLogger::MaxTimeStampLength(64);
const char* PvaPyLogger::LogLevelEnvVarName("PVA_PY_LOG_LEVEL");
const char* PvaPyLogger::TimeStampFormat("%Y/%m/%d %H:%M:%S.%03f");

FILE* PvaPyLogger::logFile(stdout);
bool PvaPyLogger::usePrintf(true);

// Static methods.
void PvaPyLogger::setLogFile(FILE* file) 
{
    logFile = file;
    if (logFile != stdout) {
        usePrintf = false;
    }
}

int PvaPyLogger::getLogLevelMaskFromEnvVar()
{
    int logLevelMask = PVA_PY_LOG_LEVEL_NONE;
    const char* logLevelMaskString = getenv(LogLevelEnvVarName);
    if (logLevelMaskString) {
        logLevelMask = atoi(logLevelMaskString);
    }
    return logLevelMask;
}

PvaPyLogger::PvaPyLogger(const char* name_) :
    name(name_),
    logLevelMask(getLogLevelMaskFromEnvVar()),
    useEpicsLog(false)
{
}

PvaPyLogger::PvaPyLogger(const char* name_, int logLevelMask_) :
    name(name_),
    logLevelMask(logLevelMask_),
    useEpicsLog(false)
{
}

PvaPyLogger::~PvaPyLogger()
{
}

//
// Always log error and critical levels.
//

void PvaPyLogger::critical(const std::string& message) const
{
    log(LogLevelCritical, message.c_str());
}

void PvaPyLogger::critical(const char* message, ...) const
{
    va_list messageArgs;
    va_start(messageArgs, message);
    log(LogLevelCritical, message, messageArgs);
    va_end(messageArgs);
}

void PvaPyLogger::critical(const char* message, va_list messageArgs) const
{
    log(LogLevelCritical, message, messageArgs);
}

void PvaPyLogger::error(const std::string& message) const
{
    log(LogLevelError, message.c_str());
}

void PvaPyLogger::error(const char* message, ...) const
{
    va_list messageArgs;
    va_start(messageArgs, message);
    log(LogLevelError, message, messageArgs);
    va_end(messageArgs);
}

void PvaPyLogger::error(const char* message, va_list messageArgs) const
{
    log(LogLevelError, message, messageArgs);
}

void PvaPyLogger::warn(const std::string& message) const
{
    if (!(logLevelMask & PVA_PY_LOG_LEVEL_WARN)) {
        return;
    }
    log(LogLevelWarn, message.c_str());
}

void PvaPyLogger::warn(const char* message, ...) const
{
    if (!(logLevelMask & PVA_PY_LOG_LEVEL_WARN)) {
        return;
    }
    va_list messageArgs;
    va_start(messageArgs, message);
    log(LogLevelWarn, message, messageArgs);
    va_end(messageArgs);
}

void PvaPyLogger::warn(const char* message, va_list messageArgs) const
{
    if (!(logLevelMask & PVA_PY_LOG_LEVEL_WARN)) {
        return;
    }
    log(LogLevelWarn, message, messageArgs);
}

void PvaPyLogger::info(const std::string& message) const
{
    if (!(logLevelMask & PVA_PY_LOG_LEVEL_INFO)) {
        return;
    }
    log(LogLevelInfo, message.c_str());
}

void PvaPyLogger::info(const char* message, ...) const
{
    if (!(logLevelMask & PVA_PY_LOG_LEVEL_INFO)) {
        return;
    }
    va_list messageArgs;
    va_start(messageArgs, message);
    log(LogLevelInfo, message, messageArgs);
    va_end(messageArgs);
}

void PvaPyLogger::info(const char* message, va_list messageArgs) const
{
    if (!(logLevelMask & PVA_PY_LOG_LEVEL_INFO)) {
        return;
    }
    log(LogLevelInfo, message, messageArgs);
}

void PvaPyLogger::debug(const std::string& message) const
{
    if (!(logLevelMask & PVA_PY_LOG_LEVEL_DEBUG)) {
        return;
    }
    log(LogLevelDebug, message.c_str());
}

void PvaPyLogger::debug(const char* message, ...) const
{
    if (!(logLevelMask & PVA_PY_LOG_LEVEL_DEBUG)) {
        return;
    }
    va_list messageArgs;
    va_start(messageArgs, message);
    log(LogLevelDebug, message, messageArgs);
    va_end(messageArgs);
}

void PvaPyLogger::debug(const char* message, va_list messageArgs) const
{
    if (!(logLevelMask & PVA_PY_LOG_LEVEL_DEBUG)) {
        return;
    }
    log(LogLevelDebug, message, messageArgs);
}

void PvaPyLogger::trace(const std::string& message) const
{
    if (!(logLevelMask & PVA_PY_LOG_LEVEL_TRACE)) {
        return;
    }
    log(LogLevelTrace, message.c_str());
}

void PvaPyLogger::trace(const char* message, ...) const
{
    if (!(logLevelMask & PVA_PY_LOG_LEVEL_TRACE)) {
        return;
    }
    va_list messageArgs;
    va_start(messageArgs, message);
    log(LogLevelTrace, message, messageArgs);
    va_end(messageArgs);
}

void PvaPyLogger::trace(const char* message, va_list messageArgs) const
{
    if (!(logLevelMask & PVA_PY_LOG_LEVEL_TRACE)) {
        return;
    }
    log(LogLevelTrace, message, messageArgs);
}

void PvaPyLogger::log(const char* messageLevel, const char* message) const
{
    char timeStamp[MaxTimeStampLength];
    prepareTimeStamp(timeStamp, MaxTimeStampLength, TimeStampFormat);
    if (useEpicsLog) {
        errlogPrintf("%s %s %s:  %s\n", timeStamp, messageLevel, name, message);
    }
    else {
        // On vxWorks fflush() fails frequently, so only use
        // fprintf() if logging goes into a file.
        if (usePrintf) {
            printf("%s %s %s:  %s\n", timeStamp, messageLevel, name, message);
        }
        else {
            fprintf(logFile, "%s %s %s:  %s\n", timeStamp, messageLevel, name, message);
            fflush(logFile);
        }
    }
}

void PvaPyLogger::log(const char* messageLevel, const char* message, va_list messageArgs) const
{
    char timeStamp[MaxTimeStampLength];
    prepareTimeStamp(timeStamp, MaxTimeStampLength, TimeStampFormat);
    if (useEpicsLog) {
        errlogPrintf("%s %s %s:  ", timeStamp, messageLevel, name);
        errlogVprintf(message, messageArgs);
        errlogPrintf("\n");
    }
    else {
        // On vxWorks fflush() fails frequently, so only use
        // fprintf() if logging goes into a file.
        if (usePrintf) {
            printf("%s %s %s:  ", timeStamp, messageLevel, name);
            vprintf(message, messageArgs);
            printf("\n");
        }
        else {
            fprintf(logFile, "%s %s %s:  ", timeStamp, messageLevel, name);
            vfprintf(logFile, message, messageArgs);
            fprintf(logFile, "\n");
            fflush(logFile);
        }
    }
}

void PvaPyLogger::prepareTimeStamp(char* timeStamp, int timeStampLength, const char* timeStampFormat) 
{
    epicsTimeStamp now;
    epicsTimeGetCurrent(&now);
    epicsTimeToStrftime(timeStamp, timeStampLength, timeStampFormat, &now);
}

