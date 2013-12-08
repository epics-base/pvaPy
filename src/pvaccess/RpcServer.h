#ifndef RPC_SERVER_H
#define RPC_SERVER_H

#include <string>
#include "pv/pvData.h"
#include "pv/pvAccess.h"
#include "pv/rpcServer.h"
#include "boost/python/object.hpp"
#include "RpcServiceImpl.h"


class RpcServer : public epics::pvAccess::RPCServer
{
public:
    RpcServer();
    virtual ~RpcServer();
    void registerService(const std::string& serviceName, const boost::python::object& pyService);
    void unregisterService(const std::string& serviceName);
    void listen(int seconds=0);
};

#endif
