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
class MirrorChannel;
typedef std::tr1::shared_ptr<MirrorChannel> MirrorChannelPtr;
 
class MirrorChannelDataProcessor : public ChannelMonitorDataProcessor
{
public:
    MirrorChannelDataProcessor(PvaMirrorServer* pvaMirrorServer, const std::string& mirrorChannelName);
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
};

 
// The purpose of this class is to simply establish channel monitor to 
// provide updates to the PVA server record

class MirrorChannel : public ChannelMonitorDataProcessor
{
public:

    MirrorChannel(const std::string& channelName, PvProvider::ProviderType providerType, unsigned int serverQueueSize, MirrorChannelDataProcessorPtr dataProcessorPtr);
    MirrorChannel(const MirrorChannel& mirrorChannel);
    virtual ~MirrorChannel();

    bool isChannelConnected() const;
    std::string getChannelName() const;

    // ChannelMonitorDataProcessor
    virtual void processMonitorData(epics::pvData::PVStructurePtr pvStructurePtr);
    virtual void onChannelConnect();
    virtual void onChannelDisconnect();

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
    virtual void removeMirrorRecord(const std::string& mirrorChannelName);

    virtual void removeAllMirrorRecords();
    virtual bool hasMirrorRecord(const std::string& mirrorChannelName);
    virtual boost::python::list getMirrorRecordNames();

private:

    static PvaPyLogger logger;
    std::map<std::string, MirrorChannelPtr> mirrorChannelMap;
};

#endif

