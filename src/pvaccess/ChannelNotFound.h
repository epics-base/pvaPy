#ifndef CHANNEL_NOT_FOUND_H
#define CHANNEL_NOT_FOUND_H

#include <string>
#include "ChannelAccessError.h"

/**
 * Channel not found error.
 */
class ChannelNotFound : public ChannelAccessError
{
public:
    ChannelNotFound(const std::string& message=""); 
    ChannelNotFound(const char* message, ...);
};

#endif 
