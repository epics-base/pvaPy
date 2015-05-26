#include "PvUnion.h"

boost::python::dict PvUnion::createVariantUnionStructureDict()
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = boost::python::make_tuple();
    return pyDict;
}

PvUnion::PvUnion()
    : PvObject(createVariantUnionStructureDict()),
    unionPtr(epics::pvData::getFieldCreate()->createVariantUnion())
{
    dataType = PvType::Variant;
}

PvUnion::PvUnion(const PvObject& pvObject)
    : PvObject(pvObject),
    unionPtr(epics::pvData::getFieldCreate()->createUnion(
        pvObject.getPvStructurePtr()->getStructure()->getFieldNames(), 
        pvObject.getPvStructurePtr()->getStructure()->getFields()))
{
    dataType = PvType::Union;
}

PvUnion::~PvUnion()
{
}

