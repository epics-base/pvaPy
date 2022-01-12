// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PVA_GW_SERVER_H
#define PVA_GW_SERVER_H

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

class PvaGwServer;
class GwChannelDataProcessor;
typedef std::tr1::shared_ptr<GwChannelDataProcessor> GwChannelDataProcessorPtr;
class GwChannel;
typedef std::tr1::shared_ptr<GwChannel> GwChannelPtr;
 
class GwChannelDataProcessor : public ChannelMonitorDataProcessor
{
public:
    GwChannelDataProcessor(PvaGwServer* pvaGwServer, const std::string& gwChannelName);
    virtual ~GwChannelDataProcessor();

    virtual void processMonitorData(epics::pvData::PVStructurePtr pvStructurePtr);
    virtual void onChannelConnect();
    virtual void onChannelDisconnect();

private:
    static PvaPyLogger logger;
    PvaGwServer *pvaGwServer;
    std::string gwChannelName;
    epics::pvData::Mutex mutex;
    bool recordAdded;
};

 
// The purpose of this class is to simply establish channel monitor to 
// provide updates to the PVA server record

class GwChannel : public ChannelMonitorDataProcessor
{
public:

    GwChannel(const std::string& channelName, PvProvider::ProviderType providerType, GwChannelDataProcessorPtr dataProcessorPtr);
    GwChannel(const GwChannel& gwChannel);
    virtual ~GwChannel();

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
    GwChannelDataProcessorPtr dataProcessorPtr;

    bool isConnected;
    bool hasIssuedConnect;
    bool monitorActive;
    epics::pvaClient::PvaClientChannelStateChangeRequesterPtr stateRequester;
};

class PvaGwServer : public PvaServer
{
public:
    PvaGwServer();
    PvaGwServer(const PvaGwServer&);
    virtual ~PvaGwServer();

    virtual void addGwRecord(const std::string& gwChannelName, const std::string& srcChannelName, PvProvider::ProviderType srcProviderType);
    virtual void removeGwRecord(const std::string& gwChannelName);

private:

    static PvaPyLogger logger;
    std::map<std::string, GwChannelPtr> gwChannelMap;
};

#endif

