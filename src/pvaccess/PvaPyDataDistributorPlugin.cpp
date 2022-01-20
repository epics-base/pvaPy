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

PvaPyDataDistributorPtr PvaPyDataDistributor::getInstance(const std::string& id)
{
    epvd::Lock lock(dataDistributorMapMutex);
    std::map<std::string,PvaPyDataDistributorPtr>::iterator ddit = dataDistributorMap.find(id);
    if (ddit != dataDistributorMap.end()) {
        PvaPyDataDistributorPtr ddPtr = ddit->second;
        return ddPtr;
    }
    else {
        PvaPyDataDistributorPtr ddPtr(new PvaPyDataDistributor(id));
        dataDistributorMap[id] = ddPtr;
        logger.debug("Created new data distributor: %s", id.c_str());
        return ddPtr;
    }
}

void PvaPyDataDistributor::removeUnusedInstance(PvaPyDataDistributorPtr dataDistributorPtr)
{
    epvd::Lock lock(dataDistributorMapMutex);
    std::string distributorId = dataDistributorPtr->getId();
    std::map<std::string,PvaPyDataDistributorPtr>::iterator ddit = dataDistributorMap.find(distributorId);
    if (ddit != dataDistributorMap.end()) {
        PvaPyDataDistributorPtr ddPtr = ddit->second;
        int nGroups = ddPtr->consumerGroupMap.size();
        logger.debug("Number of active consumer groups: %d", nGroups);
        if (nGroups == 0) {
            dataDistributorMap.erase(ddit);
            logger.debug("Removed data distributor: %s", distributorId.c_str());
        }
    }
}

PvaPyDataDistributor::PvaPyDataDistributor(const std::string& id_)
    : id(id_)
    , mutex()
    , consumerGroupMap()
    , consumerGroupIdList()
    , currentGroupIdIter(consumerGroupIdList.end())
    , lastUpdateValue()
{
}

PvaPyDataDistributor::~PvaPyDataDistributor()
{
    epvd::Lock lock(mutex);
    consumerGroupMap.clear();
    consumerGroupIdList.clear();
}

std::string PvaPyDataDistributor::addConsumer(int consumerId, const std::string& groupId, const std::string& uniqueField, int nUpdatesPerConsumer, int updateMode)
{
    epvd::Lock lock(mutex);
    std::map<std::string,ConsumerGroupPtr>::iterator git = consumerGroupMap.find(groupId);
    if (git != consumerGroupMap.end()) {
        ConsumerGroupPtr groupPtr = git->second;
        groupPtr->consumerIdList.push_back(consumerId);
        logger.debug("Added consumer %d to existing group %s", consumerId, groupId.c_str());
        return groupPtr->uniqueField;
    }
    else {
        ConsumerGroupPtr groupPtr(new ConsumerGroup(groupId, uniqueField, nUpdatesPerConsumer, updateMode));
        groupPtr->consumerIdList.push_back(consumerId);
        consumerGroupMap[groupId] = groupPtr;
        consumerGroupIdList.push_back(groupId);
        logger.debug("Added consumer %d to new group %s (unique field: %s, nUpdatesPerConsumer: %d)", consumerId, groupId.c_str(), uniqueField.c_str(), nUpdatesPerConsumer);
        return uniqueField;
    }
}

void PvaPyDataDistributor::removeConsumer(int consumerId, const std::string& groupId)
{
    epvd::Lock lock(mutex);
    logger.debug("Removing consumer %d from group %s", consumerId, groupId.c_str());
    std::map<std::string,ConsumerGroupPtr>::iterator git = consumerGroupMap.find(groupId);
    if (git != consumerGroupMap.end()) {
        ConsumerGroupPtr groupPtr = git->second;
        std::list<int>::iterator cit = std::find(groupPtr->consumerIdList.begin(), groupPtr->consumerIdList.end(), consumerId);
        if (cit != groupPtr->consumerIdList.end()) {
            // If we are removing current consumer id, advance iterator
            if (cit == groupPtr->currentConsumerIdIter) {
                logger.debug("Advancing current consumer id iterator for group %s", groupId.c_str());
                groupPtr->currentConsumerIdIter++;
            }

            // Find current consumer id 
            int currentConsumerId = -1;
            if (groupPtr->currentConsumerIdIter != groupPtr->consumerIdList.end()) {
                currentConsumerId = *(groupPtr->currentConsumerIdIter);
            }

            // Remove consumer id from the list
            groupPtr->consumerIdList.erase(cit);
            logger.debug("Removed consumer %d from group %s", consumerId, groupId.c_str());

             // Reset current consumer id iterator
            groupPtr->currentConsumerIdIter = groupPtr->consumerIdList.end();
            if (currentConsumerId < 0) {
                logger.debug("Current consumer id is not set");
            }
            else {
                std::list<int>::iterator cit2 = std::find(groupPtr->consumerIdList.begin(), groupPtr->consumerIdList.end(), currentConsumerId);
                if (cit2 != groupPtr->consumerIdList.end()) {
                    logger.debug("Current consumer id is set to %d", currentConsumerId);
                    groupPtr->currentConsumerIdIter = cit2;
                }
                else {
                    logger.warn("Could not find current consumer %d in group %s", currentConsumerId, groupId.c_str());
                }
            }
        }
        else {
            logger.warn("Could not find consumer %d in group %s", consumerId, groupId.c_str());
        }
       
        logger.debug("Number of consumers in group %s: %d", groupId.c_str(), groupPtr->consumerIdList.size());
        if (groupPtr->consumerIdList.size() == 0) {
            consumerGroupMap.erase(git);
            std::list<std::string>::iterator git2 = std::find(consumerGroupIdList.begin(), consumerGroupIdList.end(), groupId);
            if (git2 == currentGroupIdIter) {
                logger.debug("Group %s will be removed, advancing current group iterator", groupId.c_str());
                currentGroupIdIter++;
            }
            if (git2 != consumerGroupIdList.end()) {
                consumerGroupIdList.erase(git2);
            }
            logger.debug("Removed empty group %s", groupId.c_str());
        }
    }
    else {
        logger.warn("Could not find group %s", groupId.c_str());
    }
}

