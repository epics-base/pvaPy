// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

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
    virtual const char* getPyExceptionClassName() const;
};

inline const char* ChannelNotFound::getPyExceptionClassName() const
{
        return ChannelNotFound::PyExceptionClassName;
}

#endif 
