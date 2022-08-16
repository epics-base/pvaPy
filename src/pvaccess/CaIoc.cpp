// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution


#include <dbAccess.h>
#include <dbTest.h>
#include "CaIoc.h"

extern "C" int pvapy_registerRecordDeviceDriver(struct dbBase *pbase);

PvaPyLogger CaIoc::logger("CaIoc");

CaIoc::CaIoc() 
{
}

CaIoc::~CaIoc() 
{
    stop();
}

void CaIoc::start() 
{
    iocInit();
}

void CaIoc::stop() 
{
}

void CaIoc::loadDatabase(const std::string& fileName, const std::string& path, const std::string& substitutions)
{
    dbLoadDatabase(fileName.c_str(), path.c_str(), substitutions.c_str());
}

void CaIoc::registerDeviceDriver()
{
    pvapy_registerRecordDeviceDriver(pdbbase);
}

void CaIoc::loadRecords(const std::string& fileName, const std::string& substitutions)
{
    dbLoadRecords(fileName.c_str(), substitutions.c_str());
}

void CaIoc::listRecords(const std::string& recordType, const std::string& fields)
{
    dbl(recordType.c_str(), fields.c_str());
}

void CaIoc::putField(const std::string& name, const std::string& value)
{
    dbpf(name.c_str(), value.c_str());
}

void CaIoc::printRecord(const std::string& name, int level)
{
    dbpr(name.c_str(), level);
}