bool PvaPyDataDistributor::updateConsumer(int consumerId, const std::string& groupId, const std::string& uniqueFieldValue)
{
    epvd::Lock lock(mutex);
    logger.debug("Looking to update consumer %d for group %s", consumerId, groupId.c_str());
    bool proceedWithUpdate = false;
    if (currentGroupIdIter == consumerGroupIdList.end()) {
        currentGroupIdIter = consumerGroupIdList.begin();
    }
    std::string currentGroupId = *currentGroupIdIter;
    if (groupId != currentGroupId) {
        // We are not distributing data to this group at the moment
        logger.debug("Group %s is not receiving current updates, consumer %d will not be updated", groupId.c_str(), consumerId);
        return proceedWithUpdate;
    }
    ConsumerGroupPtr groupPtr = consumerGroupMap[currentGroupId];
    if (groupPtr->currentConsumerIdIter == groupPtr->consumerIdList.end()) {
        // Move current consumer iterator to the beginning of the list
        groupPtr->currentConsumerIdIter = groupPtr->consumerIdList.begin();
    }
    if (lastUpdateValue == uniqueFieldValue) {
        // This update was already distributed.
        logger.debug("Update %s has already been distributed, consumer %d will not be updated", lastUpdateValue.c_str(), consumerId);
        return proceedWithUpdate;
    }
    switch (groupPtr->updateMode) {
        case(DD_UPDATE_ONE_PER_GROUP): {
            if (consumerId != *(groupPtr->currentConsumerIdIter)) {
                // Not this consumer's turn.
                logger.debug("Consumer %d will not be updated, current consumer is %d", consumerId, *(groupPtr->currentConsumerIdIter));
                return proceedWithUpdate;
            }
            proceedWithUpdate = true;
            lastUpdateValue = uniqueFieldValue;
            groupPtr->lastUpdateValue = uniqueFieldValue;
            groupPtr->updateCounter++;
            logger.debug("Consumer %d will be updated", consumerId);
            if (groupPtr->updateCounter >= groupPtr->nUpdatesPerConsumer) {
                // This consumer and group are done.
                logger.debug("Group %s is done after %d updates", groupId.c_str(), groupPtr->updateCounter);
                groupPtr->currentConsumerIdIter++;
                groupPtr->updateCounter = 0;
                currentGroupIdIter++;
            }
            break;
        }
        case(DD_UPDATE_ALL_IN_GROUP): {
            proceedWithUpdate = true;
            static unsigned int nConsumersUpdated = 0;
            if (groupPtr->lastUpdateValue != uniqueFieldValue) {
                groupPtr->lastUpdateValue = uniqueFieldValue;
                groupPtr->updateCounter++;
                nConsumersUpdated = 0;
            }
            nConsumersUpdated++;
            logger.debug("Group %s: number of consumers updated: %d, update counter: %d", groupId.c_str(), nConsumersUpdated, groupPtr->updateCounter);
            if (nConsumersUpdated == groupPtr->consumerIdList.size() && groupPtr->updateCounter >= groupPtr->nUpdatesPerConsumer) {
                // This group is done.
                lastUpdateValue = uniqueFieldValue;
                logger.debug("Group %s is done after %d updates", groupId.c_str(), groupPtr->updateCounter);
                groupPtr->updateCounter = 0;
                currentGroupIdIter++;
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
    dataDistributorPtr->removeConsumer(consumerId, groupId);
    PvaPyDataDistributor::removeUnusedInstance(dataDistributorPtr);
}

PvaPyDataDistributorFilterPtr PvaPyDataDistributorFilter::create(
     const std::string& requestValue,
     const PVCopyPtr& pvCopy,
     const PVFieldPtr& master)
{
    static int consumerId = 0;
    consumerId++;
      
    logger.debug("Creating distributor filter with request: %s", requestValue.c_str());
    std::vector<std::string> configItems = StringUtility::split(requestValue, ';');
    // Use lowercase keys if possible.
    std::string requestValue2 = StringUtility::toLowerCase(requestValue); 
    std::vector<std::string> configItems2 = StringUtility::split(requestValue2, ';');
    int nUpdatesPerConsumer = 1;
    int updateMode = PvaPyDataDistributor::DD_UPDATE_ONE_PER_GROUP;
    std::string distributorId = "default";
    std::string groupId = "default";
    std::string uniqueField = "timeStamp";
    bool hasUpdateMode = false;
    bool hasGroupId = false;
    for(unsigned int i = 0; i < configItems2.size(); i++) {
        std::string configItem2 = configItems2[i];
        size_t ind = configItem2.find(':');
        if (ind == string::npos) {
            logger.debug("No value specified for request option: %s", configItem2.c_str());
   
        }
        if(configItem2.find("nupdatesperconsumer") == 0) {
            std::string svalue = configItem2.substr(ind+1);
            nUpdatesPerConsumer = atoi(svalue.c_str());
            logger.debug("Request spec for nUpdatesPerConsumer: %d", nUpdatesPerConsumer);
        }
        else if(configItem2.find("distributorid") == 0) {
            std::string configItem = configItems[i];
            distributorId = configItem.substr(ind+1);
            logger.debug("Request spec for distributorId: %s", distributorId.c_str());
        }
        else if(configItem2.find("groupid") == 0) {
            std::string configItem = configItems[i];
            groupId = configItem.substr(ind+1);
            hasGroupId = true;
            logger.debug("Request spec for groupId: %s", groupId.c_str());
        }
        else if(configItem2.find("updatemode") == 0) {
            std::string svalue = configItem2.substr(ind+1);
            updateMode = atoi(svalue.c_str());
            hasUpdateMode = true;
            logger.debug("Request spec for updateMode: %d", updateMode);
        }
        else if(configItem2.find("uniquefield") == 0) {
            std::string configItem = configItems[i];
            uniqueField = configItem.substr(ind+1);
            logger.debug("Request spec for unique field: %s", uniqueField.c_str());
        }
    }
    // If request does not have update mode specified, but has group id
    // then use a different update mode
    if(!hasUpdateMode && hasGroupId) {
        updateMode = PvaPyDataDistributor::DD_UPDATE_ALL_IN_GROUP;
    }

    // Make sure request is valid
    if(nUpdatesPerConsumer <= 0) {
        return PvaPyDataDistributorFilterPtr();
    }
    PvaPyDataDistributorFilterPtr filter =
         PvaPyDataDistributorFilterPtr(new PvaPyDataDistributorFilter(distributorId, consumerId, groupId, uniqueField, nUpdatesPerConsumer, updateMode, pvCopy, master));
    return filter;
}

PvaPyDataDistributorFilter::PvaPyDataDistributorFilter(const std::string& distributorId_, int consumerId_, const std::string& groupId_, const std::string& uniqueField_, int nUpdatesPerConsumer, int updateMode, const PVCopyPtr& copyPtr_, const epvd::PVFieldPtr& masterFieldPtr_)
    : dataDistributorPtr(PvaPyDataDistributor::getInstance(distributorId_))
    , consumerId(consumerId_)
    , groupId(groupId_)
    , uniqueField(uniqueField_)
    , masterFieldPtr(masterFieldPtr_)
    , uniqueFieldPtr()
    , firstUpdate(true)
{
    uniqueField = dataDistributorPtr->addConsumer(consumerId, groupId, uniqueField, nUpdatesPerConsumer, updateMode);
    if(masterFieldPtr->getField()->getType() == epvd::structure) {
        epvd::PVStructurePtr pvStructurePtr = static_pointer_cast<epvd::PVStructure>(masterFieldPtr);
        if(pvStructurePtr) {
            uniqueFieldPtr = pvStructurePtr->getSubField(uniqueField);
        }
    }
    if(!uniqueFieldPtr) {
        logger.debug("Using master field as unique field");
        uniqueFieldPtr = masterFieldPtr;
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
        ss << uniqueFieldPtr;
        std::string uniqueFieldValue = ss.str();
        proceedWithUpdate = dataDistributorPtr->updateConsumer(consumerId, groupId, uniqueFieldValue);
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
