// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef NT_SCALAR_H
#define NT_SCALAR_H

#include <string>
#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include "PvObject.h"
#include "PvType.h"
#include "PvTimeStamp.h"
#include "PvAlarm.h"
#include "PvDisplay.h"
#include "PvControl.h"
#include "NtType.h"

class NtScalar : public NtType
{
public:
    // Constants
    static const char* StructureId;

    static const char* DisplayFieldKey;
    static const char* ControlFieldKey;

    // Static methods
    static boost::python::dict createStructureDict(PvType::ScalarType scalarType);

    // Instance methods
    NtScalar(PvType::ScalarType scalarType);
    NtScalar(PvType::ScalarType scalarType, const boost::python::object& pyObject);
    NtScalar(const PvObject& pvObject);
    NtScalar(const NtScalar& ntScalar);
    virtual ~NtScalar();

    virtual void setValue(const boost::python::object& pyObject);
    virtual boost::python::object getValue() const;
    virtual void setDescriptor(const std::string& descriptor);
    virtual std::string getDescriptor() const;
    virtual void setAlarm(const PvAlarm& pvAlarm);
    virtual PvAlarm getAlarm() const;
    virtual void setTimeStamp(const PvTimeStamp& pvTimeStamp);
    virtual PvTimeStamp getTimeStamp() const;
    virtual void setDisplay(const PvDisplay& pvDisplay);
    virtual PvDisplay getDisplay() const;
    virtual void setControl(const PvControl& pvControl);
    virtual PvControl getControl() const;
};

#endif
