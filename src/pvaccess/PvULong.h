#ifndef PV_ULONG_H
#define PV_ULONG_H

#include "pv/pvData.h"
#include "PvScalar.h"

class PvULong : public PvScalar
{
public:
    PvULong();
    PvULong(unsigned long long ull);
    virtual ~PvULong();

    void set(unsigned long long ull);
    unsigned long long get() const;
private:
    static boost::python::dict createStructureDict();
};

#endif
