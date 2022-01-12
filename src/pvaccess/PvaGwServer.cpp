// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution


#include <pv/clientFactory.h>
#include "PvaException.h"
#include "ObjectAlreadyExists.h"
#include "ObjectNotFound.h"
#include "InvalidRequest.h"
#include "PvaGwServer.h"
#include "PvaConstants.h"

namespace epvd = epics::pvData;
namespace epvdb = epics::pvDatabase;
namespace epvc = epics::pvaClient;

// GW Channel Data Processor class

PvaPyLogger GwChannelDataProcessor::logger("GwChannelDataProcessor");

GwChannelDataProcessor::GwChannelDataProcessor(PvaGwServer* pvaGwServer_, const std::string& gwChannelName_)
    : pvaGwServer(pvaGwServer_)
    , gwChannelName(gwChannelName_)
    , mutex()
    , recordAdded(false)
{
}

GwChannelDataProcessor::~GwChannelDataProcessor() 
{
    onChannelDisconnect();
}

void GwChannelDataProcessor::processMonitorData(epvd::PVStructurePtr pvStructurePtr)
{
    epvd::Lock lock(mutex);
    if (!recordAdded) {
        epvd::PVStructurePtr pvStructurePtr2(epvd::getPVDataCreate()->createPVStructure(pvStructurePtr->getStructure()));
        pvStructurePtr2->copyUnchecked(*pvStructurePtr);
        pvaGwServer->addRecord(gwChannelName, pvStructurePtr2);
        recordAdded = true;
    }
    else {
        pvaGwServer->update(gwChannelName, pvStructurePtr);
    }
}

void GwChannelDataProcessor::onChannelConnect()
{
}

void GwChannelDataProcessor::onChannelDisconnect()
{
    epvd::Lock lock(mutex);
    if (recordAdded) {
        if (pvaGwServer->hasRecord(gwChannelName)) {
            try {
                pvaGwServer->removeRecord(gwChannelName);
            }
            catch (ObjectNotFound& ex) {
                // channel is not there any more
            }

        }
        recordAdded = false;
    }
}

// GW Channel class

PvaPyLogger GwChannel::logger("GwChannel");
PvaClient GwChannel::pvaClient;
CaClient GwChannel::caClient;
epvc::PvaClientPtr GwChannel::pvaClientPtr(epvc::PvaClient::get("pva ca"));

GwChannel::GwChannel(const std::string& channelName_, PvProvider::ProviderType providerType_, GwChannelDataProcessorPtr dataProcessorPtr_)
    : pvaClientChannelPtr(pvaClientPtr->createChannel(channelName_,PvProvider::getProviderName(providerType_)))
    , channelName(channelName_)
    , providerType(providerType_)
    , dataProcessorPtr(dataProcessorPtr_)
    , isConnected(false)
    , hasIssuedConnect(false)
    , monitorActive(false)
{
    stateRequester = epvc::PvaClientChannelStateChangeRequesterPtr(new ChannelStateRequesterImpl(isConnected, this));
    pvaClientChannelPtr->setStateChangeRequester(stateRequester);
    issueConnect();
}

GwChannel::GwChannel(const GwChannel& c)
    : pvaClientChannelPtr(c.pvaClientChannelPtr)
    , channelName(c.channelName)
    , providerType(c.providerType)
    , dataProcessorPtr(c.dataProcessorPtr)
    , isConnected(false)
    , hasIssuedConnect(false)
    , monitorActive(false)
    , stateRequester(c.stateRequester)
{
}

GwChannel::~GwChannel()
{
    stopMonitor();
    pvaClientChannelPtr.reset();
}

void GwChannel::issueConnect()
{
    if (hasIssuedConnect) {
        return;
    }
    try {
        pvaClientChannelPtr->issueConnect();
        hasIssuedConnect = true;
        logger.debug("Issued connect for channel " + channelName);
    }
    catch (std::runtime_error& ex) {
        logger.warn("Could not issue connect for channel %s: %s.", pvaClientChannelPtr->getChannelName().c_str(), ex.what());
    }
}

bool GwChannel::isChannelConnected() const
{
    return isConnected;
}

