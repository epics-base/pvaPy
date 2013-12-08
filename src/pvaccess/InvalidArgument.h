#ifndef INVALID_ARGUMENT_H
#define INVALID_ARGUMENT_H

#include <string>
#include "PvaException.h"

/**
 * Invalid argument error.
 */
class InvalidArgument : public PvaException
{
public:
    InvalidArgument(const std::string& message=""); 
    InvalidArgument(const char* message, ...);
};

#endif 
