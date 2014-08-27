#ifndef PY_PV_DATA_UTILITY_H
#define PY_PV_DATA_UTILITY_H

#include <string>
#include "pv/pvData.h"
#include "boost/python/str.hpp"
#include "boost/python/extract.hpp"
#include "boost/python/object.hpp"
#include "boost/python/list.hpp"
#include "boost/python/dict.hpp"

#include "PyUtility.h"
#include "InvalidDataType.h"

namespace PyPvDataUtility
{

//
// Checks
//
void checkFieldExists(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

//
// Field retrieval
//
epics::pvData::FieldConstPtr getField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::ScalarConstPtr getScalarField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::PVScalarArrayPtr getScalarArrayField(const std::string& fieldName, epics::pvData::ScalarType scalarType, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::StructureConstPtr getStructure(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::PVStructurePtr getStructureField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::PVStructureArrayPtr getStructureArrayField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::PVBooleanPtr getBooleanField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::PVBytePtr getByteField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::PVUBytePtr getUByteField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::PVShortPtr getShortField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::PVUShortPtr getUShortField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::PVIntPtr getIntField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::PVUIntPtr getUIntField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::PVLongPtr getLongField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::PVULongPtr getULongField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::PVFloatPtr getFloatField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::PVDoublePtr getDoubleField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::PVStringPtr getStringField(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

//
// Field type retrieval
//
epics::pvData::Type getFieldType(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::ScalarType getScalarType(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

epics::pvData::ScalarType getScalarArrayType(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr);

//
// Conversion PY object => PV Scalar
//
void pyObjectToScalarField(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr);

//
// Conversion PY object => PV Scalar Array
//
void pyObjectToScalarArrayField(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr);

//
// Conversion PY object => PV Structure
//
void pyObjectToStructureField(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr);

//
// Conversion PY object => PV Structure Array
//
void pyObjectToStructureArrayField(const boost::python::object& pyObject, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr);

//
// Conversion PY [] => PV Scalar Array
//
void pyListToScalarArrayField(const boost::python::list& pyList, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr);

//
// Conversion PV Scalar Array => PY []
//
void scalarArrayFieldToPyList(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::list& pyList);

//
// Conversion PY {} => PV Structure
//
void pyDictToStructure(const boost::python::dict& pyDict, epics::pvData::PVStructurePtr& pvStructurePtr);

void pyDictToStructureField(const boost::python::dict& pyDict, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr);

//
// Conversion PY [{}] => PV Structure Array
//
void pyListToStructureArrayField(const boost::python::list& pyList, const std::string& fieldName, epics::pvData::PVStructurePtr& pvStructurePtr);

//
// Conversion PV Structure Array => PY [{}] 
//
void structureArrayFieldToPyList(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::list& pyList);

//
// Conversion PV Structure => PY {}
//
void structureToPyDict(const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict);

void structureFieldToPyDict(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict);

//
//
// Add PV Scalar => PY {}
// 
void addScalarFieldToDict(const std::string& fieldName, epics::pvData::ScalarType scalarType, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict);

//
// Add PV Scalar Array => PY {}
// 
void addScalarArrayFieldToDict(const std::string& fieldName, epics::pvData::ScalarType scalarType, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict);

//
// Add PV Structure => PY {}
// 
void addStructureFieldToDict(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict);

//
// Add PV Structure Array => PY {}
// 
void addStructureArrayFieldToDict(const std::string& fieldName, const epics::pvData::PVStructurePtr& pvStructurePtr, boost::python::dict& pyDict);

//
// Conversion Structure => PY {}
//
void structureToPyDict(const epics::pvData::StructureConstPtr& structurePtr, boost::python::dict& pyDict);

//
// Copy PV Structure => PV Structure
//
void copyStructureToStructure(const epics::pvData::PVStructurePtr& srcPvStructurePtr, epics::pvData::PVStructurePtr& destPvStructurePtr);
void copyStructureToStructure(const std::string& fieldName, const epics::pvData::PVStructurePtr& srcPvStructurePtr, epics::pvData::PVStructurePtr& destPvStructurePtr);

//
// Copy PV Structure Array => PV Structure
//
void copyStructureArrayToStructure(const std::string& fieldName, const epics::pvData::PVStructurePtr& srcPvStructurePtr, epics::pvData::PVStructurePtr& destPvStructurePtr);

// 
// Copy PV Scalar to PV Structure
//
void copyScalarToStructure(const std::string& fieldName, epics::pvData::ScalarType scalarType, const epics::pvData::PVStructurePtr& srcPvStructurePtr, epics::pvData::PVStructurePtr& destPvStructurePtr);

// 
// Copy PV Scalar Array to PV Structure
//
void copyScalarArrayToStructure(const std::string& fieldName, epics::pvData::ScalarType scalarType, const epics::pvData::PVStructurePtr& srcPvStructurePtr, epics::pvData::PVStructurePtr& destPvStructurePtr);

//
//
// Template implementations
//

#if defined PVA_API_VERSION && PVA_API_VERSION == 430
template<typename PvArrayType, typename CppType, typename PyType>
void pyListToScalarArrayField(const boost::python::list& pyList, const std::string& fieldName, epics::pvData::ScalarType scalarType, epics::pvData::PVStructurePtr pvStructurePtr)
{
    int listSize = boost::python::len(pyList);
    std::tr1::shared_ptr<PvArrayType> valueArray = std::tr1::static_pointer_cast<PvArrayType>(pvStructurePtr->getScalarArrayField(fieldName, scalarType));
    std::vector<CppType> v(listSize);
    for (int i = 0; i < listSize; i++) {
        boost::python::extract<PyType> valueExtract(pyList[i]);
        if (valueExtract.check()) {
            v[i] = valueExtract();
        }
        else {
            throw InvalidDataType("Invalid data type for element %d", i);
        }
    }
    valueArray->setCapacity(listSize);
    valueArray->put(0, listSize, v, 0);
}
#else
template<typename PvArrayType, typename CppType, typename PyType>
void pyListToScalarArrayField(const boost::python::list& pyList, const std::string& fieldName, epics::pvData::ScalarType scalarType, epics::pvData::PVStructurePtr pvStructurePtr)
{
    int listSize = boost::python::len(pyList);
    std::tr1::shared_ptr<PvArrayType> valueArray = std::tr1::static_pointer_cast<PvArrayType>(pvStructurePtr->getScalarArrayField(fieldName, scalarType));
    typename PvArrayType::svector v(listSize);
    for (int i = 0; i < listSize; i++) {
        boost::python::extract<PyType> valueExtract(pyList[i]);
        if (valueExtract.check()) {
            v[i] = valueExtract();
        }
        else {
            throw InvalidDataType("Invalid data type for element %d", i);
        }
    }
    valueArray->setCapacity(listSize);
    valueArray->replace(freeze(v));
}
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430

#if defined PVA_API_VERSION && PVA_API_VERSION == 430
template<typename PvArrayType, typename PvArrayDataType>
void scalarArrayToPyList(const epics::pvData::PVScalarArrayPtr& pvScalarArrayPtr, boost::python::list& pyList) 
{
    int nDataElements = pvScalarArrayPtr->getLength();
    PvArrayDataType arrayData;
    std::tr1::static_pointer_cast<PvArrayType>(pvScalarArrayPtr)->get(0, nDataElements, arrayData);
    for (int i = 0; i < nDataElements; ++i) {
        pyList.append(arrayData.data[i]);
    }
}
#else
template<typename PvArrayType, typename CppType>
void scalarArrayToPyList(const epics::pvData::PVScalarArrayPtr& pvScalarArrayPtr, boost::python::list& pyList) 
{
    int nDataElements = pvScalarArrayPtr->getLength();
    typename PvArrayType::const_svector data;
    pvScalarArrayPtr->PVScalarArray::template getAs<CppType>(data);
    for (int i = 0; i < nDataElements; ++i) {
        pyList.append(data[i]);
    }
}
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430


#if defined PVA_API_VERSION && PVA_API_VERSION == 430
template<typename PvArrayType, typename PvArrayDataType>
void copyScalarArrayToScalarArray(const epics::pvData::PVScalarArrayPtr& srcPvScalarArrayPtr, epics::pvData::PVScalarArrayPtr& destPvScalarArrayPtr)
{
    int nDataElements = srcPvScalarArrayPtr->getLength();
    PvArrayDataType srcArrayData;
    std::tr1::static_pointer_cast<PvArrayType>(srcPvScalarArrayPtr)->get(0, nDataElements, srcArrayData);
    destPvScalarArrayPtr->setCapacity(nDataElements);
    std::tr1::static_pointer_cast<PvArrayType>(destPvScalarArrayPtr)->put(0, nDataElements, srcArrayData.data, 0);
}
#else
template<typename PvArrayType, typename CppType>
void copyScalarArrayToScalarArray(const epics::pvData::PVScalarArrayPtr& srcPvScalarArrayPtr, epics::pvData::PVScalarArrayPtr& destPvScalarArrayPtr)
{
    int nDataElements = srcPvScalarArrayPtr->getLength();
    typename PvArrayType::const_svector data;
    srcPvScalarArrayPtr->PVScalarArray::template getAs<CppType>(data);

    destPvScalarArrayPtr->setCapacity(nDataElements);
    destPvScalarArrayPtr->putFrom(data);
}
#endif // if defined PVA_API_VERSION && PVA_API_VERSION == 430

} // namespace PyPvDataUtility

#endif

