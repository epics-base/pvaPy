// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PVA_MIRROR_SERVER_H
#define PVA_MIRROR_SERVER_H

#include <string>
#include <map>
#include <boost/python/list.hpp>
#include <pv/pvData.h>
#include <pv/pvAccess.h>
#include <pv/serverContext.h>
#include <pv/pvaClient.h>

#include "ChannelMonitorRequesterImpl.h"
#include "ChannelStateRequesterImpl.h"
#include "ChannelMonitorDataProcessor.h"
#include "PvaClient.h"
#include "CaClient.h"
#include "PvProvider.h"
#include "PvaPyLogger.h"
#include "PvaServer.h"

class PvaMirrorServer;
class MirrorChannelDataProcessor;
typedef std::tr1::shared_ptr<MirrorChannelDataProcessor> MirrorChannelDataProcessorPtr;
class MirrorChannelMonitor;
typedef std::tr1::shared_ptr<MirrorChannelMonitor> MirrorChannelMonitorPtr;
 
// This class updates PVA server record and handles source channel connection
// changes

class MirrorChannelDataProcessor : public ChannelMonitorDataProcessor
{
public:
    MirrorChannelDataProcessor(PvaMirrorServer* pvaMirrorServer, const std::string& mirrorChannelName, unsigned int nSrcMonitors);
    virtual ~MirrorChannelDataProcessor();

    virtual void processMonitorData(epics::pvData::PVStructurePtr pvStructurePtr);
    virtual void onChannelConnect();
    virtual void onChannelDisconnect();

private:
    static PvaPyLogger logger;
    PvaMirrorServer *pvaMirrorServer;
    std::string mirrorChannelName;
    epics::pvData::Mutex mutex;
    bool recordAdded;
    unsigned int nSrcMonitors;
    int nUpdatesToSkip;
};

 
// The purpose of this class is to simply establish channel monitor to 
// provide updates to the PVA server record

class MirrorChannelMonitor : public ChannelMonitorDataProcessor
{
public:

    MirrorChannelMonitor(const std::string& channelName, PvProvider::ProviderType providerType, unsigned int serverQueueSize, const std::string& fieldRequestDescriptor, MirrorChannelDataProcessorPtr dataProcessorPtr);
    MirrorChannelMonitor(const MirrorChannelMonitor& mirrorChannelMonitor);
    virtual ~MirrorChannelMonitor();

    bool isChannelConnected() const;
    std::string getChannelName() const;

    // ChannelMonitorDataProcessor
    virtual void processMonitorData(epics::pvData::PVStructurePtr pvStructurePtr);
    virtual void onChannelConnect();
    virtual void onChannelDisconnect();

    virtual void resetMonitorCounters();
    virtual boost::python::dict getMonitorCounters();

private:

    static PvaPyLogger logger;
    static PvaClient pvaClient;
    static CaClient caClient;

    void issueConnect();
    void startMonitor();
    void stopMonitor();

    static epics::pvaClient::PvaClientPtr pvaClientPtr;
    epics::pvaClient::PvaClientChannelPtr pvaClientChannelPtr;
    epics::pvaClient::PvaClientMonitorRequesterPtr pvaClientMonitorRequesterPtr;
    epics::pvaClient::PvaClientMonitorPtr pvaClientMonitorPtr;

    std::string channelName;
    PvProvider::ProviderType providerType;
    unsigned int serverQueueSize;
    std::string fieldRequestDescriptor;
    MirrorChannelDataProcessorPtr dataProcessorPtr;

    bool isConnected;
    bool hasIssuedConnect;
    bool monitorActive;
    epics::pvaClient::PvaClientChannelStateChangeRequesterPtr stateRequester;
};

class PvaMirrorServer : public PvaServer
{
public:
    PvaMirrorServer();
    PvaMirrorServer(const PvaMirrorServer&);
    virtual ~PvaMirrorServer();

    virtual void addMirrorRecord(const std::string& mirrorChannelName, const std::string& srcChannelName, PvProvider::ProviderType srcProviderType);
    virtual void addMirrorRecord(const std::string& mirrorChannelName, const std::string& srcChannelName, PvProvider::ProviderType srcProviderType, unsigned int srcQueueSize);
    virtual void addMirrorRecord(const std::string& mirrorChannelName, const std::string& srcChannelName, PvProvider::ProviderType srcProviderType, unsigned int srcQueueSize, unsigned int nSrcMonitors, const std::string& srcFieldRequestDescriptor);
    virtual void removeMirrorRecord(const std::string& mirrorChannelName);

    virtual void removeAllMirrorRecords();
    virtual bool hasMirrorRecord(const std::string& mirrorChannelName);
    virtual void resetMirrorRecordCounters(const std::string& mirrorChannelName);
    virtual boost::python::dict getMirrorRecordCounters(const std::string& mirrorChannelName);
    virtual boost::python::list getMirrorRecordNames();

private:

    static PvaPyLogger logger;
    std::multimap<std::string, MirrorChannelMonitorPtr> mirrorChannelMonitorMap;
};

#endif

