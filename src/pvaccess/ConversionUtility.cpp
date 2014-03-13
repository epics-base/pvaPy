#include "ConversionUtility.h"
#include "InvalidRequest.h"

// Scalar array utilities
epics::pvData::ScalarType ConversionUtility::getScalarArrayElementType(epics::pvData::PVStructurePtr pvStructurePtr, const std::string& key)
{
    epics::pvData::PVFieldPtr pvFieldPtr = pvStructurePtr->getSubField(key);
    if (!pvFieldPtr) {
        throw InvalidRequest("Object does not have scalar array field " + key);
    }
    epics::pvData::FieldConstPtr fieldPtr = pvFieldPtr->getField();
    epics::pvData::Type type = fieldPtr->getType();
    if (type != epics::pvData::scalarArray) {
        throw InvalidRequest("Object does not have scalar array field " + key);
    }
    epics::pvData::ScalarArrayConstPtr scalarArrayPtr = std::tr1::static_pointer_cast<const epics::pvData::ScalarArray>(fieldPtr);
    scalarType = scalarArrayPtr->getElementType();
    return scalarType;
}









