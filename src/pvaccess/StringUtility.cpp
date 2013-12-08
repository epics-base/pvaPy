#include <algorithm>
#include "StringUtility.h"

namespace StringUtility
{

std::string toString(char* s) 
{
    if (s) {
        return std::string(s);
    }
    return "";
}

std::string toString(const char* s) 
{
    if (s) {
        return std::string(s);
    }
    return "";
}
std::string& leftTrim(std::string& s) 
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

std::string& rightTrim(std::string& s) 
{
    s.erase(std::find_if(s.rbegin(), s.rend(), 
        std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

std::string& trim(std::string& s) 
{
    return leftTrim(rightTrim(s));
}

std::string trim(const std::string& s) 
{
    std::string s2 = s;
    return leftTrim(rightTrim(s2));
}

} // namespace StringUtility
