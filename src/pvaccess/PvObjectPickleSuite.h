// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PV_OBJECT_PICKLE_SUITE_H
#define PV_OBJECT_PICKLE_SUITE_H

#include "boost/python/module.hpp"
#include "boost/python/tuple.hpp"

#include "PvObject.h"

struct PvObjectPickleSuite : boost::python::pickle_suite
{
    static boost::python::tuple getinitargs(const PvObject& pvObject)
    {
        return boost::python::make_tuple(
            pvObject.getStructureDict(),
            pvObject.get());
    }
};

#endif

