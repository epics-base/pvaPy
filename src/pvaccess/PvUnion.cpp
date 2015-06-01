#include "boost/python/extract.hpp"
#include "PvUnion.h"

boost::python::dict PvUnion::createVariantUnionStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = boost::python::make_tuple();
    return pyDict;
}

boost::python::dict PvUnion::createUnionStructureDict(const boost::python::dict& pyDict)
{
    // Do not duplicate value key, if one is provided, if it is a single
    // dictionary key, and if it holds dictionary or tuple holding dictionary
    boost::python::dict pyDict2;
    if (pyDict.has_key(ValueFieldKey) && boost::python::len(pyDict) == 1) {
        boost::python::object pyObject = pyDict[ValueFieldKey];
        boost::python::extract<boost::python::tuple> tupleExtract(pyObject);

        // Look for dict inside tuple
        if (tupleExtract.check()) {
            boost::python::tuple pyTuple = tupleExtract();
            if (boost::python::len(pyTuple) == 1) {
                boost::python::extract<boost::python::dict> dictExtract(pyTuple[0]);
                if (dictExtract.check()) {
                    pyDict2[ValueFieldKey] = boost::python::make_tuple(dictExtract());
                    return pyDict2;
                }
            }
        }

        // Look for dict
        boost::python::extract<boost::python::dict> dictExtract(pyObject);
        if (dictExtract.check()) {
            pyDict2[ValueFieldKey] = boost::python::make_tuple(dictExtract());
            return pyDict2;
        }
    }

    // We could not find union in the structure, simply use provided dict
    pyDict2[ValueFieldKey] = boost::python::make_tuple(pyDict);
    return pyDict2;
}

PvUnion::PvUnion()
    : PvObject(createVariantUnionStructureDict()),
    unionPtr(epics::pvData::getFieldCreate()->createVariantUnion())
{
    dataType = PvType::Variant;
}

PvUnion::PvUnion(const boost::python::dict& structureDict)
    : PvObject(createUnionStructureDict(structureDict))
{
    dataType = PvType::Union;
}

PvUnion::PvUnion(const PvObject& pvObject)
    : PvObject(createUnionStructureDict(const_cast<PvObject*>(&pvObject)->getStructureDict())),
    unionPtr(epics::pvData::getFieldCreate()->createUnion(
        pvObject.getPvStructurePtr()->getStructure()->getFieldNames(), 
        pvObject.getPvStructurePtr()->getStructure()->getFields()))
{
    dataType = PvType::Union;
}

PvUnion::~PvUnion()
{
}

