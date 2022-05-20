// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef NT_ATTRIBUTE_H
#define NT_ATTRIBUTE_H

#include "boost/python/dict.hpp"
#include "boost/python/tuple.hpp"
#include "PvObject.h"
#include "PvTimeStamp.h"
#include "PvAlarm.h"
#include "NtType.h"

class NtAttribute : public NtType
{
public:
    // Constants
    static const char* StructureId;

    static const char* NameFieldKey;
    static const char* TagsFieldKey;
    static const char* SourceTypeFieldKey;
    static const char* SourceFieldKey;

    // Static methods
    static boost::python::dict createStructureDict();

    // Instance methods
    NtAttribute();
    NtAttribute(const std::string& name, const PvObject& value);
    NtAttribute(const boost::python::dict& structureDict, const std::string& structureId=StructureId);
    NtAttribute(const epics::pvData::PVStructurePtr& pvStructurePtr);
    NtAttribute(const NtAttribute& ntAttribute); 
    virtual ~NtAttribute();

    virtual void setName(const std::string& name);
    virtual std::string getName() const;
    virtual void setValue(const PvObject& value);
    virtual PvObject getValue() const;
    virtual void setTags(const boost::python::list& pyList);
    virtual boost::python::list getTags() const;
    virtual void setDescriptor(const std::string& descriptor);
    virtual std::string getDescriptor() const;
    virtual void setTimeStamp(const PvTimeStamp& pvTimeStamp);
    virtual PvTimeStamp getTimeStamp() const;
    virtual void setAlarm(const PvAlarm& pvAlarm);
    virtual PvAlarm getAlarm() const;
    virtual void setSourceType(int sourceType);
    virtual int getSourceType() const;
    virtual void setSource(const std::string& source);
    virtual std::string getSource() const;

};


#endif
