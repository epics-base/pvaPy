
#include <iostream>
#include "RequesterImpl.h"


RequesterImpl::RequesterImpl(const epics::pvData::String& requesterName_) :
    requesterName(requesterName_)
{
}

epics::pvData::String RequesterImpl::getRequesterName()
{
	return "RequesterImpl";
}

void RequesterImpl::message(const epics::pvData::String& message, epics::pvData::MessageType messageType)
{
	std::cerr << "[" << getRequesterName() << "] message(" << message << ", " << getMessageTypeName(messageType) << ")" << std::endl;
}


