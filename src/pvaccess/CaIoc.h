// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef CA_IOC_H
#define CA_IOC_H

#include <boost/python/list.hpp>
#include <iocInit.h>
#include "PvaPyLogger.h"

class CaIoc
{
public:
    CaIoc();
    virtual ~CaIoc();

    virtual void start();
    virtual void stop();

    virtual void loadDatabase(const std::string& fileName, const std::string& path, const std::string& substitutions); 
    virtual void registerRecordDeviceDriver();
    virtual void loadRecords(const std::string& fileName, const std::string& substitutions);
    virtual boost::python::list getRecordNames();
    virtual void putField(const std::string& name, const std::string& value);
    virtual void printRecord(const std::string& name, int level);

    // Wrappers for calls available in iocsh
    virtual int dbLoadDatabase(const std::string& fileName, const std::string& path, const std::string& substitutions); 
    virtual int dbLoadRecords(const std::string& fileName, const std::string& substitutions);
    virtual int pvapyRegisterRecordDeviceDriver();
    virtual int iocInit();
    virtual int iocShutdown();
    virtual int dbl(const std::string& recordType, const std::string& fields);
    virtual int dbpr(const std::string& name, int level);
    virtual int dbgf(const std::string& name);
    virtual int dbpf(const std::string& name, const std::string& value);

private:
    static PvaPyLogger logger;
};

#endif
