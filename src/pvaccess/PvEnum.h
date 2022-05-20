// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PV_ENUM_H
#define PV_ENUM_H

#include <boost/python/dict.hpp>
#include "PvObject.h"

class PvEnum : public PvObject
{
public:
    // Constants
    static const char* StructureId;

    static const char* IndexFieldKey;
    static const char* ChoicesFieldKey;

    static const int UnknownChoice;

    // Static methods
    static boost::python::dict createStructureDict();

    // Instance methods
    PvEnum();
    PvEnum(const boost::python::list& choices, int index=0);
    PvEnum(const boost::python::dict& pyDict, const std::string& structureId=StructureId);
    PvEnum(const epics::pvData::PVStructurePtr& pvStructurePtr);
    PvEnum(const PvEnum& pvEnum); 
    virtual ~PvEnum();

    virtual void setIndex(int index);
    virtual int getIndex() const;
    virtual void setChoices(const boost::python::list& choices);
    virtual boost::python::list getChoices() const;
    virtual std::string getCurrentChoice() const;

private:
    void setMaxIndex();
    int maxIndex;
};


#endif
