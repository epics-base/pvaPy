// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution


#include <pv/clientFactory.h>
#include "StringUtility.h"
#include "PvaException.h"
#include "ObjectAlreadyExists.h"
#include "ObjectNotFound.h"
#include "InvalidRequest.h"
#include "PvaMirrorServer.h"
#include "PvaConstants.h"
#include "PvaPyConstants.h"

namespace epvd = epics::pvData;
namespace epvdb = epics::pvDatabase;
namespace epvc = epics::pvaClient;
namespace bp = boost::python;

// Mirror Channel Data Processor class

PvaPyLogger MirrorChannelDataProcessor::logger("MirrorChannelDataProcessor");

MirrorChannelDataProcessor::MirrorChannelDataProcessor(PvaMirrorServer* pvaMirrorServer_, const std::string& mirrorChannelName_)
    : pvaMirrorServer(pvaMirrorServer_)
    , mirrorChannelName(mirrorChannelName_)
    , mutex()
    , recordAdded(false)
{
}

MirrorChannelDataProcessor::~MirrorChannelDataProcessor() 
{
    onChannelDisconnect();
}

void MirrorChannelDataProcessor::processMonitorData(epvd::PVStructurePtr pvStructurePtr)
{
    epvd::Lock lock(mutex);
    if (!recordAdded) {
        epvd::PVStructurePtr pvStructurePtr2(epvd::getPVDataCreate()->createPVStructure(pvStructurePtr->getStructure()));
        pvStructurePtr2->copyUnchecked(*pvStructurePtr);
        pvaMirrorServer->addRecord(mirrorChannelName, pvStructurePtr2);
        pvaMirrorServer->disableRecordProcessing(mirrorChannelName);
        recordAdded = true;
    }
    else {
        pvaMirrorServer->updateUnchecked(mirrorChannelName, pvStructurePtr);
    }
}

void MirrorChannelDataProcessor::onChannelConnect()
{
}

void MirrorChannelDataProcessor::onChannelDisconnect()
{
    epvd::Lock lock(mutex);
    if (recordAdded) {
        if (pvaMirrorServer->hasRecord(mirrorChannelName)) {
            try {
                pvaMirrorServer->removeRecord(mirrorChannelName);
            }
            catch (ObjectNotFound& ex) {
                // channel is not there any more
            }

        }
        recordAdded = false;
    }
}

// Mirror Channel class

PvaPyLogger MirrorChannel::logger("MirrorChannel");
PvaClient MirrorChannel::pvaClient;
CaClient MirrorChannel::caClient;
epvc::PvaClientPtr MirrorChannel::pvaClientPtr(epvc::PvaClient::get("pva ca"));

MirrorChannel::MirrorChannel(const std::string& channelName_, PvProvider::ProviderType providerType_, unsigned int serverQueueSize_, MirrorChannelDataProcessorPtr dataProcessorPtr_)
    : pvaClientChannelPtr(pvaClientPtr->createChannel(channelName_,PvProvider::getProviderName(providerType_)))
    , channelName(channelName_)
    , providerType(providerType_)
    , serverQueueSize(serverQueueSize_)
    , dataProcessorPtr(dataProcessorPtr_)
    , isConnected(false)
    , hasIssuedConnect(false)
    , monitorActive(false)
{
    stateRequester = epvc::PvaClientChannelStateChangeRequesterPtr(new ChannelStateRequesterImpl(isConnected, this));
    pvaClientChannelPtr->setStateChangeRequester(stateRequester);
    issueConnect();
}

MirrorChannel::MirrorChannel(const MirrorChannel& c)
    : pvaClientChannelPtr(c.pvaClientChannelPtr)
    , channelName(c.channelName)
    , providerType(c.providerType)
    , serverQueueSize(c.serverQueueSize)
    , dataProcessorPtr(c.dataProcessorPtr)
    , isConnected(false)
    , hasIssuedConnect(false)
    , monitorActive(false)
    , stateRequester(c.stateRequester)
{
}

