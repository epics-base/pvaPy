
#include <string>
#include <istream>
#include <sstream>

#include "PvUtility.h"
#include "InvalidArgument.h"
#include "pv/convert.h"

namespace PvUtility 
{

#if defined PVA_API_VERSION && PVA_API_VERSION == 430

void fromString(const epics::pvData::PVScalarPtr& pvScalar, const epics::pvData::String& from)
{
    epics::pvData::ScalarConstPtr scalar = pvScalar->getScalar();
    epics::pvData::ScalarType scalarType = scalar->getScalarType();
    switch(scalarType) {
        case epics::pvData::pvBoolean: {
            epics::pvData::PVBooleanPtr pv = std::tr1::static_pointer_cast<epics::pvData::PVBoolean>(pvScalar);
            bool isTrue  = (from.compare("true")==0  || from.compare("1")==0);
            bool isFalse = (from.compare("false")==0 || from.compare("0")==0);
            if (!(isTrue || isFalse)) {
                throw InvalidArgument("Failed to parse field " + pvScalar->getFieldName() + " (boolean) from string value '" + from + "'");
            }
            pv->put(isTrue == true);
            return;
        }
        case epics::pvData::pvByte: {
            epics::pvData::PVBytePtr pv = std::tr1::static_pointer_cast<epics::pvData::PVByte>(pvScalar);
            int ival;
            int result = sscanf(from.c_str(),"%d",&ival);
            if (result != 1) {
                throw InvalidArgument("Failed to parse field " + pvScalar->getFieldName() + " (byte) from string value '" + from + "'");
            }
            epics::pvData::int8 value = ival;
            pv->put(value);
            return;
        }
        case epics::pvData::pvShort: {
            epics::pvData::PVShortPtr pv = std::tr1::static_pointer_cast<epics::pvData::PVShort>(pvScalar);
            int ival;
            int result = sscanf(from.c_str(),"%d",&ival);
            if (result != 1) {
               	throw InvalidArgument("Failed to parse field " + pvScalar->getFieldName() + " (short) from string value '" + from + "'");
            }
            epics::pvData::int16 value = ival;
            pv->put(value);
            return;
        }
        case epics::pvData::pvInt: {
            epics::pvData::PVIntPtr pv = std::tr1::static_pointer_cast<epics::pvData::PVInt>(pvScalar);
            int ival;
            int result = sscanf(from.c_str(),"%d",&ival);
            if (result != 1) {
                throw InvalidArgument("Failed to parse field " + pvScalar->getFieldName() + " (int) from string value '" + from + "'");
            }
            epics::pvData::int32 value = ival;
            pv->put(value);
            return;
        }
        case epics::pvData::pvLong: {
            epics::pvData::PVLongPtr pv = std::tr1::static_pointer_cast<epics::pvData::PVLong>(pvScalar);
            epics::pvData::int64 ival;
            int result = sscanf(from.c_str(),"%lld",(long long *)&ival);
            if (result != 1) {
                throw InvalidArgument("Failed to parse field " + pvScalar->getFieldName() + " (long) from string value '" + from + "'");
            }
            epics::pvData::int64 value = ival;
            pv->put(value);
            return;
        }
        case epics::pvData::pvUByte: {
            epics::pvData::PVUBytePtr pv = std::tr1::static_pointer_cast<epics::pvData::PVUByte>(pvScalar);
            unsigned int ival;
            int result = sscanf(from.c_str(),"%u",&ival);
            if (result != 1) {
                throw InvalidArgument("Failed to parse field " + pvScalar->getFieldName() + " (ubyte) from string value '" + from + "'");
            }
            epics::pvData::uint8 value = ival;
            pv->put(value);
            return;
        }
        case epics::pvData::pvUShort: {
            epics::pvData::PVUShortPtr pv = std::tr1::static_pointer_cast<epics::pvData::PVUShort>(pvScalar);
            unsigned int ival;
            int result = sscanf(from.c_str(),"%u",&ival);
            if (result != 1) {
                throw InvalidArgument("Failed to parse field " + pvScalar->getFieldName() + " (ushort) from string value '" + from + "'");
            }
            epics::pvData::uint16 value = ival;
            pv->put(value);
            return;
        }
        case epics::pvData::pvUInt: {
            epics::pvData::PVUIntPtr pv = std::tr1::static_pointer_cast<epics::pvData::PVUInt>(pvScalar);
            unsigned int ival;
            int result = sscanf(from.c_str(),"%u",&ival);
            if (result != 1) {
                throw InvalidArgument("Failed to parse field " + pvScalar->getFieldName() + " (uint) from string value '" + from + "'");
            }
            epics::pvData::uint32 value = ival;
            pv->put(value);
            return;
        }
        case epics::pvData::pvULong: {
            epics::pvData::PVULongPtr pv = std::tr1::static_pointer_cast<epics::pvData::PVULong>(pvScalar);
            unsigned long long ival;
            int result = sscanf(from.c_str(),"%llu",(long long unsigned int *)&ival);
            if (result != 1) {
               	throw InvalidArgument("Failed to parse field " + pvScalar->getFieldName() + " (ulong) from string value '" + from + "'");
            }
            epics::pvData::uint64 value = ival;
            pv->put(value);
            return;
        }
        case epics::pvData::pvFloat: {
            epics::pvData::PVFloatPtr pv = std::tr1::static_pointer_cast<epics::pvData::PVFloat>(pvScalar);
            float value;
            int result = sscanf(from.c_str(),"%f",&value); 
            if (result != 1) {
                throw InvalidArgument("Failed to parse field " + pvScalar->getFieldName() + " (float) from string value '" + from + "'");
            }
            pv->put(value);
            return;
        }
        case epics::pvData::pvDouble: {
            epics::pvData::PVDoublePtr pv = std::tr1::static_pointer_cast<epics::pvData::PVDouble>(pvScalar);
            double value;
            int result = sscanf(from.c_str(),"%lf",&value);
            if (result != 1) {
                throw InvalidArgument("Failed to parse field " + pvScalar->getFieldName() + " (double) from string value '" + from + "'");
            }
            pv->put(value);
            return;
        }
        case epics::pvData::pvString: {
            epics::pvData::PVStringPtr value = std::tr1::static_pointer_cast<epics::pvData::PVString>(pvScalar);
            value->put(from);
            return;
        }
        default: {
            std::ostringstream oss;
            oss << "Unsupported scalar type: " << scalarType;
            throw PvaException(oss.str());
        }
    }
}

size_t convertFromStringArray(epics::pvData::PVScalarArray* pv, size_t offset, size_t len, const epics::pvData::StringArray& from, size_t fromOffset)
    
{
    epics::pvData::ScalarType elemType = pv->getScalarArray()->getElementType();
    size_t ntransfered = 0;
    switch (elemType) {
        case epics::pvData::pvBoolean: {
            epics::pvData::PVBooleanArray* pvdata = static_cast<epics::pvData::PVBooleanArray*>(pv);
            epics::pvData::boolean data[1];
            while (len > 0) {
                epics::pvData::String fromString = from[fromOffset];
                bool isTrue  = (fromString.compare("true")==0  || fromString.compare("1")==0);
                bool isFalse = (fromString.compare("false")==0 || fromString.compare("0")==0);
                if (!(isTrue || isFalse)) {
            	    char soffset[64];
            	    sprintf(soffset, "%u", (unsigned int)offset);
            	    throw InvalidArgument("Failed to parse field " + pv->getFieldName() + " (boolean array at index " + soffset + ") from string value '" + fromString + "'");
                }
                data[0] = isTrue == true;
                if (pvdata->put(offset, 1, data, 0) == 0) {
                    return ntransfered;
                }
                --len;
                ++ntransfered;
                ++offset;
                ++fromOffset;
            }
            return ntransfered;
        }
        case epics::pvData::pvByte: {
            epics::pvData::PVByteArray* pvdata = static_cast<epics::pvData::PVByteArray*>(pv);
            epics::pvData::int8 data[1];
            while (len > 0) {
                epics::pvData::String fromString = from[fromOffset];
                int ival;
                int result = sscanf(fromString.c_str(),"%d",&ival);
                if (result != 1) {
            	    char soffset[64];
            	    sprintf(soffset, "%u", (unsigned int)offset);
            	    throw InvalidArgument("Failed to parse field " + pv->getFieldName() + " (byte array at index " + soffset + ") from string value '" + fromString + "'");
                }
                data[0] = ival;
                if (pvdata->put(offset, 1, data, 0) == 0) {
                    return ntransfered;
                }
                --len;
                ++ntransfered;
                ++offset;
                ++fromOffset;
            }
            return ntransfered;
        }
        case epics::pvData::pvShort: {
            epics::pvData::PVShortArray* pvdata = static_cast<epics::pvData::PVShortArray*>(pv);
            epics::pvData::int16 data[1];
            while (len > 0) {
                epics::pvData::String fromString = from[fromOffset];
                int ival;
                int result = sscanf(fromString.c_str(),"%d",&ival);
                if (result != 1) {
            	    char soffset[64];
            	    sprintf(soffset, "%u", (unsigned int)offset);
            	    throw InvalidArgument("Failed to parse field " + pv->getFieldName() + " (short array at index " + soffset + ") from string value '" + fromString + "'");
                }
                data[0] = ival;
                if (pvdata->put(offset, 1, data, 0) == 0) {
                    return ntransfered;
                }
                --len;
                ++ntransfered;
                ++offset;
                ++fromOffset;
            }
            return ntransfered;
        }
        case epics::pvData::pvInt: {
            epics::pvData::PVIntArray* pvdata = static_cast<epics::pvData::PVIntArray*>(pv);
            epics::pvData::int32 data[1];
            while (len > 0) {
                epics::pvData::String fromString = from[fromOffset];
                int ival;
                int result = sscanf(fromString.c_str(),"%d",&ival);
                if (result != 1) {
            	    char soffset[64];
            	    sprintf(soffset, "%u", (unsigned int)offset);
            	    throw InvalidArgument("Failed to parse field " + pv->getFieldName() + " (int array at index " + soffset + ") from string value '" + fromString + "'");
                }
                data[0] = ival;
                if (pvdata->put(offset, 1, data, 0) == 0) {
                    return ntransfered;
                }
                --len;
                ++ntransfered;
                ++offset;
                ++fromOffset;
            }
            return ntransfered;
        }
        case epics::pvData::pvLong: {
            epics::pvData::PVLongArray *pvdata = static_cast<epics::pvData::PVLongArray*>(pv);
            epics::pvData::int64 data[1];
            while (len > 0) {
                epics::pvData::String fromString = from[fromOffset];
                epics::pvData::int64 ival;
                int result = sscanf(fromString.c_str(),"%lld",(long long int *)&ival);
                if (result != 1) {
            	    char soffset[64];
            	    sprintf(soffset, "%u", (unsigned int)offset);
            	    throw InvalidArgument("Failed to parse field " + pv->getFieldName() + " (long array at index " + soffset + ") from string value '" + fromString + "'");
                }
                data[0] = ival;
                if (pvdata->put(offset, 1, data, 0) == 0) {
                    return ntransfered;
                }
                --len;
                ++ntransfered;
                ++offset;
                ++fromOffset;
            }
            return ntransfered;
        }
        case epics::pvData::pvUByte: {
            epics::pvData::PVUByteArray* pvdata = static_cast<epics::pvData::PVUByteArray*>(pv);
            epics::pvData::uint8 data[1];
            while (len > 0) {
                epics::pvData::String fromString = from[fromOffset];
                unsigned int ival;
                int result = sscanf(fromString.c_str(),"%u",&ival);
                if (result != 1) {
            	    char soffset[64];
            	    sprintf(soffset, "%u", (unsigned int)offset);
            	    throw InvalidArgument("failed to parse field " + pv->getFieldName() + " (ubyte array at index " + soffset + ") from string value '" + fromString + "'");
                }
                data[0] = ival;
                if (pvdata->put(offset, 1, data, 0) == 0) {
                    return ntransfered;
                }
                --len;
                ++ntransfered;
                ++offset;
                ++fromOffset;
            }
            return ntransfered;
        }
        case epics::pvData::pvUShort: {
            epics::pvData::PVUShortArray* pvdata = static_cast<epics::pvData::PVUShortArray*>(pv);
            epics::pvData::uint16 data[1];
            while (len > 0) {
                epics::pvData::String fromString = from[fromOffset];
                unsigned int ival;
                int result = sscanf(fromString.c_str(),"%u",&ival);
                if (result != 1) {
            	    char soffset[64];
            	    sprintf(soffset, "%u", (unsigned int)offset);
            	    throw InvalidArgument("Failed to parse field " + pv->getFieldName() + " (ushort array at index " + soffset + ") from string value '" + fromString + "'");
                }
                data[0] = ival;
                if (pvdata->put(offset, 1, data, 0) == 0) {
                    return ntransfered;
                }
                --len;
                ++ntransfered;
                ++offset;
                ++fromOffset;
            }
            return ntransfered;
        }
        case epics::pvData::pvUInt: {
            epics::pvData::PVUIntArray* pvdata = static_cast<epics::pvData::PVUIntArray*>(pv);
            epics::pvData::uint32 data[1];
            while (len > 0) {
                epics::pvData::String fromString = from[fromOffset];
                unsigned int ival;
                int result = sscanf(fromString.c_str(),"%u",&ival);
                if (result != 1) {
            	    char soffset[64];
            	    sprintf(soffset, "%u", (unsigned int)offset);
            	    throw InvalidArgument("Failed to parse field " + pv->getFieldName() + " (uint array at index " + soffset + ") from string value '" + fromString + "'");
                }
                data[0] = ival;
                if (pvdata->put(offset, 1, data, 0) == 0) {
                    return ntransfered;
                }
                --len;
                ++ntransfered;
                ++offset;
                ++fromOffset;
            }
            return ntransfered;
        }
        case epics::pvData::pvULong: {
            epics::pvData::PVULongArray* pvdata = static_cast<epics::pvData::PVULongArray*>(pv);
            epics::pvData::uint64 data[1];
            while (len > 0) {
                epics::pvData::String fromString = from[fromOffset];
                epics::pvData::uint64 ival;
                int result = sscanf(fromString.c_str(),"%lld",(unsigned long long int *)&ival);
                if (result != 1) {
            	    char soffset[64];
            	    sprintf(soffset, "%u", (unsigned int)offset);
            	    throw InvalidArgument("Failed to parse field " + pv->getFieldName() + " (ulong array at index " + soffset + ") from string value '" + fromString + "'");
                }
                data[0] = ival;
                if (pvdata->put(offset, 1, data, 0) == 0) {
                    return ntransfered;
                }
                --len;
                ++ntransfered;
                ++offset;
                ++fromOffset;
            }
            return ntransfered;
        }
        case epics::pvData::pvFloat: {
            epics::pvData::PVFloatArray* pvdata = static_cast<epics::pvData::PVFloatArray*>(pv);
            float data[1];
            while (len > 0) {
                epics::pvData::String fromString = from[fromOffset];
                float fval;
                int result = sscanf(fromString.c_str(),"%f",&fval);
                if (result != 1) {
            	    char soffset[64];
            	    sprintf(soffset, "%u", (unsigned int)offset);
            	    throw InvalidArgument("Failed to parse field " + pv->getFieldName() + " (float array at index " + soffset + ") from string value '" + fromString + "'");
                }
                data[0] = fval;
                if (pvdata->put(offset, 1, data, 0) == 0) {
                    return ntransfered;
                }
                --len;
                ++ntransfered;
                ++offset;
                ++fromOffset;
            }
            return ntransfered;
        }
        case epics::pvData::pvDouble: {
            epics::pvData::PVDoubleArray* pvdata = static_cast<epics::pvData::PVDoubleArray*>(pv);
            double data[1];
            while (len > 0) {
                epics::pvData::String fromString = from[fromOffset];
                double fval;
                int result = sscanf(fromString.c_str(),"%lf",&fval);
                if (result != 1) {
            	    char soffset[64];
            	    sprintf(soffset, "%u", (unsigned int)offset);
            	    throw InvalidArgument("Failed to parse field " + pv->getFieldName() + " (double array at index " + soffset + ") from string value '" + fromString + "'");
                }
                data[0] = fval;
                if (pvdata->put(offset, 1, data, 0) == 0) {
                    return ntransfered;
                }
                --len;
                ++ntransfered;
                ++offset;
                ++fromOffset;
            }
            return ntransfered;
        }
        case epics::pvData::pvString: {
            epics::pvData::PVStringArray* pvdata = static_cast<epics::pvData::PVStringArray*>(pv);
            while (len > 0) {
                epics::pvData::String* s = const_cast<epics::pvData::String*>(epics::pvData::get(from));
                size_t n = pvdata->put(offset, len, s, fromOffset);
                if (n == 0) {
                    break;
                }
                len -= n;
                offset += n;
                fromOffset += n;
                ntransfered += n;
            }
            return ntransfered;
        }
        default: {
            std::ostringstream oss;
            oss << "Unsupported field type: " << elemType;
            throw PvaException(oss.str());
        }
    }
}

size_t fromStringArray(const epics::pvData::PVScalarArrayPtr& pv, size_t offset, size_t length, const epics::pvData::StringArray& from, size_t fromOffset)
{
    return convertFromStringArray(pv.get(),offset,length,from,fromOffset);
}

size_t fromString(const epics::pvData::PVScalarArrayPtr& pv, const epics::pvData::StringArray& from, size_t fromStartIndex)
{
	int processed = 0;
	size_t fromValueCount = from.size();

	// first get count
	if (fromStartIndex >= fromValueCount) {
		throw InvalidArgument("Not enough values, stopped at field " + pv->getFieldName());
    }

	size_t count;
	std::istringstream iss(from[fromStartIndex]);
	iss >> count;
	// not fail and entire value is parsed (e.g. to detect 1.2 parsing to 1)
	if (iss.fail() || !iss.eof()) {
    	throw InvalidArgument("Failed to parse element count value (uint) of field '" + pv->getFieldName() + "' from string value '" + from[fromStartIndex] + "'");
    }
	fromStartIndex++;
	processed++;

	if ((fromStartIndex+count) > fromValueCount) {
    	throw InvalidArgument("Not enough array values for field " + pv->getFieldName());
	}

    epics::pvData::StringArray valueList;
    valueList.reserve(count);
    for(size_t i=0; i<count; i++) {
    	valueList.push_back(from[fromStartIndex++]);
    }
    processed += count;

    size_t num = fromStringArray(pv,0,count,valueList,0);
    pv->setLength(num);

    return processed;
}

size_t fromString(const epics::pvData::PVStructurePtr& pvStructure, const epics::pvData::StringArray& from, size_t fromStartIndex)
{
    size_t processed = 0;
    size_t fromValueCount = from.size();

    const epics::pvData::PVFieldPtrArray& fieldsData = pvStructure->getPVFields();
    if (fieldsData.size() != 0) {
        size_t length = pvStructure->getStructure()->getNumberFields();
        for(size_t i = 0; i < length; i++) {
            epics::pvData::PVFieldPtr fieldField = fieldsData[i];

            epics::pvData::Type type = fieldField->getField()->getType();
            if(type==epics::pvData::structure) {
                epics::pvData::PVStructurePtr pv = std::tr1::static_pointer_cast<epics::pvData::PVStructure>(fieldField);
                size_t count = fromString(pv, from, fromStartIndex);
                processed += count;
                fromStartIndex += count;
            }
            else if(type==epics::pvData::scalarArray) {
                epics::pvData::PVScalarArrayPtr pv = std::tr1::static_pointer_cast<epics::pvData::PVScalarArray>(fieldField);
                size_t count = fromString(pv, from, fromStartIndex);
                processed += count;
                fromStartIndex += count;
            }
            else if(type==epics::pvData::scalar) {

            	if (fromStartIndex >= fromValueCount) {
            		throw InvalidArgument("Not enough values, stopped at field " + fieldField->getFieldName());
                }

            	epics::pvData::PVScalarPtr pv = std::tr1::static_pointer_cast<epics::pvData::PVScalar>(fieldField);
                fromString(pv, from[fromStartIndex++]);
                processed++;
            }
            else {
                std::ostringstream oss;
                oss << "Unsupported field type: " << type;
                throw PvaException(oss.str());
            }
        }
    }

    return processed;
}

#else

size_t fromString(const epics::pvData::PVScalarArrayPtr& pv, const epics::pvData::StringArray& from, size_t fromStartIndex)
{
	int processed = 0;
	size_t count = from.size();

    epics::pvData::PVStringArray::svector valueList(count);
    std::copy(from.begin() + fromStartIndex, from.begin() + fromStartIndex + count, valueList.begin());
    processed += count;

    pv->putFrom<std::string>(freeze(valueList));

    return processed;
}

size_t fromString(const epics::pvData::PVStructureArrayPtr& pv, const epics::pvData::StringArray& from, size_t fromStartIndex)
{
	int processed = 0;
	size_t numberOfStructures = from.size();

    epics::pvData::PVStructureArray::svector pvStructures;
    pvStructures.reserve(numberOfStructures);

    epics::pvData::PVDataCreatePtr pvDataCreate = epics::pvData::getPVDataCreate();
    for (size_t i = 0; i < numberOfStructures; ++i) {
        epics::pvData::PVStructurePtr pvStructure = pvDataCreate->createPVStructure(pv->getStructureArray()->getStructure());
        size_t count = fromString(pvStructure, from, fromStartIndex);
        processed += count;
        fromStartIndex += count;
        pvStructures.push_back(pvStructure);
    }
    pv->replace(freeze(pvStructures));
    return processed;
}

size_t fromString(const epics::pvData::PVStructurePtr& pvStructure, const epics::pvData::StringArray& from, size_t fromStartIndex)
{
    // handle enum in a special way
    if (pvStructure->getStructure()->getID() == "enum_t") {
        epics::pvData::int32 index = -1;
        epics::pvData::PVInt::shared_pointer pvIndex = pvStructure->getSubField<epics::pvData::PVInt>("index");
        if (!pvIndex) {
            throw PvaException("enum_t structure does not have 'int index' field");
        }
        epics::pvData::PVStringArray::shared_pointer pvChoices = pvStructure->getSubField<epics::pvData::PVStringArray>("choices");
        if (!pvChoices) {
            throw PvaException("enum_t structure does not have 'string choices[]' field");
        }

        epics::pvData::PVStringArray::const_svector choices(pvChoices->view());
        // Assume "AutoEnum" mode
        epics::pvData::shared_vector<std::string>::const_iterator it = std::find(choices.begin(), choices.end(), from[fromStartIndex]);
        if (it != choices.end()) {
            index = static_cast<epics::pvData::int32>(it - choices.begin());
        }
        
        if (index == -1) {
            std::istringstream iss(from[fromStartIndex]);
            iss >> index;
            // not fail and entire value is parsed (e.g. to detect 1.2 parsing to 1)
            if (iss.fail() || !iss.eof()) {
                throw PvaException("enum value '" + from[fromStartIndex] + "' invalid");
            }
        }
        if (index < 0 || index >= static_cast<epics::pvData::int32>(choices.size())) {
            throw PvaException("index '" + from[fromStartIndex] + "' out of bounds");
        }
        pvIndex->put(index);
        return 1;
    }

    size_t processed = 0;
    size_t fromValueCount = from.size();

    const epics::pvData::PVFieldPtrArray& fieldsData = pvStructure->getPVFields();
    if (fieldsData.size() != 0) {
        size_t length = pvStructure->getStructure()->getNumberFields();
        for(size_t i = 0; i < length; i++) {
            epics::pvData::PVFieldPtr fieldField = fieldsData[i];

            try {
                epics::pvData::Type type = fieldField->getField()->getType();
                switch (type) {
                    case epics::pvData::structure: {
                        epics::pvData::PVStructurePtr pv = std::tr1::static_pointer_cast<epics::pvData::PVStructure>(fieldField);
                        size_t count = fromString(pv, from, fromStartIndex);
                        processed += count;
                        fromStartIndex += count;
                        break;
                    }
                    case epics::pvData::scalarArray: {
                        epics::pvData::PVScalarArrayPtr pv = std::tr1::static_pointer_cast<epics::pvData::PVScalarArray>(fieldField);
                        size_t count = fromString(pv, from, fromStartIndex);
                        processed += count;
                        fromStartIndex += count;
                        break;
                    }
                    case epics::pvData::scalar: {
                	    if (fromStartIndex >= fromValueCount) {
		                    throw InvalidArgument("Not enough values in input array.");
                        }

            	        epics::pvData::PVScalarPtr pv = std::tr1::static_pointer_cast<epics::pvData::PVScalar>(fieldField);
                        epics::pvData::getConvert()->fromString(pv, from[fromStartIndex++]);
                        processed++;
                        break;
                    }
                    case epics::pvData::structureArray: {
                        epics::pvData::PVStructureArrayPtr pv = std::tr1::static_pointer_cast<epics::pvData::PVStructureArray>(fieldField);
                        size_t count = fromString(pv, from, fromStartIndex);
                        processed += count;
                        fromStartIndex += count;
                        break;
                    }
                    default: {
                        // union/unionArray not supported
                        std::ostringstream oss;
                        oss << "Unsupported field type: " << type;
                        throw PvaException(oss.str());
                    }
                }
            }
            catch (const PvaException& ex) {
                throw;
            }
            catch (const std::exception& ex) {
                std::ostringstream os;
                os << "Failed to parse '" << fieldField->getField()->getID() << ' ' << fieldField->getFieldName() << "'";
                os << ": " << ex.what();
                throw PvaException(os.str());
            }
        }
    }

    return processed;
}
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430

}
