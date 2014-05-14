#ifndef RPC_SERVER_CONTEXT_IMPL_H
#define RPC_SERVER_CONTEXT_IMPL_H

#include "pv/serverContext.h"

class RpcServerContextImpl : public epics::pvAccess::ServerContextImpl
{
public:
    typedef std::tr1::shared_ptr<RpcServerContextImpl> shared_pointer;
    typedef std::tr1::shared_ptr<const RpcServerContextImpl> const_shared_pointer;
	RpcServerContextImpl();
	virtual ~RpcServerContextImpl();

private:
};


#endif // RPC_SERVER_CONTEXT_IMPL_H
