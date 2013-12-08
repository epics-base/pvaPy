#include "ChannelPutRequesterImpl.h"

PvaClient ChannelPutRequesterImpl::pvaClient;

ChannelPutRequesterImpl::ChannelPutRequesterImpl(const epics::pvData::String& channelName_) 
    :  epics::pvAccess::ChannelPutRequester(),
    channelName(channelName_),
    done(false)
{
}

ChannelPutRequesterImpl::ChannelPutRequesterImpl(const ChannelPutRequesterImpl& channelPutRequester)
    :  epics::pvAccess::ChannelPutRequester(),
    channelName(channelPutRequester.channelName),
    done(false)
{
}
    
epics::pvData::String ChannelPutRequesterImpl::getRequesterName()
{
    return "ChannelPutRequesterImpl";
}

void ChannelPutRequesterImpl::message(const epics::pvData::String& message, epics::pvData::MessageType messageType)
{
    std::cerr << "[" << getRequesterName() << "] message(" << message << ", " << getMessageTypeName(messageType) << ")" << std::endl;
}

void ChannelPutRequesterImpl::channelPutConnect(const epics::pvData::Status& status,
    const epics::pvAccess::ChannelPut::shared_pointer& channelPut,
    const epics::pvData::PVStructure::shared_pointer& pvStructure, 
    const epics::pvData::BitSet::shared_pointer& bitSet)
{
    if (status.isSuccess()) {
        // show warning
        if (!status.isOK()) {
            std::cerr << "[" << channelName << "] channel put create: " << status.getMessage() << std::endl;
        }

        // assign smart pointers
        {
            epics::pvData::Lock lock(pointerMutex);
            this->channelPut = channelPut;
            this->pvStructure = pvStructure;
            this->bitSet = bitSet;
        }
            
        // we always put all
        this->bitSet->set(0);
            
        // get immediately old value
        this->channelPut->get();
    }
    else {
        std::cerr << "[" << channelName << "] failed to create channel put: " << status.getMessage() << std::endl;
        event.signal();
    }
}

void ChannelPutRequesterImpl::getDone(const epics::pvData::Status& status)
{
    if (status.isSuccess()) {
        // show warning
        if (!status.isOK()) {
            std::cerr << "[" << channelName << "] channel get: " << status.getMessage() << std::endl;
        }
        done = true;
    }
    else {
        std::cerr << "[" << channelName << "] failed to get: " << status.getMessage() << std::endl;
    }
    event.signal();
}

void ChannelPutRequesterImpl::putDone(const epics::pvData::Status& status)
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
        event.signal();
    }

epics::pvData::PVStructure::shared_pointer ChannelPutRequesterImpl::getStructure()
{
    epics::pvData::Lock lock(pointerMutex);
    return pvStructure;
}

bool ChannelPutRequesterImpl::waitUntilDone(double timeOut)
{
    bool signaled = event.wait(timeOut);
    if (!signaled) {
        std::cerr << "[" << channelName << "] timeout" << std::endl;
        return false;
    }

    epics::pvData::Lock lock(pointerMutex);
    return done;
}

