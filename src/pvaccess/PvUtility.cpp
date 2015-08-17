
#include <string>
#include <istream>
#include <sstream>

#include "PvUtility.h"
#include "InvalidArgument.h"
#include "pv/convert.h"

namespace PvUtility 
{

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

}
