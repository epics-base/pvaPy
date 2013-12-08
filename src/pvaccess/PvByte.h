#ifndef PV_BYTE_H
#define PV_BYTE_H

#include "pv/pvData.h"
#include "PvScalar.h"

class PvByte : public PvScalar
{
public:
    PvByte();
    PvByte(char i);
    virtual ~PvByte();

    void set(char i);
    char get() const;
private:
    static boost::python::dict createStructureDict();
};

#endif
