// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PVAPY_DATA_DISTRIBUTOR_PLUGIN_H
#define PVAPY_DATA_DISTRIBUTOR_PLUGIN_H


#include <string>
#include <map>
#include <list>
#include <pv/lock.h>
#include <pv/pvData.h>
#include <pv/pvPlugin.h>

#include <shareLib.h>

#include "PvaPyLogger.h"

namespace epics { namespace pvCopy {

class PvaPyDataDistributorPlugin;
class PvaPyDataDistributorFilter;
class PvaPyDataDistributor;

typedef std::tr1::shared_ptr<PvaPyDataDistributorPlugin> PvaPyDataDistributorPluginPtr;
typedef std::tr1::shared_ptr<PvaPyDataDistributorFilter> PvaPyDataDistributorFilterPtr;
typedef std::tr1::shared_ptr<PvaPyDataDistributor> PvaPyDataDistributorPtr;

struct ClientSet;
typedef std::tr1::shared_ptr<ClientSet> ClientSetPtr;
typedef std::tr1::shared_ptr<const ClientSet> ClientSetConstPtr;

struct ClientSet
{
    POINTER_DEFINITIONS(ClientSet);

    ClientSet(const std::string& setId_, const std::string triggerField_, int nUpdatesPerClient_, int updateMode_) 
        : setId(setId_)
        , triggerField(triggerField_)
        , nUpdatesPerClient(nUpdatesPerClient_)
        , updateMode(updateMode_)
        , clientIdList()
        , lastUpdateValue()
        , updateCounter(0)
        , currentClientIdIter(clientIdList.end())
        {}
    ~ClientSet() {}
    std::string setId;
    std::string triggerField;
    int nUpdatesPerClient;
    int updateMode;
    std::list<int> clientIdList;
    std::string lastUpdateValue;
    int updateCounter;
    std::list<int>::iterator currentClientIdIter;
};

class PvaPyDataDistributor 
{
public:
    enum ClientUpdateMode {
        DD_UPDATE_ONE_PER_GROUP = 0, // Update goes to one client per set
        DD_UPDATE_ALL_IN_GROUP = 1,  // Update goes to all clients in set
        DD_N_UPDATE_MODES = 2        // Number of valid update modes
    };

    static PvaPyDataDistributorPtr getInstance(const std::string& groupId);
    static void removeUnusedInstance(PvaPyDataDistributorPtr dataDistributorPtr);

    virtual ~PvaPyDataDistributor();
    std::string getGroupId() const { return groupId; }
    std::string addClient(int clientId, const std::string& setId, const std::string& triggerField, int nUpdatesPerClient, int updateMode);
    void removeClient(int clientId, const std::string& setId);
    bool updateClient(int clientId, const std::string& setId, const std::string& triggerFieldValue);

private:
    PvaPyDataDistributor(const std::string& id);
    PvaPyDataDistributor(const PvaPyDataDistributor& distributor);
    PvaPyDataDistributor& operator=(const PvaPyDataDistributor& distributor);

    static PvaPyLogger logger;
    static std::map<std::string, PvaPyDataDistributorPtr> dataDistributorMap;
    static epics::pvData::Mutex dataDistributorMapMutex;

    std::string groupId;
    epics::pvData::Mutex mutex;
    std::map<std::string, ClientSetPtr> clientSetMap;
    std::list<std::string> clientSetIdList;
    std::list<std::string>::iterator currentSetIdIter;
    std::string lastUpdateValue;
};

class epicsShareClass PvaPyDataDistributorPlugin : public PVPlugin
{
private:
    PvaPyDataDistributorPlugin();
public:
    POINTER_DEFINITIONS(PvaPyDataDistributorPlugin);
    virtual ~PvaPyDataDistributorPlugin();
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
class epicsShareClass PvaPyDataDistributorFilter : public PVFilter
{
private:
    static PvaPyLogger logger;

    PvaPyDataDistributorPtr dataDistributorPtr;
    int clientId;
    std::string setId;
    std::string triggerField;
    epics::pvData::PVFieldPtr masterFieldPtr;
    epics::pvData::PVFieldPtr triggerFieldPtr;
    bool firstUpdate;

    PvaPyDataDistributorFilter(const std::string& groupId, int clientId, const std::string& setId, const std::string& triggerField, int nUpdatesPerClient, int updateMode, const epics::pvCopy::PVCopyPtr& copyPtr, const epics::pvData::PVFieldPtr& masterFieldPtr);

public:
    POINTER_DEFINITIONS(PvaPyDataDistributorFilter);
    virtual ~PvaPyDataDistributorFilter();
    /**
     * Create a PvaPyDataDistributorFilter.
     * @param requestValue The value part of a name=value request option.
     * @param master The field in the master PVStructure to which the PVFilter will be attached.
     * @return The PVFilter.
     * A null is returned if master or requestValue is not appropriate for the plugin.
     */
    static PvaPyDataDistributorFilterPtr create(
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
