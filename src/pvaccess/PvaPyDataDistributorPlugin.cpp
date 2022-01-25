// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include <stdlib.h>

#include <string>
#include <algorithm>
#include <pv/lock.h>
#include <pv/pvData.h>
#include <pv/bitSet.h>

#define epicsExportSharedSymbols
#include "PvaPyDataDistributorPlugin.h"
#include "StringUtility.h"

using std::string;
using std::size_t;
using std::cout;
using std::endl;
using std::tr1::static_pointer_cast;
using std::vector;
using namespace epics::pvData;
namespace epvd = epics::pvData;

namespace epics { namespace pvCopy {

PvaPyLogger PvaPyDataDistributorPlugin::logger("PvaPyDataDistributorPlugin");
PvaPyLogger PvaPyDataDistributorFilter::logger("PvaPyDataDistributorFilter");

static std::string name("pydistributor");
bool PvaPyDataDistributorPlugin::initialized(PvaPyDataDistributorPlugin::initialize());

PvaPyLogger PvaPyDataDistributor::logger("PvaPyDataDistributor");
std::map<std::string, PvaPyDataDistributorPtr> PvaPyDataDistributor::dataDistributorMap;
epics::pvData::Mutex PvaPyDataDistributor::dataDistributorMapMutex;

PvaPyDataDistributorPtr PvaPyDataDistributor::getInstance(const std::string& groupId)
{
    epvd::Lock lock(dataDistributorMapMutex);
    std::map<std::string,PvaPyDataDistributorPtr>::iterator ddit = dataDistributorMap.find(groupId);
    if (ddit != dataDistributorMap.end()) {
        PvaPyDataDistributorPtr ddPtr = ddit->second;
        return ddPtr;
    }
    else {
        PvaPyDataDistributorPtr ddPtr(new PvaPyDataDistributor(groupId));
        dataDistributorMap[groupId] = ddPtr;
        logger.debug("Created new data distributor with group id: %s", groupId.c_str());
        return ddPtr;
    }
}

void PvaPyDataDistributor::removeUnusedInstance(PvaPyDataDistributorPtr dataDistributorPtr)
{
    epvd::Lock lock(dataDistributorMapMutex);
    std::string groupId = dataDistributorPtr->getGroupId();
    std::map<std::string,PvaPyDataDistributorPtr>::iterator ddit = dataDistributorMap.find(groupId);
    if (ddit != dataDistributorMap.end()) {
        PvaPyDataDistributorPtr ddPtr = ddit->second;
        int nSets = ddPtr->clientSetMap.size();
        logger.debug("Number of active client sets: %d", nSets);
        if (nSets == 0) {
            dataDistributorMap.erase(ddit);
            logger.debug("Removed data distributor for group: %s", groupId.c_str());
        }
    }
}

PvaPyDataDistributor::PvaPyDataDistributor(const std::string& groupId_)
    : groupId(groupId_)
    , mutex()
    , clientSetMap()
    , clientSetIdList()
    , currentSetIdIter(clientSetIdList.end())
    , lastUpdateValue()
{
}

PvaPyDataDistributor::~PvaPyDataDistributor()
{
    epvd::Lock lock(mutex);
    clientSetMap.clear();
    clientSetIdList.clear();
}

std::string PvaPyDataDistributor::addClient(int clientId, const std::string& setId, const std::string& triggerField, int nUpdatesPerClient, int updateMode)
{
    epvd::Lock lock(mutex);
    std::map<std::string,ClientSetPtr>::iterator git = clientSetMap.find(setId);
    if (git != clientSetMap.end()) {
        ClientSetPtr setPtr = git->second;
        setPtr->clientIdList.push_back(clientId);
        logger.debug("Added client %d to existing set %s", clientId, setId.c_str());
        return setPtr->triggerField;
    }
    else {
        ClientSetPtr setPtr(new ClientSet(setId, triggerField, nUpdatesPerClient, updateMode));
        setPtr->clientIdList.push_back(clientId);
        clientSetMap[setId] = setPtr;
        clientSetIdList.push_back(setId);
        logger.debug("Added client %d to new set %s (triggerField: %s, nUpdatesPerClient: %d)", clientId, setId.c_str(), triggerField.c_str(), nUpdatesPerClient);
        return triggerField;
    }
}

void PvaPyDataDistributor::removeClient(int clientId, const std::string& setId)
{
    epvd::Lock lock(mutex);
    logger.debug("Removing client %d from set %s", clientId, setId.c_str());
    std::map<std::string,ClientSetPtr>::iterator git = clientSetMap.find(setId);
    if (git != clientSetMap.end()) {
        ClientSetPtr setPtr = git->second;
        std::list<int>::iterator cit = std::find(setPtr->clientIdList.begin(), setPtr->clientIdList.end(), clientId);
        if (cit != setPtr->clientIdList.end()) {
            // If we are removing current client id, advance iterator
            if (cit == setPtr->currentClientIdIter) {
                logger.debug("Advancing current client id iterator for set %s", setId.c_str());
                setPtr->currentClientIdIter++;
            }

            // Find current client id 
            int currentClientId = -1;
            if (setPtr->currentClientIdIter != setPtr->clientIdList.end()) {
                currentClientId = *(setPtr->currentClientIdIter);
            }

            // Remove client id from the list
            setPtr->clientIdList.erase(cit);
            logger.debug("Removed client %d from set %s", clientId, setId.c_str());

             // Reset current client id iterator
            setPtr->currentClientIdIter = setPtr->clientIdList.end();
            if (currentClientId < 0) {
                logger.debug("Current client id is not set");
            }
            else {
                std::list<int>::iterator cit2 = std::find(setPtr->clientIdList.begin(), setPtr->clientIdList.end(), currentClientId);
                if (cit2 != setPtr->clientIdList.end()) {
                    logger.debug("Current client id is set to %d", currentClientId);
                    setPtr->currentClientIdIter = cit2;
                }
                else {
                    logger.warn("Could not find current client %d in set %s", currentClientId, setId.c_str());
                }
            }
        }
        else {
            logger.warn("Could not find client %d in set %s", clientId, setId.c_str());
        }
       
        logger.debug("Number of clients in set %s: %d", setId.c_str(), setPtr->clientIdList.size());
        if (setPtr->clientIdList.size() == 0) {
            clientSetMap.erase(git);
            std::list<std::string>::iterator git2 = std::find(clientSetIdList.begin(), clientSetIdList.end(), setId);
            if (git2 == currentSetIdIter) {
                logger.debug("Set %s will be removed, advancing current set iterator", setId.c_str());
                currentSetIdIter++;
            }
            if (git2 != clientSetIdList.end()) {
                clientSetIdList.erase(git2);
            }
            logger.debug("Removed empty set %s", setId.c_str());
        }
    }
    else {
        logger.warn("Could not find set %s", setId.c_str());
    }
}

bool PvaPyDataDistributor::updateClient(int clientId, const std::string& setId, const std::string& triggerFieldValue)
{
    epvd::Lock lock(mutex);
    logger.debug("Looking to update client %d for set %s", clientId, setId.c_str());
    bool proceedWithUpdate = false;
    if (currentSetIdIter == clientSetIdList.end()) {
        currentSetIdIter = clientSetIdList.begin();
    }
    std::string currentSetId = *currentSetIdIter;
    if (setId != currentSetId) {
        // We are not distributing data to this set at the moment
        logger.debug("Set %s is not receiving current updates, client %d will not be updated", setId.c_str(), clientId);
        return proceedWithUpdate;
    }
    ClientSetPtr setPtr = clientSetMap[currentSetId];
    if (setPtr->currentClientIdIter == setPtr->clientIdList.end()) {
        // Move current client iterator to the beginning of the list
        setPtr->currentClientIdIter = setPtr->clientIdList.begin();
    }
    if (lastUpdateValue == triggerFieldValue) {
        // This update was already distributed.
        logger.debug("Update %s has already been distributed, client %d will not be updated", lastUpdateValue.c_str(), clientId);
        return proceedWithUpdate;
    }
    switch (setPtr->updateMode) {
        case(DD_UPDATE_ONE_PER_GROUP): {
            if (clientId != *(setPtr->currentClientIdIter)) {
                // Not this client's turn.
                logger.debug("Client %d will not be updated, current client is %d", clientId, *(setPtr->currentClientIdIter));
                return proceedWithUpdate;
            }
            proceedWithUpdate = true;
            lastUpdateValue = triggerFieldValue;
            setPtr->lastUpdateValue = triggerFieldValue;
            setPtr->updateCounter++;
            logger.debug("Client %d will be updated", clientId);
            if (setPtr->updateCounter >= setPtr->nUpdatesPerClient) {
                // This client and set are done.
                logger.debug("Set %s is done after %d updates", setId.c_str(), setPtr->updateCounter);
                setPtr->currentClientIdIter++;
                setPtr->updateCounter = 0;
                currentSetIdIter++;
            }
            break;
        }
        case(DD_UPDATE_ALL_IN_GROUP): {
            proceedWithUpdate = true;
            static unsigned int nClientsUpdated = 0;
            if (setPtr->lastUpdateValue != triggerFieldValue) {
                setPtr->lastUpdateValue = triggerFieldValue;
                setPtr->updateCounter++;
                nClientsUpdated = 0;
            }
            nClientsUpdated++;
            logger.debug("Set %s: number of clients updated: %d, update counter: %d", setId.c_str(), nClientsUpdated, setPtr->updateCounter);
            if (nClientsUpdated == setPtr->clientIdList.size() && setPtr->updateCounter >= setPtr->nUpdatesPerClient) {
                // This set is done.
                lastUpdateValue = triggerFieldValue;
                logger.debug("Set %s is done after %d updates", setId.c_str(), setPtr->updateCounter);
                setPtr->updateCounter = 0;
                currentSetIdIter++;
            }
            break;
        }
        default: {
            proceedWithUpdate = true;
        }
    }
    return proceedWithUpdate;
}

PvaPyDataDistributorPlugin::PvaPyDataDistributorPlugin()
{
}

PvaPyDataDistributorPlugin::~PvaPyDataDistributorPlugin()
{
}

void PvaPyDataDistributorPlugin::create()
{
    initialize();
}

bool PvaPyDataDistributorPlugin::initialize()
{
    PvaPyDataDistributorPluginPtr pvPlugin = PvaPyDataDistributorPluginPtr(new PvaPyDataDistributorPlugin());
    PVPluginRegistry::registerPlugin(name,pvPlugin);
    return true;
}

PVFilterPtr PvaPyDataDistributorPlugin::create(
     const std::string& requestValue,
     const PVCopyPtr& pvCopy,
     const PVFieldPtr& master)
{
    return PvaPyDataDistributorFilter::create(requestValue,pvCopy,master);
}

PvaPyDataDistributorFilter::~PvaPyDataDistributorFilter()
{
    dataDistributorPtr->removeClient(clientId, setId);
    PvaPyDataDistributor::removeUnusedInstance(dataDistributorPtr);
}

PvaPyDataDistributorFilterPtr PvaPyDataDistributorFilter::create(
     const std::string& requestValue,
     const PVCopyPtr& pvCopy,
     const PVFieldPtr& master)
{
    static int clientId = 0;
    clientId++;
      
    logger.debug("Creating distributor filter with request: %s", requestValue.c_str());
    std::vector<std::string> configItems = StringUtility::split(requestValue, ';');
    // Use lowercase keys if possible.
    std::string requestValue2 = StringUtility::toLowerCase(requestValue); 
    std::vector<std::string> configItems2 = StringUtility::split(requestValue2, ';');
    int nUpdatesPerClient = 1;
    int updateMode = PvaPyDataDistributor::DD_UPDATE_ONE_PER_GROUP;
    std::string groupId = "default";
    std::string setId = "default";
    std::string triggerField = "timeStamp";
    bool hasUpdateMode = false;
    bool hasSetId = false;
    for(unsigned int i = 0; i < configItems2.size(); i++) {
        std::string configItem2 = configItems2[i];
        size_t ind = configItem2.find(':');
        if (ind == string::npos) {
            logger.debug("No value specified for request option: %s", configItem2.c_str());
            continue;
        }
        if(configItem2.find("updates") == 0) {
            std::string svalue = configItem2.substr(ind+1);
            nUpdatesPerClient = atoi(svalue.c_str());
            logger.debug("Request spec for nUpdatesPerClient: %d", nUpdatesPerClient);
        }
        else if(configItem2.find("group") == 0) {
            std::string configItem = configItems[i];
            groupId = configItem.substr(ind+1);
            logger.debug("Request spec for groupId: %s", groupId.c_str());
        }
        else if(configItem2.find("set") == 0) {
            std::string configItem = configItems[i];
            setId = configItem.substr(ind+1);
            hasSetId = true;
            logger.debug("Request spec for setId: %s", setId.c_str());
        }
        else if(configItem2.find("mode") == 0) {
            std::string svalue = StringUtility::toLowerCase(configItem2.substr(ind+1));
            if (svalue == "one") {
                updateMode = PvaPyDataDistributor::DD_UPDATE_ONE_PER_GROUP;
                hasUpdateMode = true;
            }
            else if (svalue == "all") {
                updateMode = PvaPyDataDistributor::DD_UPDATE_ALL_IN_GROUP;
                hasUpdateMode = true;
            }
            if (!hasUpdateMode) {
                logger.debug("Invalid request spec for updateMode: %s", svalue.c_str());
            }
            else {
                logger.debug("Request spec for updateMode: %d", updateMode);
            }
        }
        else if(configItem2.find("trigger") == 0) {
            std::string configItem = configItems[i];
            triggerField = configItem.substr(ind+1);
            logger.debug("Request spec for trigger field: %s", triggerField.c_str());
        }
    }
    // If request does not have update mode specified, but has set id
    // then use a different update mode
    if(!hasUpdateMode && hasSetId) {
        updateMode = PvaPyDataDistributor::DD_UPDATE_ALL_IN_GROUP;
        logger.debug("Request specifies set id but does not specify update mode; using update mode: %d", updateMode);
    }

    // Make sure request is valid
    if(nUpdatesPerClient <= 0) {
        return PvaPyDataDistributorFilterPtr();
    }
    PvaPyDataDistributorFilterPtr filter =
         PvaPyDataDistributorFilterPtr(new PvaPyDataDistributorFilter(groupId, clientId, setId, triggerField, nUpdatesPerClient, updateMode, pvCopy, master));
    return filter;
}

PvaPyDataDistributorFilter::PvaPyDataDistributorFilter(const std::string& groupId_, int clientId_, const std::string& setId_, const std::string& triggerField_, int nUpdatesPerClient, int updateMode, const PVCopyPtr& copyPtr_, const epvd::PVFieldPtr& masterFieldPtr_)
    : dataDistributorPtr(PvaPyDataDistributor::getInstance(groupId_))
    , clientId(clientId_)
    , setId(setId_)
    , triggerField(triggerField_)
    , masterFieldPtr(masterFieldPtr_)
    , triggerFieldPtr()
    , firstUpdate(true)
{
    triggerField = dataDistributorPtr->addClient(clientId, setId, triggerField, nUpdatesPerClient, updateMode);
    if(masterFieldPtr->getField()->getType() == epvd::structure) {
        epvd::PVStructurePtr pvStructurePtr = static_pointer_cast<epvd::PVStructure>(masterFieldPtr);
        if(pvStructurePtr) {
            triggerFieldPtr = pvStructurePtr->getSubField(triggerField);
        }
    }
    if(!triggerFieldPtr) {
        logger.debug("Using master field as trigger field");
        triggerFieldPtr = masterFieldPtr;
    }
}


bool PvaPyDataDistributorFilter::filter(const PVFieldPtr& pvCopy, const BitSetPtr& bitSet, bool toCopy)
{
    if(!toCopy) {
        return false;
    }

    bool proceedWithUpdate = false;
    if(firstUpdate) {
        // Always send first update
        firstUpdate = false;
        proceedWithUpdate = true;
    }
    else {
        std::stringstream ss;
        ss << triggerFieldPtr;
        std::string triggerFieldValue = ss.str();
        proceedWithUpdate = dataDistributorPtr->updateClient(clientId, setId, triggerFieldValue);
    }

    if(proceedWithUpdate) {
        pvCopy->copyUnchecked(*masterFieldPtr);
        bitSet->set(pvCopy->getFieldOffset());
    } 
    else {
        // Clear all bits
        //bitSet->clear(pvCopy->getFieldOffset());
        bitSet->clear();
    }

    return true;
}

string PvaPyDataDistributorFilter::getName()
{
    return name;
}

}}
