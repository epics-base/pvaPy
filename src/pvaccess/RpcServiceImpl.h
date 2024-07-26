// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef RPC_SERVICE_IMPL_H
#define RPC_SERVICE_IMPL_H

#include <pv/pvData.h>
#include <pv/pvAccess.h>
#include <pv/rpcServer.h>
#include <boost/python/object.hpp>
#include "PvaPyLogger.h"
#include "PvObject.h"

class RpcServiceImpl : public epics::pvAccess::RPCService
{
public:
    POINTER_DEFINITIONS(RpcServiceImpl);
    RpcServiceImpl(const boost::python::object& pyService);
    virtual ~RpcServiceImpl();
    virtual epics::pvData::PVStructurePtr request(const epics::pvData::PVStructurePtr& args);
private:
    static PvaPyLogger logger;
    boost::python::object pyService;
    // Keeping python response object in scope prevents problem with
    // service segfaulting while returning RPC result; this issue seems to have
    // appeared in recent boost versions 
    boost::python::object pyObject;
};

#endif
