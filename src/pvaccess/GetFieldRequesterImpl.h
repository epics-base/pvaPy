#ifndef GET_FIELD_REQUESTER_IMPL_H
#define GET_FIELD_REQUESTER_IMPL_H

#include <pv/event.h>
#include <pv/pvData.h>
#include <pv/pvAccess.h>

#include <string>

class GetFieldRequesterImpl : public epics::pvAccess::GetFieldRequester
{
private:
	epics::pvAccess::Channel::shared_pointer channel;
	epics::pvData::FieldConstPtr field;
    epics::pvData::Event event;
    epics::pvData::Mutex pointerMutex;

public:

    GetFieldRequesterImpl(epics::pvAccess::Channel::shared_pointer channel);

    virtual epics::pvData::String getRequesterName();
    virtual void message(const epics::pvData::String& message, epics::pvData::MessageType messageType);
    virtual void getDone(const epics::pvData::Status& status, const epics::pvData::FieldConstPtr& field);
    epics::pvData::FieldConstPtr getField();
    bool waitUntilFieldGet(double timeOut);
};

#endif
