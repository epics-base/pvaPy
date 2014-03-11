#ifndef NT_TABLE_H
#define NT_TABLE_H

#include "boost/python/dict.hpp"
#include "boost/python/list.hpp"
#include "PvObject.h"
#include "PvType.h"
#include "NtType.h"

class NtTable : public NtType
{
public:
    // Constants
    static const char* LabelsFieldKey;

    NtTable(int nColumns, PvType::ScalarType scalarType);
    NtTable(const NtTable& ntTable);
    virtual ~NtTable();

    virtual void setLabels(const boost::python::list& pyList);
    virtual void setColumn(int column, const boost::python::list& pyList);
    virtual boost::python::list getLabels() const;
    virtual boost::python::list getColumn(int column) const;
private:
    static std::string getColumnName(int column);
    static boost::python::dict createStructureDict(int nColumns, PvType::ScalarType scalarType);
};

#endif
