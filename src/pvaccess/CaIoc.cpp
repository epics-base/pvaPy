// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution


#include <dbAccess.h>
#include <dbTest.h>
#include <dbStaticLib.h>
#include "InvalidState.h"
#include "InvalidArgument.h"
#include "InvalidRequest.h"
#include "ObjectNotFound.h"
#include "StringUtility.h"
#include "CaIoc.h"

namespace bp = boost::python;

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
    int status = ::iocInit();
    if (status) {
        throw InvalidState("iocInit() failed with status of " + StringUtility::toString<int>(status));
    }
}

void CaIoc::stop() 
{
    int status = ::iocShutdown();
    if (status) {
        throw InvalidState("iocShutdown() failed with status of " + StringUtility::toString<int>(status));
    }
}

void CaIoc::loadDatabase(const std::string& fileName, const std::string& path, const std::string& substitutions)
{
    if (fileName.size() == 0) {
        throw InvalidArgument("File name cannot be empty.");
    }
    int status = ::dbReadDatabase(&pdbbase, fileName.c_str(), path.c_str(), substitutions.c_str());
    if (status) {
        throw InvalidState("dbReadDatabase() failed with status of " + StringUtility::toString<int>(status));
    }
}

void CaIoc::registerRecordDeviceDriver()
{
    int status = pvapy_registerRecordDeviceDriver(pdbbase);
    if (status) {
        throw InvalidState("pvapy_registerRecordDeviceDriver() failed with status of " + StringUtility::toString<int>(status));
    }
}

void CaIoc::loadRecords(const std::string& fileName, const std::string& substitutions)
{
    if (fileName.size() == 0) {
        throw InvalidArgument("File name cannot be empty.");
    }
    int status = ::dbReadDatabase(&pdbbase, fileName.c_str(), 0, substitutions.c_str());
    switch (status) {
        case 0: {
            if(dbLoadRecordsHook) {
                dbLoadRecordsHook(fileName.c_str(), substitutions.c_str());
            }
            break;
        }
        case -2: {
            throw InvalidState("Records cannot be read after ioc was initialized, failed to load" + fileName);
            break;
        }
        default: {
            throw InvalidState("dbReadDatabase() failed with status " + StringUtility::toString<int>(status) + " for file " + fileName);
        }
    }
}

// Modified from the dbl() code in dbTest.h to return list of record names
bp::list CaIoc::getRecordNames()
{
    bp::list recordNames;

    DBENTRY dbentry;
    DBENTRY *pdbentry=&dbentry;
    int status;

    if (!pdbbase) {
        throw InvalidState("No database loaded.");
    }
    dbInitEntry(pdbbase, pdbentry);
    while (!status) {
        status = dbFirstRecord(pdbentry);
        while (!status) {
            recordNames.append(dbGetRecordName(pdbentry));
            status = dbNextRecord(pdbentry);
        }
    }
    dbFinishEntry(pdbentry);
    return recordNames;
}

void CaIoc::putField(const std::string& name, const std::string& value)
{
    if (name.size() == 0) {
        throw InvalidArgument("Record name cannot be empty.");
    }

    int status = ::dbpf(name.c_str(), value.c_str());
    switch (status) {
        case 0: {
            // ok
            break;
        }
        case -1: {
            throw ObjectNotFound("Record " + name + " not found");
        }
        default: {
            throw InvalidState("dbpf() failed with status of " + StringUtility::toString<int>(status));
        }
    }
}

void CaIoc::printRecord(const std::string& name, int level)
{
    if (name.size() == 0) {
        throw InvalidArgument("Record name cannot be empty.");
    }

    int status = ::dbpr(name.c_str(), level);
    switch (status) {
        case 0: {
            // ok
            break;
        }
        case -1: {
            throw ObjectNotFound("Record " + name + " not found");
        }
        default: {
            throw InvalidState("dbpr() failed with status of " + StringUtility::toString<int>(status));
        }
    }
}

//
// Wrappers for calls available in iocsh
//
int CaIoc::dbLoadDatabase(const std::string& fileName, const std::string& path, const std::string& substitutions)
{
    return ::dbLoadDatabase(fileName.c_str(), path.c_str(), substitutions.c_str());
}

int CaIoc::dbLoadRecords(const std::string& fileName, const std::string& substitutions)
{
    return ::dbLoadRecords(fileName.c_str(), substitutions.c_str());
}

int CaIoc::iocInit() 
{
    return ::iocInit();
}

int CaIoc::iocShutdown() 
{
    return ::iocShutdown();
}

int CaIoc::pvapyRegisterRecordDeviceDriver()
{
    return pvapy_registerRecordDeviceDriver(pdbbase);
}

int CaIoc::dbl(const std::string& recordType, const std::string& fields)
{
    return ::dbl(recordType.c_str(), fields.c_str());
}

int CaIoc::dbpr(const std::string& name, int level)
{
   return ::dbpr(name.c_str(), level);
}

int CaIoc::dbgf(const std::string& name)
{
    return ::dbgf(name.c_str());
}

int CaIoc::dbpf(const std::string& name, const std::string& value)
{
    return ::dbpf(name.c_str(), value.c_str());
}
