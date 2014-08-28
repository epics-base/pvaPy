
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
    size_t processed = 0;
    size_t fromValueCount = from.size();

    const epics::pvData::PVFieldPtrArray& fieldsData = pvStructure->getPVFields();
    if (fieldsData.size() != 0) {
        size_t length = pvStructure->getStructure()->getNumberFields();
        for(size_t i = 0; i < length; i++) {
            epics::pvData::PVFieldPtr fieldField = fieldsData[i];

            try {
                epics::pvData::Type type = fieldField->getField()->getType();
                // TODO union/unionArray support
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
