// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PVA_CONSTANTS_H
#define PVA_CONSTANTS_H

#include <string>

#define PVA_DEFAULT_KEY "__default__"

#define PVA_STRUCTURE_ID "structure"

#define PVA_ALL_FIELDS_REQUEST "field()"
#define PVA_FIELD_VALUE_REQUEST "field(value)"
#define PVA_FIELD_VALUE_ALARM_TIMESTAMP_REQUEST "field(value,alarm,timeStamp)"
#define PVA_PUT_GET_FIELD_VALUE_REQUEST "putField(value)getField(value)"
#define PVA_PUT_GET_ALL_FIELDS_REQUEST "putField()getField()"
#define PVA_VALUE_FIELD_KEY "value"

class PvaConstants
{
public:
    static const std::string DefaultKey;

    static const std::string StructureId;

    static const std::string AllFieldsRequest;
    static const std::string FieldValueRequest;
    static const std::string FieldValueAlarmTimestampRequest;
    static const std::string PutGetAllFieldsRequest;
    static const std::string PutGetFieldValueRequest;

    static const std::string ValueFieldKey;
}; 

#endif

