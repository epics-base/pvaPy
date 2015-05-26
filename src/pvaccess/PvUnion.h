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
    PvUnion(const PvObject&);
    virtual ~PvUnion();
    epics::pvData::UnionConstPtr getUnionPtr() const;
private:
    epics::pvData::UnionConstPtr unionPtr;
    static boost::python::dict createVariantUnionStructureDict();
    //static boost::python::dict createUnionStructureDict();
};

inline epics::pvData::UnionConstPtr PvUnion::getUnionPtr() const 
{
    return unionPtr;
}

#endif
