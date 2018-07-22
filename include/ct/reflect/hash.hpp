#pragma once
#include <cstdint>

namespace ct
{
    // same for this type so long as the structure is the same
    template<class T>
    uint32_t hashType();

    // same for this type if the values of all accessible data members is the same
    template<class T>
    uint32_t hashValues(const T& data);

    // hash of a types data member datatypes and names, such that two types can be checked for equivalence.
    // IE struct Vec{float x,y,z;};
    //    struct Point{float x,y,z;};
    //    hashMembers<Vec>() == hashMembers<Point>();
    template<class T>
    uint32_t hashMembers();

    template<class T>
    uint32_t hashMemberValues(const T& data);

}

#include "hash-inl.hpp"
