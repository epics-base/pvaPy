#ifndef PV_USHORT_H
#define PV_USHORT_H

#include "pv/pvData.h"
#include "PvScalar.h"

class PvUShort : public PvScalar
{
public:
    PvUShort();
    PvUShort(unsigned short us);
    virtual ~PvUShort();

    void set(unsigned short us);
    unsigned short get() const;
private:
    static boost::python::dict createStructureDict();
};

#endif
