// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef NT_TYPE_H
#define NT_TYPE_H

#include "boost/python/dict.hpp"
#include "PvObject.h"

class NtType : public PvObject
{
public:
    // Constants
    static const char* StructureId;
    static const char* AlarmFieldKey;
    static const char* DescriptorFieldKey;
    static const char* TimeStampFieldKey;

    // Instance methods
    NtType(const NtType& ntType); 
    virtual ~NtType();
protected:
    NtType(const boost::python::dict& pyDict, const std::string& structureId=StructureId, const boost::python::dict& structureFieldIdDict=boost::python::dict());
    NtType(const epics::pvData::PVStructurePtr& pvStructurePtr);
};

#endif
