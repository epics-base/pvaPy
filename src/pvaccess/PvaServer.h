// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PVA_SERVER_H
#define PVA_SERVER_H

#include <string>
#include <map>
#include "boost/python/list.hpp"
#include "pv/pvData.h"
#include "pv/pvAccess.h"
#include "pv/serverContext.h"
#include "PvObject.h"
#include "PyPvRecord.h"
#include "PvaPyLogger.h"

class PvaServer 
{
public:
    PvaServer();
    PvaServer(const std::string& channelName, const PvObject& pvObject);
    virtual ~PvaServer();
    virtual void update(const PvObject& pvObject);
    virtual void update(const std::string& channelName, const PvObject& pvObject);
    virtual void addRecord(const std::string& channelName, const PvObject& pvObject);
    virtual void removeRecord(const std::string& channelName);
    virtual bool hasRecord(const std::string& channelName);
    virtual boost::python::list getRecordNames();

private:
    static PvaPyLogger logger;
    epics::pvAccess::ServerContext::shared_pointer server;
    std::map<std::string, PyPvRecordPtr> recordMap;
};

#endif
