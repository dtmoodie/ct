#pragma once
#include <cstdint>

// forces a compile time value
template<uint32_t I> struct ctv
{
    static constexpr const uint32_t value = I;
};


#ifdef DEBUG_CONSTEXPR_OUTPUT
#include <iostream>
#define STATIC_EQUAL(LHS, RHS) if(ctv<(LHS)>::value != ctv<(RHS)>::value) {std::cout << #LHS << " (" << LHS << ")"  << " != " << #RHS << " (" << RHS << ")";}
#define STATIC_EQUAL(LHS, RHS) if(ctv<(LHS)>::value == ctv<(RHS)>::value) {std::cout << #LHS << " (" << LHS << ")"  << " == " << #RHS << " (" << RHS << ")";}
#else
#define STATIC_EQUAL(LHS, RHS) static_assert(ctv<(LHS)>::value == ctv<(RHS)>::value, #LHS " != " #RHS)
#define STATIC_NE(LHS, RHS) static_assert(ctv<(LHS)>::value != ctv<(RHS)>::value, #LHS " == " #RHS)

template<class T, T LHS, T RHS>
struct StaticEquality
{
    static_assert(LHS == RHS, "asdf");
};

#endif
