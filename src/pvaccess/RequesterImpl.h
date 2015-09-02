// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef REQUESTER_IMPL_H
#define REQUESTER_IMPL_H

#include <pv/pvData.h>
#include <pv/pvAccess.h>

#include <string>

class RequesterImpl : public epics::pvData::Requester
{
public:
    RequesterImpl(const std::string& requesterName);
    virtual std::string getRequesterName();
    virtual void message(const std::string& message, epics::pvData::MessageType messageType);

private:
    std::string requesterName;
};

#endif
