// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PV_UNION_H
#define PV_UNION_H

#include "pv/pvData.h"
#include "boost/python/dict.hpp"
#include "boost/python/tuple.hpp"

#include "PvType.h"
#include "PvObject.h"

class PvUnion : public PvObject
{
public:
    PvUnion();
    PvUnion(const boost::python::dict& structureDict);
    PvUnion(const PvObject& pvObject);
    virtual ~PvUnion();
    //epics::pvData::UnionConstPtr getUnionPtr() const;
private:
    epics::pvData::UnionConstPtr unionPtr;
    static boost::python::dict createVariantUnionStructureDict();
    static boost::python::dict createUnionStructureDict(const boost::python::dict& pyDict);
};

/**
inline epics::pvData::UnionConstPtr PvUnion::getUnionPtr() const 
{
    return unionPtr;
}
*/

#endif
