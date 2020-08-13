// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PV_DIMENSION_H
#define PV_DIMENSION_H

#include "boost/python/dict.hpp"
#include "boost/python/tuple.hpp"
#include "PvObject.h"

class PvDimension : public PvObject
{
public:
    // Constants
    static const char* StructureId;

    static const char* SizeFieldKey;
    static const char* OffsetFieldKey;
    static const char* FullSizeFieldKey;
    static const char* BinningFieldKey;
    static const char* ReverseFieldKey;

    // Static methods
    static boost::python::dict createStructureDict();

    // Instance methods
    PvDimension();
    PvDimension(int size, int offset, int fullSize, int binning, bool reverse);
    PvDimension(const boost::python::dict& structureDict, const std::string& structureId=StructureId);
    PvDimension(const epics::pvData::PVStructurePtr& pvStructurePtr);
    PvDimension(const PvDimension& pvDimension); 
    virtual ~PvDimension();

    virtual void setSize(int value);
    virtual int getSize() const;
    virtual void setOffset(int value);
    virtual int getOffset() const;
    virtual void setFullSize(int value);
    virtual int getFullSize() const;
    virtual void setBinning(int value);
    virtual int getBinning() const;
    virtual void setReverse(bool value);
    virtual bool getReverse() const;
};


#endif
