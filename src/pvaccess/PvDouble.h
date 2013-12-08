#ifndef PV_DOUBLE_H
#define PV_DOUBLE_H

#include "pv/pvData.h"
#include "PvScalar.h"

class PvDouble : public PvScalar
{
public:
    PvDouble();
    PvDouble(double d);
    virtual ~PvDouble();

    void set(double d);
    double get() const;
private:
    static boost::python::dict createStructureDict();
};

#endif
