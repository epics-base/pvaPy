// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution


#include "boost/python.hpp"
#include <iostream>

#include "MultiChannel.h"
#include "PyUtility.h"

PvaPyLogger MultiChannel::logger("MultiChannel");
PvaClient MultiChannel::pvaClient;
CaClient MultiChannel::caClient;
epics::pvaClient::PvaClientPtr MultiChannel::pvaClientPtr(epics::pvaClient::PvaClient::get("pva ca"));


MultiChannel::MultiChannel(const boost::python::list& channelNames, PvProvider::ProviderType providerType_) 
{
    int listSize = boost::python::len(channelNames);
    epics::pvData::shared_vector<std::string> names(listSize);
    for (int i = 0; i < listSize; i++) {
        names[i] = PyUtility::extractStringFromPyObject(channelNames[i]);
    }
    epics::pvData::shared_vector<const std::string> names2(freeze(names));
    pvaClientMultiChannelPtr = epics::pvaClient::PvaClientMultiChannel::create(pvaClientPtr, names2, PvProvider::getProviderName(providerType_));
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
        epics::pvaClient::PvaClientNTMultiGetPtr mGet(pvaClientMultiChannelPtr->createNTGet());
        mGet->get();
        epics::pvaClient::PvaClientNTMultiDataPtr mData = mGet->getData();
        epics::pvData::PVStructurePtr pvStructure = mData->getNTMultiChannel()->getPVStructure();
        return new PvObject(pvStructure);
    }
    catch (std::runtime_error& ex) {
        throw PvaException(ex.what());
    }
}
 
