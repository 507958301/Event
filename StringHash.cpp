#include "StringHash.h"


StringHash::StringHash(const char* str)
    :m_value(hashbit(str, strlen(str)))   
{

}
StringHash::StringHash(const std::string& str)
    :m_value(hashbit(str.c_str(), str.size()))
{

}
std::string StringHash::toString() const
{
    std::string result ;
    std::sprintf(result.data(), "%08llX", m_value);
    return result;
}
