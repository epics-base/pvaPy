#ifndef PV_FLOAT_H
#define PV_FLOAT_H

#include "pv/pvData.h"
#include "PvScalar.h"

class PvFloat : public PvScalar
{
public:
    PvFloat();
    PvFloat(float f);
    virtual ~PvFloat();

    void set(float f);
    float get() const;
private:
    static boost::python::dict createStructureDict();
};

#endif
