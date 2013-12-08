#ifndef INVALID_DATA_TYPE_H
#define INVALID_DATA_TYPE_H

#include <string>
#include "PvaException.h"

/**
 * Invalid request error.
 */
class InvalidDataType : public PvaException
{
public:
    InvalidDataType(const std::string& message=""); 
    InvalidDataType(const char* message, ...);
};

#endif 
