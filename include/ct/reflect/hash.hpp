#pragma once
#include <cstdint>
#include <ct/Hash.hpp>

#define DECL_HASHED_TYPE(TYPE) template<> struct TypeHash<TYPE, void>{ static constexpr const char* name = #TYPE; static constexpr const uint32_t value = crc32(name); }

namespace ct
{

    template<class T, class Enable = void>
    struct TypeHash
    {

    };

    // same for this type so long as the structure is the same
    template<class T>
    constexpr uint32_t hashStruct();

    // same for this type if the values of all accessible data members is the same
    template<class T>
    uint32_t hashValues(const T& data);

    // hash of a types data member datatypes and names, such that two types can be checked for equivalence.
    // IE struct Vec{float x,y,z;};
    //    struct Point{float x,y,z;};
    //    hashMembers<Vec>() == hashMembers<Point>();
    template<class T>
    constexpr uint32_t hashMembers();

    template<class T>
    uint32_t hashMemberValues(const T& data);

    DECL_HASHED_TYPE(float);
    DECL_HASHED_TYPE(int32_t);
    DECL_HASHED_TYPE(double);
    DECL_HASHED_TYPE(uint32_t);
    DECL_HASHED_TYPE(int16_t);
    DECL_HASHED_TYPE(uint16_t);
    DECL_HASHED_TYPE(uint8_t);
    DECL_HASHED_TYPE(int8_t);
}

#include "hash-inl.hpp"
