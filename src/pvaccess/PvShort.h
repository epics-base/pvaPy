// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PV_SHORT_H
#define PV_SHORT_H

#include "pv/pvData.h"
#include "PvScalar.h"

class PvShort : public PvScalar
{
public:
    PvShort();
    PvShort(short s);
    virtual ~PvShort();

    void set(short s);
    short get() const;
private:
    static boost::python::dict createStructureDict();
};

#endif
