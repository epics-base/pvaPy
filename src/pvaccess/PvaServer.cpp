// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python.hpp"
#include "pv/channelProviderLocal.h"
#include "PvaException.h"
#include "PvaServer.h"

PvaPyLogger PvaServer::logger("PvaServer");

PvaServer::PvaServer(const std::string& channelName, const PvObject& pvObject) :
    record(PyPvRecord::create(channelName, pvObject))
{
    if(!record.get()) {
        throw PvaException("Failed to create PyPvRecord: " + channelName);
    }

    epics::pvDatabase::PVDatabasePtr master = epics::pvDatabase::PVDatabase::getMaster();
    epics::pvDatabase::ChannelProviderLocalPtr channelProvider = epics::pvDatabase::getChannelProviderLocal();

    if(!master->addRecord(record)) {
        throw PvaException("Cannot add record to master database for channel: " + channelName);
    }
    server = epics::pvAccess::startPVAServer(epics::pvAccess::PVACCESS_ALL_PROVIDERS, 0, true, true);
}

PvaServer::~PvaServer() 
{
}

void PvaServer::update(const PvObject& pvObject) 
{
    record->update(pvObject);
}
