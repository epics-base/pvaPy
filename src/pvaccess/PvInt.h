#ifndef PV_INT_H
#define PV_INT_H

#include "pv/pvData.h"
#include "PvScalar.h"

class PvInt : public PvScalar
{
public:
    PvInt();
    PvInt(int i);
    virtual ~PvInt();

    void set(int i);
    int get() const;
private:
    static boost::python::dict createStructureDict();
};

#endif
