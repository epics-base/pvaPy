// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "ChannelMonitorRequesterImpl.h"

using namespace epics::pvaClient;

PvaPyLogger ChannelMonitorRequesterImpl::logger("ChannelMonitorRequesterImpl");

ChannelMonitorRequesterImpl::ChannelMonitorRequesterImpl(const std::string& channelName_, ChannelMonitorDataProcessor* processor_) : 
    channelName(channelName_),
    processor(processor_),
    isActive(true)
{
}

ChannelMonitorRequesterImpl::ChannelMonitorRequesterImpl(const ChannelMonitorRequesterImpl& channelMonitor) : 
    channelName(channelMonitor.channelName),
    processor(channelMonitor.processor),
    isActive(true)
{
}

ChannelMonitorRequesterImpl::~ChannelMonitorRequesterImpl()
{
}

void ChannelMonitorRequesterImpl::event(const PvaClientMonitorPtr& monitor)
{
    epics::pvaClient::PvaClientMonitorDataPtr pvaData = monitor->getData();
    try {
        while (isActive) {
            if (!monitor->poll()) {;
                break;
            }
            if (isActive) {
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


