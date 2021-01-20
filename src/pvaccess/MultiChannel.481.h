// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef MULTI_CHANNEL_H
#define MULTI_CHANNEL_H

#include <string>
#include <vector>
#include <map>

#include "boost/python/list.hpp"
#include "boost/python/dict.hpp"

#include "pv/pvaClient.h"
#include "pv/pvaClientMultiChannel.h"

#include "PvaClient.h"
#include "CaClient.h"
#include "PvObject.h"
#include "PvProvider.h"
#include "PvaPyLogger.h"
#include "PvaConstants.h"

class MultiChannel 
{
public:
    MultiChannel(const boost::python::list& channelNames, PvProvider::ProviderType providerType=PvProvider::PvaProviderType);
    MultiChannel(const MultiChannel& multiChannel);
    virtual ~MultiChannel();

    virtual PvObject* get();
    virtual void put(const boost::python::list& pyList);

private:
    static PvaPyLogger logger;
    static PvaClient pvaClient;
    static CaClient caClient;
    static epics::pvaClient::PvaClientPtr pvaClientPtr;

    epics::pvaClient::PvaClientMultiChannelPtr pvaClientMultiChannelPtr;
    
};

#endif