std::string GwChannel::getChannelName() const
{
    return channelName;
}

void GwChannel::processMonitorData(epvd::PVStructurePtr pvStructurePtr)
{
}

void GwChannel::onChannelConnect()
{
    logger.debug("GW channel %s connected", channelName.c_str());
    if (!monitorActive) {
        startMonitor();
    }
    dataProcessorPtr->onChannelConnect();
}

void GwChannel::onChannelDisconnect()
{
    logger.debug("GW channel %s disconnected", channelName.c_str());
    dataProcessorPtr->onChannelDisconnect();
}

void GwChannel::startMonitor()
{
    if (monitorActive) {
        return;
    }
    try {
        logger.debug("Starting monitor, channel connected: %d", isConnected);
        pvaClientMonitorRequesterPtr = epvc::PvaClientMonitorRequesterPtr(new ChannelMonitorRequesterImpl(channelName, dataProcessorPtr.get()));
        logger.debug("Monitor requester created");
        pvaClientMonitorPtr = pvaClientChannelPtr->createMonitor(PvaConstants::AllFieldsRequest);
        logger.debug("Monitor ptr created");
        pvaClientMonitorPtr->setRequester(pvaClientMonitorRequesterPtr);
        logger.debug("Monitor requester set");
        pvaClientMonitorPtr->issueConnect();
        logger.debug("Monitor issued connect");
        monitorActive = true;
    }
    catch (std::runtime_error& ex) {
        logger.error(ex.what());
    }
}

void GwChannel::stopMonitor()
{
    if (!monitorActive) {
        return;
    }
    logger.debug("Stopping monitor for GW channel " + channelName);
    if (pvaClientMonitorRequesterPtr) {
        pvaClientMonitorRequesterPtr->unlisten();
    }
    if (pvaClientMonitorPtr) {
        try {
            pvaClientMonitorPtr->stop();
        }
        catch (std::runtime_error& ex) {
            logger.error("Caught exception while trying to stop monitor: %s", ex.what());
        }
    }
    monitorActive = false;
}


// PVA GW Server class

PvaPyLogger PvaGwServer::logger("PvaGwServer");

PvaGwServer::PvaGwServer() 
    : PvaServer()
    , gwChannelMap()
{
}

PvaGwServer::PvaGwServer(const PvaGwServer& pvaGwServer)
    : PvaServer()
    , gwChannelMap()
{
}

PvaGwServer::~PvaGwServer() 
{
    removeAllRecords();
    stop();
}

void PvaGwServer::addGwRecord(const std::string& gwChannelName, const std::string& srcChannelName, PvProvider::ProviderType srcProviderType)
{
    if (hasRecord(gwChannelName)) {
        throw ObjectAlreadyExists("Master database already has record for channel: " + gwChannelName);
    }
    std::map<std::string, GwChannelPtr>::iterator it2 = gwChannelMap.find(gwChannelName);
    if (it2 != gwChannelMap.end()) {
        throw ObjectAlreadyExists("Master database already has GW record for channel: " + gwChannelName);
    }
    GwChannelDataProcessorPtr dataProcessorPtr = GwChannelDataProcessorPtr(new GwChannelDataProcessor(this, gwChannelName));
    GwChannelPtr gwChannelPtr = GwChannelPtr(new GwChannel(srcChannelName, srcProviderType, dataProcessorPtr));
    gwChannelMap[gwChannelName] = gwChannelPtr;
    logger.debug("Added GW record: " + gwChannelName);
}

void PvaGwServer::removeGwRecord(const std::string& gwChannelName)
{
    std::map<std::string, GwChannelPtr>::iterator it = gwChannelMap.find(gwChannelName);
    if (it == gwChannelMap.end()) {
        throw ObjectNotFound("Master database does not have GW record for channel: " + gwChannelName);
    }
    GwChannelPtr gwChannel = it->second;
    std::string srcChannelName = gwChannel->getChannelName();
    logger.debug("Removing GW channel for " + srcChannelName);
    if (hasRecord(srcChannelName)) {
        removeRecord(srcChannelName);
    }
    gwChannelMap.erase(it);
    logger.debug("Removed GW record: " + gwChannelName);
}

