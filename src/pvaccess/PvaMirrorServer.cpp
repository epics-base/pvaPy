// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution


#include <pv/clientFactory.h>
#include "StringUtility.h"
#include "PyUtility.h"
#include "PvaException.h"
#include "InvalidRequest.h"
#include "ObjectAlreadyExists.h"
#include "ObjectNotFound.h"
#include "PvaMirrorServer.h"
#include "PvaConstants.h"
#include "PvaPyConstants.h"

namespace epvd = epics::pvData;
namespace epvdb = epics::pvDatabase;
namespace epvc = epics::pvaClient;
namespace bp = boost::python;

// Mirror Channel Data Processor class

PvaPyLogger MirrorChannelDataProcessor::logger("MirrorChannelDataProcessor");

MirrorChannelDataProcessor::MirrorChannelDataProcessor(PvaMirrorServer* pvaMirrorServer_, const std::string& mirrorChannelName_, unsigned int nSrcMonitors_)
    : pvaMirrorServer(pvaMirrorServer_)
    , mirrorChannelName(mirrorChannelName_)
    , mutex()
    , recordAdded(false)
    , nSrcMonitors(nSrcMonitors_)
    , nUpdatesToSkip(nSrcMonitors)
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
        nUpdatesToSkip--;
    }
    else if (nUpdatesToSkip > 0) {
        // This makes sure we do not generate first update
        // multiple times in case we have multiple monitors
        nUpdatesToSkip--;
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
        nUpdatesToSkip = nSrcMonitors;
    }
}

// Mirror Channel Monitor class

PvaPyLogger MirrorChannelMonitor::logger("MirrorChannelMonitor");
PvaClient MirrorChannelMonitor::pvaClient;
CaClient MirrorChannelMonitor::caClient;
epvc::PvaClientPtr MirrorChannelMonitor::pvaClientPtr(epvc::PvaClient::get("pva ca"));

MirrorChannelMonitor::MirrorChannelMonitor(const std::string& channelName_, PvProvider::ProviderType providerType_, unsigned int serverQueueSize_, const std::string& fieldRequestDescriptor_, MirrorChannelDataProcessorPtr dataProcessorPtr_)
    : pvaClientChannelPtr(pvaClientPtr->createChannel(channelName_,PvProvider::getProviderName(providerType_)))
    , channelName(channelName_)
    , providerType(providerType_)
    , serverQueueSize(serverQueueSize_)
    , fieldRequestDescriptor(fieldRequestDescriptor_)
    , dataProcessorPtr(dataProcessorPtr_)
    , isConnected(false)
    , hasIssuedConnect(false)
    , monitorActive(false)
{
    stateRequester = epvc::PvaClientChannelStateChangeRequesterPtr(new ChannelStateRequesterImpl(isConnected, this));
    pvaClientChannelPtr->setStateChangeRequester(stateRequester);
    issueConnect();
}

MirrorChannelMonitor::MirrorChannelMonitor(const MirrorChannelMonitor& c)
    : pvaClientChannelPtr(c.pvaClientChannelPtr)
    , channelName(c.channelName)
    , providerType(c.providerType)
    , serverQueueSize(c.serverQueueSize)
    , fieldRequestDescriptor(c.fieldRequestDescriptor)
    , dataProcessorPtr(c.dataProcessorPtr)
    , isConnected(false)
    , hasIssuedConnect(false)
    , monitorActive(false)
    , stateRequester(c.stateRequester)
{
}

MirrorChannelMonitor::~MirrorChannelMonitor()
{
    stopMonitor();
    pvaClientChannelPtr.reset();
}

void MirrorChannelMonitor::issueConnect()
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

bool MirrorChannelMonitor::isChannelConnected() const
{
    return isConnected;
}

std::string MirrorChannelMonitor::getChannelName() const
{
    return channelName;
}

void MirrorChannelMonitor::processMonitorData(epvd::PVStructurePtr pvStructurePtr)
{
}

void MirrorChannelMonitor::onChannelConnect()
{
    logger.debug("Mirror channel %s connected", channelName.c_str());
    if (!monitorActive) {
        startMonitor();
    }
    dataProcessorPtr->onChannelConnect();
}

void MirrorChannelMonitor::onChannelDisconnect()
{
    logger.debug("Mirror channel %s disconnected", channelName.c_str());
    dataProcessorPtr->onChannelDisconnect();
}

