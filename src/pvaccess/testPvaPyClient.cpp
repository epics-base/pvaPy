// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

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
}
