#include "GetFieldRequesterImpl.h"

#include <iostream>
#include <string>

GetFieldRequesterImpl::GetFieldRequesterImpl(epics::pvAccess::Channel::shared_pointer channel_) :
    channel(channel_)
{
}

std::string GetFieldRequesterImpl::getRequesterName()
{
	return "GetFieldRequesterImpl";
}

void GetFieldRequesterImpl::message(const std::string& message, epics::pvData::MessageType messageType)
{
	std::cerr << "[" << getRequesterName() << "] message(" << message << ", " << getMessageTypeName(messageType) << ")" << std::endl;
}

void GetFieldRequesterImpl::getDone(const epics::pvData::Status& status, const epics::pvData::FieldConstPtr& field)
{
	if (status.isSuccess()) {
		// show warning
		if (!status.isOK()) {
            std::cerr << "[" << channel->getChannelName() << "] getField create: " << status.getMessage() << std::endl;
		}

		// assign smart pointers
		{
		    epics::pvData::Lock lock(pointerMutex);
		    this->field = field;
		}
	}
	else {
		// do not complain about missing field
        //std::cerr << "[" << channel->getChannelName() << "] failed to get channel introspection data: " << status << std::endl;
	}
	event.signal();
}

bool GetFieldRequesterImpl::waitUntilFieldGet(double timeOut)
{
	return event.wait(timeOut);
}

epics::pvData::FieldConstPtr GetFieldRequesterImpl::getField()
{
    epics::pvData::Lock lock(pointerMutex);
    return field;
}


