#ifndef REQUESTER_IMPL_H
#define REQUESTER_IMPL_H

#include <pv/pvData.h>
#include <pv/pvAccess.h>

#include <string>

class RequesterImpl : public epics::pvData::Requester
{
public:
    RequesterImpl(const epics::pvData::String& requesterName);
    virtual epics::pvData::String getRequesterName();
    virtual void message(const epics::pvData::String& message, epics::pvData::MessageType messageType);

private:
    epics::pvData::String requesterName;
};

#endif
