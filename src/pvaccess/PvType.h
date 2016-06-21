// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PV_TYPE_H
#define PV_TYPE_H

#include "pv/pvIntrospect.h"

namespace PvType
{

enum DataType {
    Scalar = epics::pvData::scalar,
    ScalarArray = epics::pvData::scalarArray,
    Structure = epics::pvData::structure,
    StructureArray = epics::pvData::structureArray,
    Union = epics::pvData::union_,
    UnionArray = epics::pvData::unionArray,
    Variant,
    VariantArray,
};

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

enum UnionType {
    RestrictedUnion,
    VariantUnion,
};

}

#endif

