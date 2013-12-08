#ifndef PV_SCALAR_ARRAY_H
#define PV_SCALAR_ARRAY_H

#include "boost/python/dict.hpp"
#include "boost/python/list.hpp"
#include "PvObject.h"
#include "PvType.h"

class PvScalarArray : public PvObject
{
public:
    PvScalarArray(PvType::ScalarType scalarType);
    PvScalarArray(const PvScalarArray& pvScalarArray);
    virtual ~PvScalarArray();

    operator boost::python::list() const;
    boost::python::list toList() const;

    virtual void set(const boost::python::list& pyList);
    virtual boost::python::list get() const;
private:
    static boost::python::dict createStructureDict(PvType::ScalarType scalarType);
};

#endif
