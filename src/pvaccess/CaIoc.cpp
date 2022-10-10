// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution


#include <algorithm>
#include <dbAccess.h>
#include <dbTest.h>
#include <dbStaticLib.h>
#include <dbConvertJSON.h>

#include "InvalidState.h"
#include "InvalidArgument.h"
#include "InvalidRequest.h"
#include "ObjectNotFound.h"
#include "StringUtility.h"
#include "PyUtility.h"
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

void CaIoc::getRecordDbAddr(const std::string& name, DBADDR *dbAddr)
{
    int status = dbNameToAddr(name.c_str(), dbAddr);

    if (status) {
        throw ObjectNotFound("Record " + name + " not found");
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

void CaIoc::putField(const std::string& name, const bp::object& pyValue)
{
    std::string value = PyUtility::extractStringFromPyObject(pyValue);
    putField(name, value);
}

void CaIoc::putField(const std::string& name, const std::string& value)
{
    if (name.size() == 0) {
        throw InvalidArgument("Record name cannot be empty.");
    }

    if (value.size() == 0) {
        throw InvalidArgument("Record value cannot be empty.");
    }

    DBADDR addr;
    int status;
    short dbrType = DBR_STRING;
    long n = 1;
    char *array = NULL;

    getRecordDbAddr(name, &addr);

    if (addr.precord->lset == NULL) {
        throw InvalidState("Record " + name + " cannot be set before ioc is initialized");
    }

    if (addr.no_elements > 1) {
        dbrType = addr.dbr_field_type;
        if (addr.dbr_field_type == DBR_CHAR || addr.dbr_field_type == DBR_UCHAR) {
            n = value.size() + 1;
        } 
        else {
            n = addr.no_elements;
            array = (char*)calloc(n, dbValueSize(dbrType));
            if (!array) {
                throw InvalidState("Record " + name + " cannot be set (out of memory)");
            }
            status = dbPutConvertJSON(value.c_str(), dbrType, array, &n);
            if (status) {
                throw InvalidState("dbPutConvertJSON() failed with status of " + StringUtility::toString<int>(status));
            }
            status = dbPutField(&addr, dbrType, array, n);
            free(array);
        }
    }
    else {
        status = dbPutField(&addr, dbrType, value.c_str(), n);
    }
    if (status) {
        throw InvalidState("dbPutField() failed with status of " + StringUtility::toString<int>(status));
    }
}

bp::object CaIoc::getField(const std::string& name)
{
    if (name.size() == 0) {
        throw InvalidArgument("Record name cannot be empty.");
    }

    DBADDR addr;
    getRecordDbAddr(name, &addr);
    long nElements = addr.no_elements; 
    int bufferSize = std::max(static_cast<int>(nElements*addr.field_size), MAX_STRING_SIZE);

    char buffer[bufferSize];
    char *pBuffer = &buffer[0];

    long options = 0;
    long status = dbGetField(&addr, addr.dbr_field_type, pBuffer,
            &options, &nElements, NULL);
    if (status) {
        throw InvalidState("dbGetField() failed with status of " + StringUtility::toString<int>(status));
    }

    bp::list pyList;
    switch (addr.dbr_field_type) {
        case DBR_STRING: {
            for(int i = 0; i < nElements; i++) {
                pyList.append(std::string(pBuffer));
                pBuffer = (char*)pBuffer + MAX_STRING_SIZE;
            }
            break;
        }

        case DBR_CHAR: {
            bufferToPyList<epicsInt8>(pBuffer, pyList, nElements);
            break;
        }

        case DBR_UCHAR: {
            bufferToPyList<epicsUInt8>(pBuffer, pyList, nElements);
        }

        case DBR_SHORT: {
            bufferToPyList<epicsInt16>(pBuffer, pyList, nElements);
            break;
        }

        case DBR_USHORT: {
            bufferToPyList<epicsUInt16>(pBuffer, pyList, nElements);
            break;
        }

        case DBR_LONG: {
            bufferToPyList<epicsInt32>(pBuffer, pyList, nElements);
            break;
        }

        case DBR_ULONG: {
            bufferToPyList<epicsUInt32>(pBuffer, pyList, nElements);
            break;
        }

        case DBR_INT64: {
            bufferToPyList<epicsInt64>(pBuffer, pyList, nElements);
            break;
        }

        case DBR_UINT64: {
            bufferToPyList<epicsUInt64>(pBuffer, pyList, nElements);
            break;
        }

        case DBR_FLOAT: {
            bufferToPyList<epicsFloat32>(pBuffer, pyList, nElements);
            break;
        }

        case DBR_DOUBLE: {
            bufferToPyList<epicsFloat64>(pBuffer, pyList, nElements);
            break;
        }

        case DBR_ENUM: {
            bufferToPyList<epicsEnum16>(pBuffer, pyList, nElements);
            break;
        }
        default: {
            throw InvalidState("dbGetField() returned unknown record type " + StringUtility::toString<int>(addr.dbr_field_type));
            break;
        }
    }
    if (nElements == 1) {
        return pyList[0];
    }
    return pyList;
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
