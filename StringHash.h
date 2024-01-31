#ifndef STRINGHASH_H
#define STRINGHASH_H
#include  <type_traits>
#include <cstddef>
#include <utility>
#include <stddef.h>
#include <cstring>
#include <iostream>
#include <cstdint>
#include <string>
inline uint32_t hashbit(const void* data, size_t len) 
{
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    uint32_t hash_value = 0;
    
    for (size_t i = 0; i < len; ++i) 
    {
        hash_value = ((hash_value << 5) + hash_value) ^ bytes[i];
    }
    
    return hash_value;
}

class StringHash
{
public:
    StringHash()
        :m_value(0)
    {

    }
    StringHash(const StringHash& rhs) noexcept
        :m_value(std::move(rhs.m_value))
    {

    }
    StringHash(size_t value)
        :m_value(value)
    {

    }
    StringHash(const char* str);
    StringHash(const std::string& str);

    StringHash& operator=(StringHash&& rhs) noexcept
    {
        m_value = std::move(rhs.m_value);
        return *this;
    }

    StringHash operator +(const StringHash& rhs)  noexcept
    {
        StringHash ret;
        ret.m_value = m_value + rhs.m_value;
        return ret;
    }
    StringHash& operator +=(const StringHash& rhs) noexcept
    {
        m_value += rhs.m_value;
        return *this;
    }
    bool operator ==(const StringHash& rhs)const { return m_value == rhs.m_value; }

    bool operator !=(const StringHash& rhs)const { return m_value != rhs.m_value; }
    
    bool operator <(const StringHash& rhs)const { return m_value < rhs.m_value; }
    
    bool operator >(const StringHash& rhs)const { return m_value > rhs.m_value; }
     
    explicit operator bool() const { return m_value != 0; }

    size_t value() const { return m_value; }

    std::string toString() const;

    size_t toHash() const { return m_value; }

    static const StringHash ZERO;
private:
    size_t m_value;
};
inline size_t Hash(const StringHash& hash) noexcept
{
    return static_cast<size_t>( hash.value() );
}


#endif //STRINGHASH_H