MirrorChannel::~MirrorChannel()
{
    stopMonitor();
    pvaClientChannelPtr.reset();
}

void MirrorChannel::issueConnect()
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

bool MirrorChannel::isChannelConnected() const
{
    return isConnected;
}

std::string MirrorChannel::getChannelName() const
{
    return channelName;
}

void MirrorChannel::processMonitorData(epvd::PVStructurePtr pvStructurePtr)
{
}

void MirrorChannel::onChannelConnect()
{
    logger.debug("Mirror channel %s connected", channelName.c_str());
    if (!monitorActive) {
        startMonitor();
    }
    dataProcessorPtr->onChannelConnect();
}

void MirrorChannel::onChannelDisconnect()
{
    logger.debug("Mirror channel %s disconnected", channelName.c_str());
    dataProcessorPtr->onChannelDisconnect();
}

void MirrorChannel::startMonitor()
{
    if (monitorActive) {
        return;
    }
    try {
        logger.debug("Starting monitor, channel connected: %d", isConnected);
        pvaClientMonitorRequesterPtr = epvc::PvaClientMonitorRequesterPtr(new ChannelMonitorRequesterImpl(channelName, dataProcessorPtr.get()));
        logger.debug("Monitor requester created");
        std::string request = PvaConstants::AllFieldsRequest;
        if (serverQueueSize > 0) {
            request = "record[queueSize=" + StringUtility::toString<unsigned int>(serverQueueSize) + "]" + PvaConstants::AllFieldsRequest;
        }
        pvaClientMonitorPtr = pvaClientChannelPtr->createMonitor(request);
        logger.debug("Monitor ptr created with request: " + request);
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

void MirrorChannel::stopMonitor()
{
    if (!monitorActive) {
        return;
    }
    logger.debug("Stopping monitor for mirror channel " + channelName);
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

void MirrorChannel::resetMonitorCounters()
{
    if (pvaClientMonitorRequesterPtr) {
        ChannelMonitorRequesterImpl* requesterImpl = static_cast<ChannelMonitorRequesterImpl*>(pvaClientMonitorRequesterPtr.get());
        requesterImpl->resetCounters();
    }
}

bp::dict MirrorChannel::getMonitorCounters()
{
    bp::dict pyDict;
    if (pvaClientMonitorRequesterPtr) {
        ChannelMonitorRequesterImpl* requesterImpl = static_cast<ChannelMonitorRequesterImpl*>(pvaClientMonitorRequesterPtr.get());
        pyDict[PvaPyConstants::NumReceivedCounterKey] = requesterImpl->getNumReceived();
        pyDict[PvaPyConstants::NumOverrunsCounterKey] = requesterImpl->getNumOverruns();
    }
    return pyDict;
}

// PVA Mirror Server class

PvaPyLogger PvaMirrorServer::logger("PvaMirrorServer");

PvaMirrorServer::PvaMirrorServer() 
    : PvaServer()
    , mirrorChannelMap()
{
}

PvaMirrorServer::PvaMirrorServer(const PvaMirrorServer& pvaMirrorServer)
    : PvaServer()
    , mirrorChannelMap()
{
}

PvaMirrorServer::~PvaMirrorServer() 
{
    removeAllMirrorRecords();
    removeAllRecords();
    stop();
}

void PvaMirrorServer::addMirrorRecord(const std::string& mirrorChannelName, const std::string& srcChannelName, PvProvider::ProviderType srcProviderType)
{
    unsigned int srcQueueSize = 0;
    addMirrorRecord(mirrorChannelName, srcChannelName, srcProviderType, srcQueueSize);
}

void PvaMirrorServer::addMirrorRecord(const std::string& mirrorChannelName, const std::string& srcChannelName, PvProvider::ProviderType srcProviderType, unsigned int srcQueueSize)
{
    if (hasRecord(mirrorChannelName)) {
        throw ObjectAlreadyExists("Master database already has record for channel: " + mirrorChannelName);
    }
    std::map<std::string, MirrorChannelPtr>::iterator it2 = mirrorChannelMap.find(mirrorChannelName);
    if (it2 != mirrorChannelMap.end()) {
        throw ObjectAlreadyExists("Master database already has mirror record for channel: " + mirrorChannelName);
    }
    MirrorChannelDataProcessorPtr dataProcessorPtr = MirrorChannelDataProcessorPtr(new MirrorChannelDataProcessor(this, mirrorChannelName));
    MirrorChannelPtr mirrorChannelPtr = MirrorChannelPtr(new MirrorChannel(srcChannelName, srcProviderType, srcQueueSize, dataProcessorPtr));
    mirrorChannelMap[mirrorChannelName] = mirrorChannelPtr;
    logger.debug("Added mirror record: " + mirrorChannelName + " (source channel: " + srcChannelName + "; source queue size: " + StringUtility::toString<unsigned int>(srcQueueSize) + ")");
}

void PvaMirrorServer::removeMirrorRecord(const std::string& mirrorChannelName)
{
    std::map<std::string, MirrorChannelPtr>::iterator it = mirrorChannelMap.find(mirrorChannelName);
    if (it == mirrorChannelMap.end()) {
        throw ObjectNotFound("Master database does not have mirror record for channel: " + mirrorChannelName);
    }
    MirrorChannelPtr mirrorChannel = it->second;
    std::string srcChannelName = mirrorChannel->getChannelName();
    logger.debug("Removing mirror channel for " + srcChannelName);
    if (hasRecord(srcChannelName)) {
        removeRecord(srcChannelName);
    }
    mirrorChannelMap.erase(it);
    logger.debug("Removed mirror record: " + mirrorChannelName);
}

void PvaMirrorServer::removeAllMirrorRecords()
{
    std::list<std::string> mirrorRecordNames;
    typedef std::map<std::string, MirrorChannelPtr>::iterator MI;
    for (MI it = mirrorChannelMap.begin(); it != mirrorChannelMap.end(); it++) {
        mirrorRecordNames.push_back(it->first);
    }

    typedef std::list<std::string>::iterator LI;
    for (LI it = mirrorRecordNames.begin(); it != mirrorRecordNames.end(); ++it) {
        removeMirrorRecord(*it);
    }
}

bool PvaMirrorServer::hasMirrorRecord(const std::string& mirrorChannelName)
{
    if (mirrorChannelMap.find(mirrorChannelName) != mirrorChannelMap.end()) {
        return true;
    }
    return false;
}

void PvaMirrorServer::resetMirrorRecordCounters(const std::string& mirrorChannelName)
{
    std::map<std::string, MirrorChannelPtr>::iterator it = mirrorChannelMap.find(mirrorChannelName);
    if (it == mirrorChannelMap.end()) {
        throw ObjectNotFound("Master database does not have mirror record for channel: " + mirrorChannelName);
    }
    MirrorChannelPtr mirrorChannel = it->second;
    mirrorChannel->resetMonitorCounters();
}

bp::dict PvaMirrorServer::getMirrorRecordCounters(const std::string& mirrorChannelName)
{
    std::map<std::string, MirrorChannelPtr>::iterator it = mirrorChannelMap.find(mirrorChannelName);
    if (it == mirrorChannelMap.end()) {
        throw ObjectNotFound("Master database does not have mirror record for channel: " + mirrorChannelName);
    }
    MirrorChannelPtr mirrorChannel = it->second;
    return mirrorChannel->getMonitorCounters();
}

bp::list PvaMirrorServer::getMirrorRecordNames()
{
    bp::list mirrorRecordNames;
    typedef std::map<std::string, MirrorChannelPtr>::iterator MI;
    for (MI it = mirrorChannelMap.begin(); it != mirrorChannelMap.end(); it++) {
        mirrorRecordNames.append(it->first);
    }
    return mirrorRecordNames;
}

