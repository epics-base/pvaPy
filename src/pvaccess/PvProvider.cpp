#include "PvProvider.h"
#include "InvalidArgument.h"

namespace PvProvider
{

const char* PvaProviderName("pva");
const char* CaProviderName("ca");

void checkProviderName(const std::string& providerName) 
{
    if (providerName != PvaProviderName && providerName != CaProviderName) {
        throw InvalidArgument("Unsupported provider name: %s. Allowed values are %s and %s.", providerName.c_str(), PvaProviderName, CaProviderName);
    }
}

std::string getProviderName(ProviderType providerType) 
{
    switch (providerType) {
        case PvaProviderType: {
            return PvaProviderName;
        }
        case CaProviderType: {
            return CaProviderName;
        }
        default: {
            throw InvalidArgument("Unsupported provider type: %d. Allowed types are %d (%s) and %d (%s).", providerType, PvaProviderType, PvaProviderName, CaProviderType, CaProviderName);
        }
    }
}

}

