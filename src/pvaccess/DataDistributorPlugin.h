// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef DATA_DISTRIBUTOR_PLUGIN_H
#define DATA_DISTRIBUTOR_PLUGIN_H


#include <string>
#include <map>
#include <list>
#include <pv/lock.h>
#include <pv/pvData.h>
#include <pv/pvPlugin.h>

#include <shareLib.h>

#include "PvaPyLogger.h"

namespace epics { namespace pvCopy {

class DataDistributorPlugin;
class DataDistributorFilter;

typedef std::tr1::shared_ptr<DataDistributorPlugin> DataDistributorPluginPtr;
typedef std::tr1::shared_ptr<DataDistributorFilter> DataDistributorFilterPtr;

struct ConsumerGroup;
typedef std::tr1::shared_ptr<ConsumerGroup> ConsumerGroupPtr;
typedef std::tr1::shared_ptr<const ConsumerGroup> ConsumerGroupConstPtr;

struct ConsumerGroup
{
    POINTER_DEFINITIONS(ConsumerGroup);

    ConsumerGroup(const std::string& groupId_, const std::string distinguishingField_, int nUpdatesPerConsumer_, int updateMode_) 
        : groupId(groupId_)
        , distinguishingField(distinguishingField_)
        , nUpdatesPerConsumer(nUpdatesPerConsumer_)
        , updateMode(updateMode_)
        , consumerIdList()
        , lastUpdateValue()
        , updateCounter(0)
        , currentConsumerIdIter(consumerIdList.end())
        {}
    ~ConsumerGroup() {}
    std::string groupId;
    std::string distinguishingField;
    int nUpdatesPerConsumer;
    int updateMode;
    std::list<int> consumerIdList;
    std::string lastUpdateValue;
    int updateCounter;
    std::list<int>::iterator currentConsumerIdIter;
};

class DataDistributor 
{
public:
    enum ConsumerUpdateMode {
        DD_UPDATE_ONE_PER_GROUP = 0, // Update goes to one consumer per group
        DD_UPDATE_ALL_IN_GROUP = 1,  // Update goes to all consumers in group
        DD_N_UPDATE_MODES = 2        // Number of valid update modes
    };

    static DataDistributor& getInstance();
    virtual ~DataDistributor();
    std::string addConsumer(int consumerId, const std::string& groupId, const std::string& distinguishingField, int nUpdatesPerConsumer, int updateMode);
    void removeConsumer(int consumerId, const std::string& groupId);
    bool updateConsumer(int consumerId, const std::string& groupId, const std::string& distinguishingFieldValue);

private:
    DataDistributor();
    DataDistributor(const DataDistributor& distributor);
    DataDistributor& operator=(const DataDistributor& distributor);

    static PvaPyLogger logger;
    epics::pvData::Mutex mutex;
    std::map<std::string, ConsumerGroupPtr> consumerGroupMap;
    std::list<std::string> consumerGroupIdList;
    std::list<std::string>::iterator currentGroupIdIter;
    std::string lastUpdateValue;
};

class epicsShareClass DataDistributorPlugin : public PVPlugin
{
private:
    DataDistributorPlugin();
public:
    POINTER_DEFINITIONS(DataDistributorPlugin);
    virtual ~DataDistributorPlugin();
    /**
     * Factory
     */
    static void create();
    /**
     * Create a PVFilter.
     * @param requestValue The value part of a name=value request option.
     * @param pvCopy The PVCopy to which the PVFilter will be attached.
     * @param master The field in the master PVStructure to which the PVFilter will be attached
     * @return The PVFilter.
     * Null is returned if master or requestValue is not appropriate for the plugin.
     */
    virtual PVFilterPtr create(
         const std::string& requestValue,
         const PVCopyPtr& pvCopy,
         const epics::pvData::PVFieldPtr& master);
private:
    static PvaPyLogger logger;

    static bool initialize();
    static bool initialized;
};

/**
 * @brief  A Plugin for a filter that gets a sub array from a PVScalarDeadband.
 */
class epicsShareClass DataDistributorFilter : public PVFilter
{
private:
    static PvaPyLogger logger;

    int consumerId;
    std::string groupId;
    std::string distinguishingField;
    epics::pvData::PVFieldPtr masterFieldPtr;
    epics::pvData::PVFieldPtr distinguishingFieldPtr;
    bool firstUpdate;

    DataDistributorFilter(int consumerId, const std::string& groupId, const std::string& distinguishingField, int nUpdatesPerConsumer, int updateMode, const epics::pvCopy::PVCopyPtr& copyPtr, const epics::pvData::PVFieldPtr& masterFieldPtr);

public:
    POINTER_DEFINITIONS(DataDistributorFilter);
    virtual ~DataDistributorFilter();
    /**
     * Create a DataDistributorFilter.
     * @param requestValue The value part of a name=value request option.
     * @param master The field in the master PVStructure to which the PVFilter will be attached.
     * @return The PVFilter.
     * A null is returned if master or requestValue is not appropriate for the plugin.
     */
    static DataDistributorFilterPtr create(
        const std::string& requestValue,
        const PVCopyPtr& pvCopy,
        const epics::pvData::PVFieldPtr & master);
    /**
     * Perform a filter operation
     * @param pvCopy The field in the copy PVStructure.
     * @param bitSet A bitSet for copyPVStructure.
     * @param toCopy (true,false) means copy (from master to copy,from copy to master)
     * @return if filter (modified, did not modify) destination.
     * Null is returned if master or requestValue is not appropriate for the plugin.
     */
    bool filter(const epics::pvData::PVFieldPtr & pvCopy,const epics::pvData::BitSetPtr & bitSet,bool toCopy);
    /**
     * Get the filter name.
     * @return The name.
     */
    std::string getName();
};

}}
#endif  
