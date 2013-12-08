#ifndef RPC_SERVICE_IMPL_H
#define RPC_SERVICE_IMPL_H

#include "pv/pvData.h"
#include "pv/pvAccess.h"
#include "pv/rpcServer.h"
#include "boost/python/object.hpp"

class RpcServiceImpl : public epics::pvAccess::RPCService
{
public:
    POINTER_DEFINITIONS(RpcServiceImpl);
    RpcServiceImpl(const boost::python::object& pyService);
    virtual ~RpcServiceImpl();
    epics::pvData::PVStructurePtr request(const epics::pvData::PVStructurePtr& args)
        throw (epics::pvAccess::RPCRequestException);
private:
    boost::python::object pyService;
};

#endif
