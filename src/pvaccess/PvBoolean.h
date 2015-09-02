// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PV_BOOLEAN_H
#define PV_BOOLEAN_H

#include "pv/pvData.h"
#include "PvScalar.h"

class PvBoolean : public PvScalar
{
public:
    PvBoolean();
    PvBoolean(int i);
    virtual ~PvBoolean();

    void set(bool b);
    bool get() const;
private:
    static boost::python::dict createStructureDict();
};

#endif
