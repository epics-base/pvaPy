// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/extract.hpp"
#include "PvUnion.h"
#include "PyPvDataUtility.h"

boost::python::dict PvUnion::createVariantUnionStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = boost::python::make_tuple();
    return pyDict;
}

boost::python::dict PvUnion::createUnionStructureDict(const boost::python::dict& pyDict)
{
    boost::python::dict pyDict2;
    pyDict2[ValueFieldKey] = boost::python::make_tuple(PyPvDataUtility::extractUnionStructureDict(pyDict));
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

