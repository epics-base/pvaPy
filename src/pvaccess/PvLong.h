#ifndef PV_LONG_H
#define PV_LONG_H

#include "pv/pvData.h"
#include "PvScalar.h"

class PvLong : public PvScalar
{
public:
    PvLong();
    PvLong(long long ll);
    virtual ~PvLong();

    void set(long long ll);
    long long get() const;
private:
    static boost::python::dict createStructureDict();
};

#endif
