// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "PvEnum.h"
#include "PvType.h"
#include "PyPvDataUtility.h"
#include "PyUtility.h"
#include "StringUtility.h"

namespace pvd = epics::pvData;
namespace bp = boost::python;

const char* PvEnum::StructureId("enum_t");

const char* PvEnum::IndexFieldKey("index");
const char* PvEnum::ChoicesFieldKey("choices");
const int PvEnum::UnknownChoice(-1);

bp::dict PvEnum::createStructureDict()
{
    bp::dict structureDict;
    structureDict[IndexFieldKey] = PvType::Int;
    boost::python::list choicesList;
    choicesList.append(PvType::String);
    structureDict[ChoicesFieldKey] = choicesList;
    return structureDict;
}

PvEnum::PvEnum()
    : PvObject(createStructureDict(), StructureId)
    , maxIndex(UnknownChoice)
{
    pvStructurePtr->getSubField<pvd::PVInt>(IndexFieldKey)->put(UnknownChoice);
}

PvEnum::PvEnum(const bp::list& choices, int index)
    : PvObject(createStructureDict(), StructureId)
    , maxIndex(UnknownChoice)
{
    setChoices(choices);
    setIndex(index);
}

PvEnum::PvEnum(const bp::dict& pyDict, const std::string& structureId)
    : PvObject(pyDict, structureId)
    , maxIndex(UnknownChoice)
{
    setMaxIndex();
}

PvEnum::PvEnum(const PvEnum& pvEnum)
    : PvObject(pvEnum.pvStructurePtr)
    , maxIndex(pvEnum.maxIndex)
{
}

PvEnum::PvEnum(const pvd::PVStructurePtr& pvStructurePtr)
    : PvObject(pvStructurePtr)
{
    setMaxIndex();
}

PvEnum::~PvEnum()
{
}

void PvEnum::setIndex(int index)
{
    if (index < 0 || index > maxIndex) {
        throw InvalidArgument("Index value cannot be negative or be larger than " + StringUtility::toString(maxIndex) + ".");
    }
    pvStructurePtr->getSubField<pvd::PVInt>(IndexFieldKey)->put(index);
}

int PvEnum::getIndex() const
{
    return pvStructurePtr->getSubField<pvd::PVInt>(IndexFieldKey)->get();
}

void PvEnum::setChoices(const bp::list& choices)
{
    PyPvDataUtility::pyListToScalarArrayField(choices, ChoicesFieldKey, pvStructurePtr);
    setMaxIndex();
    setIndex(0);
}

bp::list PvEnum::getChoices() const
{
    bp::list choices;
    PyPvDataUtility::scalarArrayFieldToPyList(ChoicesFieldKey, pvStructurePtr, choices);
    return choices;
}

void PvEnum::setMaxIndex()
{
   bp::list choices = getChoices();
   maxIndex = bp::len(choices)-1;
}

std::string PvEnum::getCurrentChoice() const
{
    int index = getIndex();
    if (index < 0) {
        return "";
    }
    bp::list choices = getChoices();
    return PyUtility::extractStringFromPyObject(choices[index]);
}

