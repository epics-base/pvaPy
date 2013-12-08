#ifndef FIELD_NOT_FOUND_H
#define FIELD_NOT_FOUND_H

#include <string>
#include "PvaException.h"

/**
 * Field not found error.
 */
class FieldNotFound : public PvaException
{
public:
    FieldNotFound(const std::string& message=""); 
    FieldNotFound(const char* message, ...);
};

#endif 
