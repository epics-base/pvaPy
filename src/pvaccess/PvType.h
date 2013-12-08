#ifndef PV_TYPE_H
#define PV_TYPE_H

#include "pv/pvIntrospect.h"

namespace PvType
{

enum ScalarType {
    Boolean = epics::pvData::pvBoolean,
    Byte = epics::pvData::pvByte,
    UByte = epics::pvData::pvUByte,
    Short = epics::pvData::pvShort,
    UShort = epics::pvData::pvUShort,
    Int = epics::pvData::pvInt,
    UInt = epics::pvData::pvUInt,
    Long = epics::pvData::pvLong,
    ULong = epics::pvData::pvULong,
    Float = epics::pvData::pvFloat,
    Double = epics::pvData::pvDouble,
    String = epics::pvData::pvString,
};

}

#endif

