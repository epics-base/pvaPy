#ifndef INVALID_STATE_H
#define INVALID_STATE_H

#include <string>
#include "PvaException.h"

/**
 * Invalid state error.
 */
class InvalidState : public PvaException
{
public:
    InvalidState(const std::string& message=""); 
    InvalidState(const char* message, ...);
};

#endif 
