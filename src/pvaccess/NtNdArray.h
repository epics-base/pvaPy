// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef NT_ND_ARRAY_H
#define NT_ND_ARRAY_H

#include <string>
#include "boost/python/dict.hpp"
#include "boost/python/list.hpp"
#include "PvObject.h"
#include "PvType.h"
#include "PvTimeStamp.h"
#include "PvaConstants.h"
#include "PvAlarm.h"
#include "PvDisplay.h"
#include "PvCodec.h"
#include "PvDimension.h"
#include "NtType.h"

class NtNdArray : public NtType
{
public:
    // Constants
    static const char* StructureId;

    static const char* BooleanValueFieldKey;
    static const char* ByteValueFieldKey;
    static const char* UByteValueFieldKey;
    static const char* ShortValueFieldKey;
    static const char* UShortValueFieldKey;
    static const char* IntValueFieldKey;
    static const char* UIntValueFieldKey;
    static const char* LongValueFieldKey;
    static const char* ULongValueFieldKey;
    static const char* FloatValueFieldKey;
    static const char* DoubleValueFieldKey;

    static const char* AttributeFieldKey;
    static const char* CodecFieldKey;
    static const char* CompressedSizeFieldKey;
    static const char* DataTimeStampFieldKey;
    static const char* DimensionFieldKey;
    static const char* DisplayFieldKey;
    static const char* UncompressedSizeFieldKey;
    static const char* UniqueIdFieldKey;

    // Static methods
    static boost::python::dict createStructureDict(const boost::python::dict& extraFieldsDict = boost::python::dict());
    static boost::python::dict createStructureFieldIdDict();

    // Instance methods
    NtNdArray();
    NtNdArray(const boost::python::dict& extraFieldsDict);
    NtNdArray(const PvObject& pvObject);
    NtNdArray(const boost::python::dict& structureDict, const boost::python::dict& valueDict, const std::string& structureId, const boost::python::dict& structureFieldIdDict);
    NtNdArray(const NtNdArray& ntNdArray);
    virtual ~NtNdArray();

    virtual void setValue(const boost::python::dict& pyDict);
    virtual void setValue(const PvObject& pvObject);
    virtual boost::python::object getValue() const;
    virtual void setCodec(const PvCodec& pvCodec);
    virtual PvCodec getCodec() const;
    virtual void setCompressedSize(int value);
    virtual int getCompressedSize() const;
    virtual void setUncompressedSize(int value);
    virtual int getUncompressedSize() const;
    virtual void setDimension(const boost::python::list& pyList);
    virtual boost::python::list getDimension() const;
    virtual void setUniqueId(int value);
    virtual int getUniqueId() const;
    virtual void setDataTimeStamp(const PvTimeStamp& pvTimeStamp);
    virtual PvTimeStamp getDataTimeStamp() const;
    virtual void setAttribute(const boost::python::list& pyList);
    virtual boost::python::list getAttribute() const;
    virtual void setDescriptor(const std::string& descriptor);
    virtual std::string getDescriptor() const;
    virtual void setAlarm(const PvAlarm& pvAlarm);
    virtual PvAlarm getAlarm() const;
    virtual void setTimeStamp(const PvTimeStamp& pvTimeStamp);
    virtual PvTimeStamp getTimeStamp() const;
    virtual void setDisplay(const PvDisplay& pvDisplay);
    virtual PvDisplay getDisplay() const;
};

struct NtNdArrayPickleSuite : boost::python::pickle_suite
{
    static boost::python::tuple getinitargs(const NtNdArray& ntNdArray)
    {
        return boost::python::make_tuple(
            ntNdArray.getStructureDict(),
            ntNdArray.get(),
            NtNdArray::StructureId,
            NtNdArray::createStructureFieldIdDict());
    }
};

#endif
