// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "CaClient.h"
#include "pv/caProvider.h"

CaClient::ClientFactoryController CaClient::clientFactoryController;

CaClient::ClientFactoryController::ClientFactoryController() 
{
#if PVA_API_VERSION < 480
    epics::pvAccess::ca::CAClientFactory::start();
#endif // PVA_API_VERSION < 480
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


