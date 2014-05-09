#include <iostream>
#include "PvObject.h"
#include "PvaException.h"
#include "Channel.h"

int main(int argc, char** argv)
{
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " <channelName>" << std::endl;
        return 1;
    }
    try {
        Channel channel(argv[1]);
        std::cout << *(channel.get()) << std::endl;
    }
    catch (PvaException& ex) {
        std::cerr << ex.what() << std::endl;
    }

    #include "pv/pvAccess.h"
    #include "pv/pvData.h"
                                        #include "pv/event.h"
                                                    #include "pv/rpcClient.h"
                                                                class RequesterImpl : public epics::pvData::Requester
                                                                            {
                                                                                            public:
                                                                                                            RequesterImpl(const epics::pvData::String& requesterName) {}
                                                                                                                            virtual epics::pvData::String getRequesterName() { return "requester"; }
                                                                                                                                            virtual void message(const epics::pvData::String& message, epics::pvData::MessageType messageType) {}
                                                                                                                                                        };
                                                                                                                                                                    ]],
                                                                                                                                                                                [[
                                                                                                                                                                                            epics::pvData::Requester::shared_pointer requester(new RequesterImpl("Channel"));
                                                                                                                                                                                                        epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvAccess::getCreateRequest()->createRequest("field(value)", requester);
                                                                                                                                                                                                                    epics::pvAccess::RPCClient::create("Channel", pvRequest);
                                                                                                                                                                                                                                ]])
    return 0;
}
