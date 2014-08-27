#include "ChannelRequesterImpl.h"

#include <iostream>
#include <string>

ChannelRequesterImpl::ChannelRequesterImpl(bool printOnlyErrors_) :
    printOnlyErrors(printOnlyErrors_)
{
}

std::string ChannelRequesterImpl::getRequesterName()
{
	return "ChannelRequesterImpl";
}

void ChannelRequesterImpl::message(const std::string& message, epics::pvData::MessageType messageType)
{
    if (!printOnlyErrors || messageType > epics::pvData::warningMessage) {
        std::cerr << "[" << getRequesterName() << "] message(" << message << ", " << getMessageTypeName(messageType) << ")" << std::endl;
    }
}

void ChannelRequesterImpl::channelCreated(const epics::pvData::Status& status, const epics::pvAccess::Channel::shared_pointer& channel)
{
	if (status.isSuccess()) {
		// show warning
		if (!status.isOK()) {
            std::cerr << "[" << channel->getChannelName() << "] channel create: " << status.getMessage() << std::endl;
		}
	}
	else {
        std::cerr << "[" << channel->getChannelName() << "] failed to create a channel: " << status.getMessage() << std::endl;
	}
}

void ChannelRequesterImpl::channelStateChange(const epics::pvAccess::Channel::shared_pointer& /*channel*/, epics::pvAccess::Channel::ConnectionState connectionState)
{
	if (connectionState == epics::pvAccess::Channel::CONNECTED) {
		event.signal();
	}
}
    
bool ChannelRequesterImpl::waitUntilConnected(double timeOut)
{
	return event.wait(timeOut);
}


