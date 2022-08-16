// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef CA_IOC_H
#define CA_IOC_H

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
    virtual void registerDeviceDriver();
    virtual void loadRecords(const std::string& fileName, const std::string& substitutions);
    virtual void listRecords(const std::string& recordType, const std::string& fields);
    virtual void putField(const std::string& name, const std::string& value);
    virtual void printRecord(const std::string& name, int level);

private:
    static PvaPyLogger logger;
};

#endif
