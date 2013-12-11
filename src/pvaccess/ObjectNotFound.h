#ifndef OBJECT_NOT_FOUND_H
#define OBJECT_NOT_FOUND_H

#include <string>
#include "PvaException.h"

/**
 * Object not found error.
 */
class ObjectNotFound : public PvaException
{
public:
    ObjectNotFound(const std::string& message=""); 
    ObjectNotFound(const char* message, ...);
};

#endif 
