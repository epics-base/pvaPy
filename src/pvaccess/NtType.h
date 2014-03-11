#ifndef NT_TYPE_H
#define NT_TYPE_H

#include "boost/python/dict.hpp"
#include "PvObject.h"

class NtType : public PvObject
{
public:
    NtType(const NtType& ntType);
    virtual ~NtType();
protected:
    NtType(const boost::python::dict& pyDict);
    NtType(const epics::pvData::PVStructurePtr& pvStructurePtr);
};

#endif
