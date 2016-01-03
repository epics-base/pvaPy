#include "boost/python/enum.hpp"
#include "PvType.h"

using namespace boost::python;

//
// PvType enum 
// 
void wrapPvType()
{

enum_<PvType::ScalarType>("PvType")
    .value("BOOLEAN", PvType::Boolean)
    .value("BYTE", PvType::Byte)
    .value("UBYTE", PvType::UByte)
    .value("SHORT", PvType::Short)
    .value("USHORT", PvType::UShort)
    .value("INT", PvType::Int)
    .value("UINT", PvType::UInt)
    .value("LONG", PvType::Long)
    .value("ULONG", PvType::ULong)
    .value("FLOAT", PvType::Float)
    .value("DOUBLE", PvType::Double)
    .value("STRING", PvType::String)
    .export_values()
;
    
} // wrapPvType()

