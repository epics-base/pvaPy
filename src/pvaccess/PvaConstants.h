// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PVA_CONSTANTS_H
#define PVA_CONSTANTS_H

#define PVA_ALL_FIELDS_REQUEST "field()"
#define PVA_FIELD_VALUE_REQUEST "field(value)"
#define PVA_PUT_GET_FIELD_VALUE_REQUEST "putField(value)getField(value)"
#define PVA_VALUE_FIELD_KEY "value"

class PvaConstants
{
public:
    static const char* AllFieldsRequest;
    static const char* FieldValueRequest;
    static const char* PutGetFieldValueRequest;

    static const char* ValueFieldKey;
}; 

#endif

