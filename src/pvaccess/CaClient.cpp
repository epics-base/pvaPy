// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "CaClient.h"
#include "pv/caProvider.h"

CaClient::ClientFactoryController CaClient::clientFactoryController;

CaClient::ClientFactoryController::ClientFactoryController() 
{
    epics::pvAccess::ca::CAClientFactory::start();
}

CaClient::ClientFactoryController::~ClientFactoryController() 
{
    // Stopping CA client factory is causing segfault at the moment.
    //epics::pvAccess::ca::CAClientFactory::stop();
}

CaClient::CaClient() 
{
}

CaClient::~CaClient()
{
}


