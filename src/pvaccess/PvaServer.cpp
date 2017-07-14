// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python.hpp"
#include "pv/channelProviderLocal.h"
#include "PvaException.h"
#include "ObjectAlreadyExists.h"
#include "ObjectNotFound.h"
#include "InvalidRequest.h"
#include "PvaServer.h"

PvaPyLogger PvaServer::logger("PvaServer");

PvaServer::PvaServer() :
    recordMap()
{
    epics::pvDatabase::ChannelProviderLocalPtr channelProvider = epics::pvDatabase::getChannelProviderLocal();
    server = epics::pvAccess::startPVAServer(epics::pvAccess::PVACCESS_ALL_PROVIDERS, 0, true, true);
}

PvaServer::PvaServer(const std::string& channelName, const PvObject& pvObject) :
    recordMap()
{
    initRecord(channelName, pvObject);
    epics::pvDatabase::ChannelProviderLocalPtr channelProvider = epics::pvDatabase::getChannelProviderLocal();
    server = epics::pvAccess::startPVAServer(epics::pvAccess::PVACCESS_ALL_PROVIDERS, 0, true, true);
}

PvaServer::PvaServer(const std::string& channelName, const PvObject& pvObject, const boost::python::object& onWriteCallback) :
    recordMap()
{
    initRecord(channelName, pvObject, onWriteCallback);
    epics::pvDatabase::ChannelProviderLocalPtr channelProvider = epics::pvDatabase::getChannelProviderLocal();
    server = epics::pvAccess::startPVAServer(epics::pvAccess::PVACCESS_ALL_PROVIDERS, 0, true, true);
}

PvaServer::~PvaServer() 
{
}

void PvaServer::initRecord(const std::string& channelName, const PvObject& pvObject, const boost::python::object& onWriteCallback) 
{
    PyPvRecordPtr record(PyPvRecord::create(channelName, pvObject, onWriteCallback));
    if(!record.get()) {
        throw PvaException("Failed to create PyPvRecord: " + channelName);
    }
    
    epics::pvDatabase::PVDatabasePtr master = epics::pvDatabase::PVDatabase::getMaster();
    if(!master->addRecord(record)) {
        throw PvaException("Cannot add record to master database for channel: " + channelName);
    }
    recordMap[channelName] = record;
}

void PvaServer::update(const PvObject& pvObject) 
{
    if (recordMap.size() == 0) {
        throw InvalidRequest("Master database does not have any records.");
    }
    if (recordMap.size() != 1) {
        throw InvalidRequest("Master database has multiple records.");
    }

    std::map<std::string, PyPvRecordPtr>::iterator it = recordMap.begin();
    it->second->update(pvObject);
}

void PvaServer::update(const std::string& channelName, const PvObject& pvObject) 
{
    std::map<std::string, PyPvRecordPtr>::iterator it = recordMap.find(channelName);
    if (it == recordMap.end()) {
        throw ObjectNotFound("Master database does not have record for channel: " + channelName);
    }
    it->second->update(pvObject);
}

void PvaServer::addRecord(const std::string& channelName, const PvObject& pvObject, const boost::python::object& onWriteCallback)
{
    std::map<std::string, PyPvRecordPtr>::iterator it = recordMap.find(channelName);
    if (it != recordMap.end()) {
        throw ObjectAlreadyExists("Master database already has record for channel: " + channelName);
    }

    initRecord(channelName, pvObject, onWriteCallback);
}

void PvaServer::removeRecord(const std::string& channelName)
{
    std::map<std::string, PyPvRecordPtr>::iterator it = recordMap.find(channelName);
    if (it == recordMap.end()) {
        throw ObjectNotFound("Master database does not have record for channel: " + channelName);
    }

    epics::pvDatabase::PVDatabasePtr master = epics::pvDatabase::PVDatabase::getMaster();
    if(!master->removeRecord(it->second)) {
        throw PvaException("Cannot remove record to master database for channel: " + channelName);
    }
    recordMap.erase(it);
}

bool PvaServer::hasRecord(const std::string& channelName)
{
    if (recordMap.find(channelName) != recordMap.end()) {
        return true;
    }
    return false;
}

boost::python::list PvaServer::getRecordNames() 
{
    boost::python::list recordNames;
    typedef std::map<std::string, PyPvRecordPtr>::iterator MI;
    for (MI it = recordMap.begin(); it != recordMap.end(); it++) {
        recordNames.append(it->first);
    }
    return recordNames;
}

