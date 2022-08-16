// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution


#include <dbAccess.h>
#include <dbTest.h>
#include "InvalidState.h"
#include "InvalidArgument.h"
#include "InvalidRequest.h"
#include "ObjectNotFound.h"
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

// Modified from the dbl() code in dbTest.h to return list of record names
bp::list getRecordNames()
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
            recordNames.append(dbGetRecordName(pdbentry)));
            status = dbNextRecord(pdbentry);
        }
    }
    dbFinishEntry(pdbentry);
    return recordNames;
}

void CaIoc::putField(const std::string& name, const std::string& value)
{
    DBADDR addr;
    long status;
    short dbrType = DBR_STRING;
    long n = 1;
    char *array = NULL;
    const char* pname = name.c_str();
    const char* pvalue = value.c_str();

    if (name.size() == 0) {
        throw InvalidArgument("Record name cannot be empty.");
    }

    if (nameToAddr(pname, &addr)) {
        throw ObjectNotFound("Record name not found.");
    }

    if (addr.precord->lset == NULL) {
        throw InvalidState("IOC must be started before putField() can be called.");
    }

    if (addr.no_elements > 1) {
        dbrType = addr.dbr_field_type;
        if (addr.dbr_field_type == DBR_CHAR || addr.dbr_field_type == DBR_UCHAR) {
            n = (long)strlen(pvalue) + 1;
        } else {
            n = addr.no_elements;
            array = calloc(n, dbValueSize(dbrType));
            if (!array) {
                InvalidState("Out of memory.");
            }
            status = dbPutConvertJSON(pvalue, dbrType, array, &n);
            if (status) {
                throw InvalidRequest("dbPutConvertJSON returned status: " + StringUtility::toString<int>(status));
            }
            pvalue = array;
        }
    }
    status = dbPutField(&addr, dbrType, pvalue, n);
    if (status) {
        throw InvalidRequest("dbPutField returned status: " + StringUtility::toString<int>(status));
    }
    pvalue = array;
    free(array);
}

void CaIoc::printRecord(const std::string& name, int level)
{
    static TAB_BUFFER msg_Buff;
    TAB_BUFFER *pMsgBuff = &msg_Buff;
    DBADDR addr;
    char *pmsg;
    int tab_size = 20;
    const char* pname = name.c_str();

    if (name.size() == 0) {
        throw InvalidArgument("Record name cannot be empty.");
    }

    if (nameToAddr(pname, &addr)) {
        throw ObjectNotFound("Record name not found.");
    }

    pmsg = pMsgBuff->message;

    int status = dbpr_report(pname, &addr, level, pMsgBuff, tab_size);
    if (status) {
        throw InvalidRequest("dbpr_report returned status: " + StringUtility::toString<int>(status));
    }

    pmsg[0] = '\0';
    dbpr_msgOut(pMsgBuff, tab_size);
    return 0;
}
}

//
// Wrappers for calls available in iocsh
//
int CaIoc::dbLoadDatabase(const std::string& fileName, const std::string& path, const std::string& substitutions)
{
    return dbLoadDatabase(fileName.c_str(), path.c_str(), substitutions.c_str());
}

int CaIoc::dbLoadRecords(const std::string& fileName, const std::string& substitutions)
{
    return dbLoadRecords(fileName.c_str(), substitutions.c_str());
}

int CaIoc::iocInit() 
{
    return iocInit();
}

int CaIoc::dbl(const std::string& recordType, const std::string& fields)
{
    return dbl(recordType.c_str(), fields.c_str());
}

int CaIoc::dbpr(const std::string& name, int level)
{
   return dbpr(name.c_str(), level);
}

int CaIoc::dbgf(const std::string& name)
{
    return dbgf(name.c_str());
}

int CaIoc::dbpf(const std::string& name, const std::string& value)
{
    return dbpf(name.c_str(), value.c_str());
}
