// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python.hpp"
#include "ChannelMonitorRequesterImpl.h"

using namespace epics::pvaClient;
namespace pvc = epics::pvaClient;
namespace pvd = epics::pvData;

PvaPyLogger ChannelMonitorRequesterImpl::logger("ChannelMonitorRequesterImpl");

ChannelMonitorRequesterImpl::ChannelMonitorRequesterImpl(const std::string& channelName_, ChannelMonitorDataProcessor* processor_)
    : channelName(channelName_)
    , processor(processor_)
    , isActive(true)
    , nReceived(0)
    , nOverruns(0)
{
}

ChannelMonitorRequesterImpl::ChannelMonitorRequesterImpl(const ChannelMonitorRequesterImpl& channelMonitor)
    : channelName(channelMonitor.channelName)
    , processor(channelMonitor.processor)
    , isActive(true)
    , nReceived(channelMonitor.nReceived)
    , nOverruns(channelMonitor.nOverruns)
{
}

ChannelMonitorRequesterImpl::~ChannelMonitorRequesterImpl()
{
}

void ChannelMonitorRequesterImpl::event(const PvaClientMonitorPtr& monitor)
{
    try {
        while (isActive) {
            if (!monitor->poll()) {;
                break;
            }
            if (isActive) {
                nReceived++;
                pvc::PvaClientMonitorDataPtr pvaData = monitor->getData();
                pvd::BitSetPtr overrunBitSet = pvaData->getOverrunBitSet();
                if (!overrunBitSet->isEmpty()) {
                    nOverruns++;
                    processor->onMonitorOverrun(overrunBitSet);
                }
                processor->processMonitorData(pvaData->getPVStructure()); 
            }
            monitor->releaseEvent();
        }
    }
    catch (std::runtime_error& ex) {
        logger.warn(ex.what());
    }
}

void ChannelMonitorRequesterImpl::unlisten()
{
    isActive = false;
}

