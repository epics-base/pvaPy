#ifndef PVA_EXCEPTION_TRANSLATOR_H
#define PVA_EXCEPTION_TRANSLATOR_H

#include "PvaException.h"

class PvaExceptionTranslator 
{
public:
    static void translator(const PvaException& ex);
};

#endif

