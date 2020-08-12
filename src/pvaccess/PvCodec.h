// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PV_CODEC_H
#define PV_CODEC_H

#include "boost/python/dict.hpp"
#include "boost/python/tuple.hpp"
#include "PvObject.h"

class PvCodec : public PvObject
{
public:
    // Constants
    static const char* StructureId;

    static const char* NameFieldKey;
    static const char* ParametersFieldKey;

    // Static methods
    static boost::python::dict createStructureDict();

    // Instance methods
    PvCodec();
    PvCodec(const std::string& name, const PvObject& parameters);
    PvCodec(const boost::python::dict& structureDict, const std::string& structureId=StructureId);
    PvCodec(const epics::pvData::PVStructurePtr& pvStructurePtr);
    PvCodec(const PvCodec& pvCodec); 
    virtual ~PvCodec();

    virtual void setName(const std::string& name);
    virtual std::string getName() const;
    virtual void setParameters(const PvObject& parameters);
    virtual PvObject getParameters() const;
};


#endif
