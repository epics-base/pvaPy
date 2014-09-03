#include "pv/event.h"
#include "pv/rpcClient.h"

int main()
{
    epics::pvData::PVStructure::shared_pointer pvRequest;
    epics::pvAccess::RPCClient::shared_pointer rpcClient = epics::pvAccess::RPCClient::create("Channel", pvRequest);
    return 0;
}

