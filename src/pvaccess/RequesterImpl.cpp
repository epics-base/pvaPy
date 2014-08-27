
#include <iostream>
#include "RequesterImpl.h"


RequesterImpl::RequesterImpl(const std::string& requesterName_) :
    requesterName(requesterName_)
{
}

std::string RequesterImpl::getRequesterName()
{
	return "RequesterImpl";
}

void RequesterImpl::message(const std::string& message, epics::pvData::MessageType messageType)
{
	std::cerr << "[" << getRequesterName() << "] message(" << message << ", " << getMessageTypeName(messageType) << ")" << std::endl;
}


