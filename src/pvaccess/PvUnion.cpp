#include "PvUnion.h"

boost::python::dict PvUnion::createStructureDict(PvType::UnionType unionType)
{
    boost::python::dict pyDict;
    pyDict[ValueFieldKey] = unionType;
    return pyDict;
}

PvUnion::PvUnion()
    : PvObject(createStructureDict(PvType::VARIANT_)),
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

