// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef NT_ENUM_H
#define NT_ENUM_H

#include <string>
#include "boost/python/dict.hpp"
#include "boost/python/list.hpp"
#include "PvObject.h"
#include "PvType.h"
#include "PvEnum.h"
#include "PvTimeStamp.h"
#include "PvaConstants.h"
#include "PvAlarm.h"
#include "NtType.h"

class NtEnum : public NtType
{
public:
    // Constants
    static const char* StructureId;

    // Static methods
    static boost::python::dict createStructureDict();
    static boost::python::dict createStructureFieldIdDict();

    // Instance methods
    NtEnum();
    NtEnum(const boost::python::list& choices, int index=0);
    NtEnum(const boost::python::dict& pyDict, const std::string& structureId=StructureId);
    NtEnum(const PvObject& pvObject);
    NtEnum(const NtEnum& ntEnum);
    virtual ~NtEnum();

    virtual void setValue(int index);
    virtual void setValue(const boost::python::dict& pyDict);
    virtual void setValue(const PvEnum& pvEnum);
    virtual PvEnum getValue() const;
    virtual void setDescriptor(const std::string& descriptor);
    virtual std::string getDescriptor() const;
    virtual void setAlarm(const PvAlarm& pvAlarm);
    virtual PvAlarm getAlarm() const;
    virtual void setTimeStamp(const PvTimeStamp& pvTimeStamp);
    virtual PvTimeStamp getTimeStamp() const;
};

#endif
