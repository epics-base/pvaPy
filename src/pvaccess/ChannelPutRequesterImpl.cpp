// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "ChannelPutRequesterImpl.h"

PvaClient ChannelPutRequesterImpl::pvaClient;

ChannelPutRequesterImpl::ChannelPutRequesterImpl(const std::string& channelName_) 
    :  epics::pvAccess::ChannelPutRequester(),
    event(new epics::pvData::Event()),
    channelName(channelName_),
    done(false)
{
}

ChannelPutRequesterImpl::ChannelPutRequesterImpl(const ChannelPutRequesterImpl& channelPutRequester)
    :  epics::pvAccess::ChannelPutRequester(),
    event(new epics::pvData::Event()),
    channelName(channelPutRequester.channelName),
    done(false)
{
}
    
std::string ChannelPutRequesterImpl::getRequesterName()
{
    return "ChannelPutRequesterImpl";
}

void ChannelPutRequesterImpl::message(const std::string& message, epics::pvData::MessageType messageType)
{
    std::cerr << "[" << getRequesterName() << "] message(" << message << ", " << getMessageTypeName(messageType) << ")" << std::endl;
}

void ChannelPutRequesterImpl::channelPutConnect(const epics::pvData::Status& status,
    const epics::pvAccess::ChannelPut::shared_pointer& channelPut,
    const epics::pvData::Structure::const_shared_pointer& structure)
{
    if (status.isSuccess()) {
        // show warning
        if (!status.isOK()) {
            std::cerr << "[" << channelName << "] channel put create: " << status.getMessage() << std::endl;
        }

        // get immediately old value
        channelPut->get();
    }
    else {
        std::cerr << "[" << channelName << "] failed to create channel put: " << status.getMessage() << std::endl;
        event->signal();
    }
}

void ChannelPutRequesterImpl::getDone(const epics::pvData::Status& status, const epics::pvAccess::ChannelPut::shared_pointer& channelPut, const epics::pvData::PVStructure::shared_pointer& pvStructure, const epics::pvData::BitSet::shared_pointer& bitSet)
{
    if (status.isSuccess()) {
        // show warning
        if (!status.isOK()) {
            std::cerr << "[" << channelName << "] channel get: " << status.getMessage() << std::endl;
        }
        done = true;

        {
            epics::pvData::Lock lock(pointerMutex);
            this->channelPut = channelPut;
            this->pvStructure = pvStructure;
            this->bitSet = bitSet;
        }
            
    }
    else {
        std::cerr << "[" << channelName << "] failed to get: " << status.getMessage() << std::endl;
    }
    event->signal();
}

void ChannelPutRequesterImpl::putDone(const epics::pvData::Status& status, const epics::pvAccess::ChannelPut::shared_pointer& channelPut)
{
    if (status.isSuccess()) {
        // show warning
        if (!status.isOK()) {
            std::cerr << "[" << channelName << "] channel put: " << status.getMessage() << std::endl;
        }
        done = true;
    }
    else {
        std::cerr << "[" << channelName << "] failed to put: " << status.getMessage() << std::endl;
    }
    event->signal();
}

epics::pvData::PVStructure::shared_pointer ChannelPutRequesterImpl::getStructure()
{
    epics::pvData::Lock lock(pointerMutex);
    return pvStructure;
}

bool ChannelPutRequesterImpl::waitUntilDone(double timeOut)
{
    bool signaled = event->wait(timeOut);
    if (!signaled) {
        std::cerr << "[" << channelName << "] timeout" << std::endl;
        return false;
    }

    epics::pvData::Lock lock(pointerMutex);
    return done;
}

void ChannelPutRequesterImpl::resetEvent()
{
    epics::pvData::Lock lock(eventMutex);
    event.reset(new epics::pvData::Event());
    done = false;
}

epics::pvData::BitSet::shared_pointer ChannelPutRequesterImpl::getBitSet()
{
    epics::pvData::Lock lock(pointerMutex);
    return bitSet;
}

