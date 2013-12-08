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

/*
void PvObject::setScalarArray(const std::string& key, const boost::python::list& pyList)
{
    epics::pvData::ScalarType scalarType;
    epics::pvData::ScalarArrayConstPtr scalarArrayPtr = getScalarArrayPtr(key, scalarType);
    int listSize = boost::python::len(pyList);
    switch (scalarType) {
        case epics::pvData::pvBoolean: {
            epics::pvData::PVBooleanArrayPtr valueArray = std::tr1::static_pointer_cast<epics::pvData::PVBooleanArray>(pvStructurePtr->getScalarArrayField(key, scalarType));
            valueArray->setCapacity(listSize);
            std::cout << "LIST SIZE " << listSize << std::endl;
            std::vector<unsigned char> v(listSize);
            for (int i = 0; i < listSize; i++) {
                boost::python::extract<bool> boolExtract(pyList[i]);
                if (boolExtract.check()) {
                    std::cout << "SETTING " << i << ": " << boolExtract() << std::endl;
                    v.push_back(boolExtract());
                }
                else {
                    throw InvalidDataType("Invalid data type for element %d", i);
                }
            }
            valueArray->put(0, listSize, v, 0);
            std::cout << "VALUE ARR: " << *valueArray << std::endl;
            break;
        }








