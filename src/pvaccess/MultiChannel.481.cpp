// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution


#include "boost/python.hpp"
#include <iostream>

#include "MultiChannel.h"
#include "PyUtility.h"
#include "PyPvDataUtility.h"

PvaPyLogger MultiChannel::logger("MultiChannel");
PvaClient MultiChannel::pvaClient;
CaClient MultiChannel::caClient;
epics::pvaClient::PvaClientPtr MultiChannel::pvaClientPtr(epics::pvaClient::PvaClient::get("pva ca"));

namespace bp = boost::python;
namespace epvd = epics::pvData;
namespace epvac = epics::pvaClient;

MultiChannel::MultiChannel(const bp::list& channelNames, PvProvider::ProviderType providerType_) 
{
    unsigned int listSize = bp::len(channelNames);
    epvd::shared_vector<std::string> names(listSize);
    for (unsigned int i = 0; i < listSize; i++) {
        names[i] = PyUtility::extractStringFromPyObject(channelNames[i]);
    }
    epvd::shared_vector<const std::string> names2(freeze(names));
    pvaClientMultiChannelPtr = epvac::PvaClientMultiChannel::create(pvaClientPtr, names2, PvProvider::getProviderName(providerType_));
}
    
MultiChannel::MultiChannel(const MultiChannel& mc) 
{
}

MultiChannel::~MultiChannel()
{
}

PvObject* MultiChannel::get()
{
    pvaClientMultiChannelPtr->connect();
    try {
        epvac::PvaClientNTMultiGetPtr mGet(pvaClientMultiChannelPtr->createNTGet());
        mGet->get();
        epvac::PvaClientNTMultiDataPtr mData = mGet->getData();
        epvd::PVStructurePtr pvStructure = mData->getNTMultiChannel()->getPVStructure();
        return new PvObject(pvStructure);
    }
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
}

void MultiChannel::put(const bp::list& pyList)
{
    epvac::PvaClientNTMultiPutPtr mPut(pvaClientMultiChannelPtr->createNTPut());
    epvd::shared_vector<epvd::PVUnionPtr> data = mPut->getValues();
    unsigned int listSize = bp::len(pyList);
    for (unsigned int i = 0; i < data.size(); i++) {
        if (i > listSize) {
            break;
        }
        bp::object pyObject = pyList[i];
        bp::extract<PvObject> extractPvObject(pyObject);
        if (extractPvObject.check()) {
            PvObject pvObject = extractPvObject();
            std::string keyFrom = PyPvDataUtility::getValueOrSingleFieldName(pvObject.getPvStructurePtr());
            epvd::PVFieldPtr pvFrom = PyPvDataUtility::getSubField(keyFrom, pvObject.getPvStructurePtr());
            PyPvDataUtility::setUnionField(pvFrom, data[i]);
        }
    }
    mPut->put();
}

 