void MirrorChannelMonitor::startMonitor()
{
    if (monitorActive) {
        return;
    }
    try {
        logger.debug("Starting monitor, channel connected: %d", isConnected);
        pvaClientMonitorRequesterPtr = epvc::PvaClientMonitorRequesterPtr(new ChannelMonitorRequesterImpl(channelName, dataProcessorPtr.get()));
        logger.debug("Monitor requester created");
        std::string request = "field(" + fieldRequestDescriptor + ")";
        if (fieldRequestDescriptor.empty()) {
            request = PvaConstants::AllFieldsRequest;
        }
        if (serverQueueSize > 0) {
            request = "record[queueSize=" + StringUtility::toString<unsigned int>(serverQueueSize) + "]" + request;
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

void MirrorChannelMonitor::stopMonitor()
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

void MirrorChannelMonitor::resetMonitorCounters()
{
    if (pvaClientMonitorRequesterPtr) {
        ChannelMonitorRequesterImpl* requesterImpl = static_cast<ChannelMonitorRequesterImpl*>(pvaClientMonitorRequesterPtr.get());
        requesterImpl->resetCounters();
    }
}

bp::dict MirrorChannelMonitor::getMonitorCounters()
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
    , mirrorChannelMonitorMap()
{
}

PvaMirrorServer::PvaMirrorServer(const PvaMirrorServer& pvaMirrorServer)
    : PvaServer()
    , mirrorChannelMonitorMap()
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
    unsigned int nSrcMonitors = 1;
    std::string srcFieldRequestDescriptor = "";
    addMirrorRecord(mirrorChannelName, srcChannelName, srcProviderType, srcQueueSize, nSrcMonitors, srcFieldRequestDescriptor);
}

void PvaMirrorServer::addMirrorRecord(const std::string& mirrorChannelName, const std::string& srcChannelName, PvProvider::ProviderType srcProviderType, unsigned int srcQueueSize)
{
    unsigned int nSrcMonitors = 1;
    std::string srcFieldRequestDescriptor = "";
    addMirrorRecord(mirrorChannelName, srcChannelName, srcProviderType, srcQueueSize, nSrcMonitors, srcFieldRequestDescriptor);
}

void PvaMirrorServer::addMirrorRecord(const std::string& mirrorChannelName, const std::string& srcChannelName, PvProvider::ProviderType srcProviderType, unsigned int srcQueueSize, unsigned int nSrcMonitors, const std::string& srcFieldRequestDescriptor)
{
    if (hasRecord(mirrorChannelName)) {
        throw ObjectAlreadyExists("Master database already has record for channel: " + mirrorChannelName);
    }
    typedef std::multimap<std::string, MirrorChannelMonitorPtr>::iterator MI;
    MI it = mirrorChannelMonitorMap.find(mirrorChannelName);
    if (it != mirrorChannelMonitorMap.end()) {
        throw ObjectAlreadyExists("Master database already has mirror record for channel: " + mirrorChannelName);
    }
    if (nSrcMonitors < 1) {
        throw InvalidRequest("Number of source listeners for channel " + mirrorChannelName + " cannot be less than 1");
    }
    MirrorChannelDataProcessorPtr dataProcessorPtr = MirrorChannelDataProcessorPtr(new MirrorChannelDataProcessor(this, mirrorChannelName, nSrcMonitors));
    for (unsigned int i = 0; i < nSrcMonitors; i++) {
        MirrorChannelMonitorPtr mirrorChannelMonitorPtr = MirrorChannelMonitorPtr(new MirrorChannelMonitor(srcChannelName, srcProviderType, srcQueueSize, srcFieldRequestDescriptor, dataProcessorPtr));
        mirrorChannelMonitorMap.insert(std::make_pair(mirrorChannelName, mirrorChannelMonitorPtr));
    }
    logger.debug("Added mirror record: " + mirrorChannelName + " (source channel: " + srcChannelName + "; source queue size: " + StringUtility::toString<unsigned int>(srcQueueSize) + "; number of source listeners: " + StringUtility::toString<unsigned int>(nSrcMonitors) + "; source field request descriptor: " +  srcFieldRequestDescriptor + ")");
}

void PvaMirrorServer::removeMirrorRecord(const std::string& mirrorChannelName)
{
    typedef std::multimap<std::string, MirrorChannelMonitorPtr>::iterator MI;
    MI it = mirrorChannelMonitorMap.find(mirrorChannelName);
    if (it == mirrorChannelMonitorMap.end()) {
        throw ObjectNotFound("Master database does not have mirror record for channel: " + mirrorChannelName);
    }
    std::string srcChannelName;
    for (it = mirrorChannelMonitorMap.begin(); it != mirrorChannelMonitorMap.end(); ) {
        MirrorChannelMonitorPtr mirrorChannelMonitor = it->second;
        if (it->first == mirrorChannelName) {
            logger.debug("Removing mirror channel listener for " + srcChannelName);
            srcChannelName = mirrorChannelMonitor->getChannelName();
            mirrorChannelMonitorMap.erase(it++);
        }
        else {
            it++;
        }
    }
    logger.debug("Removing mirror channel for " + srcChannelName);
    if (hasRecord(srcChannelName)) {
        removeRecord(srcChannelName);
    }
    logger.debug("Removed mirror record: " + mirrorChannelName);
}

void PvaMirrorServer::removeAllMirrorRecords()
{
    std::list<std::string> mirrorRecordNames;
    typedef std::multimap<std::string, MirrorChannelMonitorPtr>::iterator MI;
    for (MI it = mirrorChannelMonitorMap.begin(); it != mirrorChannelMonitorMap.end(); it = mirrorChannelMonitorMap.upper_bound(it->first)) {
        mirrorRecordNames.push_back(it->first);
    }

    typedef std::list<std::string>::iterator LI;
    for (LI it = mirrorRecordNames.begin(); it != mirrorRecordNames.end(); ++it) {
        removeMirrorRecord(*it);
    }
}

bool PvaMirrorServer::hasMirrorRecord(const std::string& mirrorChannelName)
{
    if (mirrorChannelMonitorMap.find(mirrorChannelName) != mirrorChannelMonitorMap.end()) {
        return true;
    }
    return false;
}

void PvaMirrorServer::resetMirrorRecordCounters(const std::string& mirrorChannelName)
{
    typedef std::multimap<std::string, MirrorChannelMonitorPtr>::iterator MI;
    MI it = mirrorChannelMonitorMap.find(mirrorChannelName);
    if (it == mirrorChannelMonitorMap.end()) {
        throw ObjectNotFound("Master database does not have mirror record for channel: " + mirrorChannelName);
    }
    for (it = mirrorChannelMonitorMap.begin(); it != mirrorChannelMonitorMap.end(); it++) {
        MirrorChannelMonitorPtr mirrorChannelMonitor = it->second;
        mirrorChannelMonitor->resetMonitorCounters();
    }
}

bp::dict PvaMirrorServer::getMirrorRecordCounters(const std::string& mirrorChannelName)
{
    int nReceived = 0;
    int nOverruns = 0;
    int nSrcMonitors = 0;
    typedef std::multimap<std::string, MirrorChannelMonitorPtr>::iterator MI;
    MI it = mirrorChannelMonitorMap.find(mirrorChannelName);
    if (it == mirrorChannelMonitorMap.end()) {
        throw ObjectNotFound("Master database does not have mirror record for channel: " + mirrorChannelName);
    }
    for (it = mirrorChannelMonitorMap.begin(); it != mirrorChannelMonitorMap.end(); it++) {
        MirrorChannelMonitorPtr mirrorChannelMonitor = it->second;
        bp::dict listenerDict = mirrorChannelMonitor->getMonitorCounters();
        nReceived += PyUtility::extractKeyValueFromPyDict<int>(PvaPyConstants::NumReceivedCounterKey, listenerDict, 0);
        nOverruns += PyUtility::extractKeyValueFromPyDict<int>(PvaPyConstants::NumOverrunsCounterKey, listenerDict, 0);
        nSrcMonitors++;
    }
    // Correct for initial connections with multiple clients,
    if (nReceived > 0) {
        nReceived -= (nSrcMonitors-1);
    }
    bp::dict recordDict;
    recordDict[PvaPyConstants::NumReceivedCounterKey] = nReceived;
    recordDict[PvaPyConstants::NumOverrunsCounterKey] = nOverruns;
    return recordDict;
}

bp::list PvaMirrorServer::getMirrorRecordNames()
{
    bp::list mirrorRecordNames;
    typedef std::multimap<std::string, MirrorChannelMonitorPtr>::iterator MI;
    for (MI it = mirrorChannelMonitorMap.begin(); it != mirrorChannelMonitorMap.end(); it = mirrorChannelMonitorMap.upper_bound(it->first)) {
        mirrorRecordNames.append(it->first);
    }
    return mirrorRecordNames;
}

