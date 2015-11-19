// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PVA_CLIENT_UTILITY_H
#define PVA_CLIENT_UTILITY_H

#include <string>
#include <vector>

#include "boost/python/list.hpp"
#include "pv/pvaClient.h"

#include "PvaClient.h"
#include "PvObject.h"
#include "PvProvider.h"
#include "PvaPyLogger.h"
#include "PyPvDataUtility.h"
#include "PvaConstants.h"

namespace PvaClientUtility
{

template<typename CppType, typename PvType, typename PvPtrType>
PvPtrType getField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

template<typename CppType, typename PvType, typename PvPtrType>
void put(epics::pvaClient::PvaClientChannelPtr pvaClientChannelPtr, CppType value, const std::string& requestDescriptor);

template<typename CppType, typename PvType, typename PvPtrType>
PvObject* putGet(epics::pvaClient::PvaClientChannelPtr pvaClientChannelPtr, CppType value, const std::string& requestDescriptor);

template<typename CppType, typename PvType, typename PvPtrType>
PvObject* getPut(epics::pvaClient::PvaClientChannelPtr pvaClientChannelPtr, CppType value, const std::string& requestDescriptor);

//
// Template implementations
//
template<typename PvType, typename PvPtrType>
PvPtrType getField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr)
{
    epics::pvData::PVFieldPtr pvFieldPtr = pvStructurePtr->getSubField(fieldName);
    if (!pvFieldPtr) {
        throw FieldNotFound("Object does not have field " + fieldName);
    }

    PvPtrType fieldPtr = pvStructurePtr->getSubField<PvType>(fieldName);
    if (!fieldPtr) {
        throw InvalidRequest("Invalid field type request for field " + fieldName);
    }
    return fieldPtr;
}


template<typename CppType, typename PvType, typename PvPtrType>
void put(epics::pvaClient::PvaClientChannelPtr pvaClientChannelPtr, CppType value, const std::string& requestDescriptor)
{
    try {
        epics::pvaClient::PvaClientPutPtr pvaPut = pvaClientChannelPtr->createPut(requestDescriptor);
        epics::pvaClient::PvaClientPutDataPtr pvaData = pvaPut->getData();
        epics::pvData::PVStructurePtr pvStructurePtr = pvaData->getPVStructure();
        PvPtrType pvValue = getField<PvType, PvPtrType>(PvaConstants::ValueFieldKey, pvStructurePtr);
        pvValue->put(value);
        pvaPut->put();
    } 
    catch (std::runtime_error e) {
        throw PvaException(e.what());
    }
}

template<typename CppType, typename PvType, typename PvPtrType>
PvObject* putGet(epics::pvaClient::PvaClientChannelPtr pvaClientChannelPtr, CppType value, const std::string& requestDescriptor)
{
    try {
        epics::pvaClient::PvaClientPutGetPtr pvaPutGet = pvaClientChannelPtr->createPutGet(requestDescriptor);
        epics::pvaClient::PvaClientPutDataPtr pvaData = pvaPutGet->getPutData();
        epics::pvData::PVStructurePtr pvStructurePtr = pvaData->getPVStructure();
        PvPtrType pvValue = getField<PvType, PvPtrType>(PvaConstants::ValueFieldKey, pvStructurePtr);
        pvValue->put(value);
        pvaPutGet->putGet();
        return new PvObject(pvaPutGet->getGetData()->getPVStructure());
    } 
    catch (std::runtime_error e) {
        throw PvaException(e.what());
    }
}

template<typename CppType, typename PvType, typename PvPtrType>
PvObject* getPut(epics::pvaClient::PvaClientChannelPtr pvaClientChannelPtr, CppType value, const std::string& requestDescriptor)
{
    try {
        epics::pvaClient::PvaClientPutGetPtr pvaPutGet = pvaClientChannelPtr->createPutGet(requestDescriptor);
        epics::pvaClient::PvaClientPutDataPtr pvaData = pvaPutGet->getPutData();
        epics::pvData::PVStructurePtr pvStructurePtr = pvaData->getPVStructure();
        PvPtrType pvValue = getField<PvType, PvPtrType>(PvaConstants::ValueFieldKey, pvStructurePtr);
        pvValue->put(value);
        pvaPutGet->getPut();
        return new PvObject(pvaPutGet->getGetData()->getPVStructure());
    } 
    catch (std::runtime_error e) {
        throw PvaException(e.what());
    }
}

} // namespace PvaClientUtility

#endif // PVA_CLIENT_UTILITY_H
