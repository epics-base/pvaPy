// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include <sys/time.h>
#include <iostream>
#include <unistd.h>

#include "pv/pvaClient.h"

class ChannelStateRequesterImpl : public epics::pvaClient::PvaClientChannelStateChangeRequester
{
public:
    POINTER_DEFINITIONS(ChannelStateRequesterImpl);

    ChannelStateRequesterImpl(bool& connected_) : connected(connected_) {}
    virtual ~ChannelStateRequesterImpl() {}

    bool isConnected() const {return connected;}

    // PvaClientChannelStateChangeRequester interface
    virtual void channelStateChange(const epics::pvaClient::PvaClientChannelPtr& channel, bool isConnected) {connected=isConnected;}

private:
    bool& connected;
};

class ChannelMonitorRequesterImpl : public epics::pvaClient::PvaClientMonitorRequester
{
public:
    POINTER_DEFINITIONS(ChannelMonitorRequesterImpl);
    ChannelMonitorRequesterImpl(const std::string& channelName_) :
        channelName(channelName_),
        isActive(true) {}
    ChannelMonitorRequesterImpl(const ChannelMonitorRequesterImpl& channelMonitor) :
        channelName(channelMonitor.channelName),
        isActive(true) {}

    virtual ~ChannelMonitorRequesterImpl() {}
    virtual void event(const epics::pvaClient::PvaClientMonitorPtr& monitor)
    {
        if (nReceived == 0) {
            gettimeofday(&startTime, NULL);
        }
        try {
            while (isActive) {
                if (!monitor->poll()) {;
                    break;
                }
                nReceived++;
                if (nReceived % 100000 == 0) {
                    struct timeval currentTime;
                    gettimeofday(&currentTime, NULL);
                    double deltaT = (currentTime.tv_sec - startTime.tv_sec) + (currentTime.tv_usec - startTime.tv_usec)/1000000.0;
                    double receiveRate = nReceived/deltaT/1000.0;
                    printf("%s: Received: %lu (%.2f [kHz]); Runtime: %.2f [s]\n", channelName.c_str(), nReceived, receiveRate, deltaT);
                }
                monitor->releaseEvent();
            }
        }
        catch (std::runtime_error& ex) {
            std::cerr << ex.what() << std::endl;
        }
    }


    virtual void unlisten() {isActive=false;}

private:
    std::string channelName;
    bool isActive;
    unsigned long nReceived;
    struct timeval startTime;
};


int main(int argc, char** argv)
{
    int runtime = 60;
    if(argc >= 2) {
        runtime = atoi(argv[1]);
    }

    epics::pvaClient::PvaClientPtr pvaClientPtr(epics::pvaClient::PvaClient::get());

    std::string channelName = "X1";
    epics::pvaClient::PvaClientChannelPtr pvaClientChannelPtr(pvaClientPtr->createChannel(channelName, "pva"));
    bool isConnected = false;
    epics::pvaClient::PvaClientChannelStateChangeRequesterPtr stateRequester(epics::pvaClient::PvaClientChannelStateChangeRequesterPtr(new ChannelStateRequesterImpl(isConnected)));
    pvaClientChannelPtr->setStateChangeRequester(stateRequester);

    epics::pvaClient::PvaClientMonitorRequesterPtr monitorRequester(new ChannelMonitorRequesterImpl(channelName));
    epics::pvaClient::PvaClientMonitorPtr monitor = pvaClientChannelPtr->monitor("field(value)", monitorRequester);
    std::cout << "STARTED MONITOR for " << channelName << std::endl;
    sleep(runtime);
    monitor->stop();
    std::cout << "STOPPED MONITOR for " << channelName << std::endl;
    sleep(1);
}


