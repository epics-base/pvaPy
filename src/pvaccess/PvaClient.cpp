// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvaClient.h"
#include "pv/clientFactory.h"

PvaClient::ClientFactoryController PvaClient::clientFactoryController;

PvaClient::ClientFactoryController::ClientFactoryController() 
{
    epics::pvAccess::ClientFactory::start();
}

PvaClient::ClientFactoryController::~ClientFactoryController() 
{
    epics::pvAccess::ClientFactory::stop();
}

PvaClient::PvaClient() 
{
}

PvaClient::~PvaClient()
{
}


