#ifndef INVALID_REQUEST_H
#define INVALID_REQUEST_H

#include <string>
#include "PvaException.h"

/**
 * Invalid request error.
 */
class InvalidRequest : public PvaException
{
public:
    InvalidRequest(const std::string& message=""); 
    InvalidRequest(const char* message, ...);
};

#endif 
