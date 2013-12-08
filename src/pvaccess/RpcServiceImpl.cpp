
#include "boost/python/extract.hpp"
#include "RpcServiceImpl.h"
#include "PvObject.h"

RpcServiceImpl::RpcServiceImpl(const boost::python::object& pyService_)
    : pyService(pyService_)
{
}

RpcServiceImpl::~RpcServiceImpl()
{
}

epics::pvData::PVStructurePtr RpcServiceImpl::request(const epics::pvData::PVStructurePtr& args)
    throw (epics::pvAccess::RPCRequestException)
{
    PvObject pyRequest(args);
    boost::python::object pyObject = pyService(pyRequest);
    boost::python::extract<PvObject> pvObjectExtract(pyObject);
    if (!pvObjectExtract.check()) {
        throw epics::pvAccess::RPCRequestException(epics::pvData::Status::STATUSTYPE_ERROR, "Callable python service object must return instance of PvObject.");
    }
    PvObject pyResponse = pvObjectExtract();
    return static_cast<epics::pvData::PVStructurePtr>(pyResponse);
}

