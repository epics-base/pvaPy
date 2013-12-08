#ifndef PV_SCALAR_H
#define PV_SCALAR_H

#include "boost/python/dict.hpp"
#include "PvObject.h"

class PvScalar : public PvObject
{
public:
    PvScalar(const PvScalar& pvScalar);
    virtual ~PvScalar();
    virtual operator int() const;  
    virtual operator double() const;
    virtual int toInt() const;
    virtual long long toLongLong() const;
    virtual double toDouble() const;
    virtual std::string toString() const;
    virtual PvScalar& add(int i);
protected:
    PvScalar(const boost::python::dict& pyDict);
};

#endif
