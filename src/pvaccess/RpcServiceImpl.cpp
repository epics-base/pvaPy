// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include <boost/python/extract.hpp>
#include "RpcServiceImpl.h"
#include "PvObject.h"
#include "PyGilManager.h"
#include "PyUtility.h"

namespace bp = boost::python; 

PvaPyLogger RpcServiceImpl::logger("Channel");

RpcServiceImpl::RpcServiceImpl(const boost::python::object& pyService_) : 
    pyService(pyService_),
    pyObject()
{
    PvObject::initializeBoostNumPy();
}

RpcServiceImpl::~RpcServiceImpl()
{
    // This prevents response object destruction that is causing segfault
    // with older versions of v4 libraries.
    //bp::incref(pyObject.ptr());
}

epics::pvData::PVStructurePtr RpcServiceImpl::request(const epics::pvData::PVStructurePtr& args)
{
    PvObject pyRequest(args);
    // Acquire GIL
    PyGilManager::gilStateEnsure();

    // Call python code
    try {
        pyObject = pyService(pyRequest);
    }
    catch(bp::error_already_set& ex) {
        std::string errorMessage = PyUtility::getErrorMessageFromTraceback(ex);
        throw epics::pvAccess::RPCRequestException(epics::pvData::Status::STATUSTYPE_ERROR, errorMessage);
    }
    catch (const std::exception& ex) {
        throw epics::pvAccess::RPCRequestException(epics::pvData::Status::STATUSTYPE_ERROR, ex.what());
        logger.error(ex.what());
    }

    // Release GIL. 
    PyGilManager::gilStateRelease();

    boost::python::extract<PvObject> pvObjectExtract(pyObject);
    if (!pvObjectExtract.check()) {
        throw epics::pvAccess::RPCRequestException(epics::pvData::Status::STATUSTYPE_ERROR, "Callable python service object must return instance of PvObject.");
    }
    PvObject pyResponse = pvObjectExtract();
    return static_cast<epics::pvData::PVStructurePtr>(pyResponse);
}

