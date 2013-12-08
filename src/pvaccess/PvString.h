#ifndef PV_STRING_H
#define PV_STRING_H

#include <string>
#include "pv/pvData.h"
#include "PvScalar.h"

class PvString : public PvScalar
{
public:
    PvString();
    PvString(const std::string& s);
    virtual ~PvString();

    void set(const std::string& s);
    std::string get() const;
private:
    static boost::python::dict createStructureDict();
};

#endif
