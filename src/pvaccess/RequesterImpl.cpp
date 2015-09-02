// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

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


