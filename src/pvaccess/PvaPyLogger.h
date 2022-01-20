// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PVAPY_LOGGER_H
#define PVAPY_LOGGER_H

#include <string>
#include <cstdarg>
#include <cstdio>

#include <pv/logger.h>

class PvaPyLogger
{
public:
    enum LogLogLevel { 
        PVAPY_LOG_LEVEL_NONE = 0,
        PVAPY_LOG_LEVEL_CRITICAL = 1,
        PVAPY_LOG_LEVEL_ERROR = 2,
        PVAPY_LOG_LEVEL_WARN = 4,
        PVAPY_LOG_LEVEL_INFO = 8,
        PVAPY_LOG_LEVEL_DEBUG = 16,
        PVAPY_LOG_LEVEL_TRACE = 32,
        PVAPY_LOG_LEVEL_ALL = 255
    };

    static const char* LogLevelCritical;
    static const char* LogLevelError;
    static const char* LogLevelWarn;
    static const char* LogLevelInfo;
    static const char* LogLevelDebug;
    static const char* LogLevelTrace;

    static const int MaxTimeStampLength;
    static const char* LogLevelEnvVarName;
    static const char* EpicsLogLevelEnvVarName;
    static const char* TimeStampFormat;

    static void setLogFile(FILE* file);

    PvaPyLogger(const char* name);
    PvaPyLogger(const char* name, int logLevelMask);
    virtual ~PvaPyLogger();

    const char*  getName() const;
    int getLogLevelMask() const;
    void setLogLevelMask(int logLevel);
    void setLogLevelMaskFromEnvVar();
    bool hasLogLevel(int level) const;

    bool isEpicsLogEnabled() const;
    void setUseEpicsLog(bool useEpicsLog);
    void enableEpicsLog();
    void disableEpicsLog();

    virtual void critical(const std::string& message) const;
    virtual void critical(const char* message, ...) const;
    virtual void critical(const char* message, va_list messageArgs) const;

    virtual void error(const std::string& message) const;
    virtual void error(const char* message, ...) const;
    virtual void error(const char* message, va_list messageArgs) const;

    virtual void warn(const std::string& message) const;
    virtual void warn(const char* message, ...) const;
    virtual void warn(const char* message, va_list messageArgs) const;

    virtual void info(const std::string& message) const;
    virtual void info(const char* message, ...) const;
    virtual void info(const char* message, va_list messageArgs) const;

    virtual void debug(const std::string& message) const;
    virtual void debug(const char* message, ...) const;
    virtual void debug(const char* message, va_list messageArgs) const;

    virtual void trace(const std::string& message) const;
    virtual void trace(const char* message, ...) const;
    virtual void trace(const char* message, va_list messageArgs) const;

    virtual void log(const char* messageLogLevel, const char* message) const;
    virtual void log(const char* messageLogLevel, const char* message, va_list messageArgs) const;

private:
    static int getLogLevelMaskFromEnvVar();
    static epics::pvAccess::pvAccessLogLevel getEpicsLogLevelFromEnvVar();

    static void prepareTimeStamp(char* timeStamp, int timeStampLength, const char* timeStampFormat);
    static FILE* logFile;    
    static bool usePrintf;

    const char* name;
    int logLevelMask;
    bool useEpicsLog;
};

inline const char* PvaPyLogger::getName() const
{
    return name;
}

inline int PvaPyLogger::getLogLevelMask() const
{
    return logLevelMask;
}

inline void PvaPyLogger::setLogLevelMask(int mask) 
{
    logLevelMask = mask;
}

inline void PvaPyLogger::setLogLevelMaskFromEnvVar() 
{
    logLevelMask = getLogLevelMaskFromEnvVar();
}

inline bool PvaPyLogger::hasLogLevel(int level) const
{
    return ((logLevelMask & level) > 0);
}

inline bool PvaPyLogger::isEpicsLogEnabled() const
{
    return useEpicsLog;
}

inline void PvaPyLogger::setUseEpicsLog(bool value) 
{
    useEpicsLog = value;
}

inline void PvaPyLogger::enableEpicsLog() 
{
    useEpicsLog = true;
}

inline void PvaPyLogger::disableEpicsLog() 
{
    useEpicsLog = false;
}

#endif

