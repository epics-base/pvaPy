// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef SCALAR_ARRAY_PY_OWNER_H
#define SCALAR_ARRAY_PY_OWNER_H

#include "boost/python/object.hpp"
#include "pv/pvData.h"

//
// This class is used for maintaining ownership of scalar arrays in python
//
class ScalarArrayPyOwner : public boost::python::object
{
public:

    POINTER_DEFINITIONS(ScalarArrayPyOwner);
    ScalarArrayPyOwner() :
        boost::python::object() {}
    ScalarArrayPyOwner(const epics::pvData::PVScalarArrayPtr& pvScalarArrayPtr_) :
        boost::python::object(),
        pvScalarArrayPtr(pvScalarArrayPtr_) {}
    virtual ~ScalarArrayPyOwner() {}

private:
    epics::pvData::PVScalarArrayPtr pvScalarArrayPtr;
};

#endif // SCALAR_ARRAY_PY_OWNER_H

