#pragma once


// forces a compile time value
template<int I> struct ctv
{
    enum { value = I };
};


#ifdef DEBUG_CONSTEXPR_OUTPUT
#include <iostream>
#define STATIC_EQUAL(LHS, RHS) if(ctv<(LHS)>::value != ctv<(RHS)>::value) {std::cout << #LHS << " (" << LHS << ")"  << " != " << #RHS << " (" << RHS << ")";}
#define STATIC_EQUAL(LHS, RHS) if(ctv<(LHS)>::value == ctv<(RHS)>::value) {std::cout << #LHS << " (" << LHS << ")"  << " == " << #RHS << " (" << RHS << ")";}
#else
#define STATIC_EQUAL(LHS, RHS) static_assert(ctv<(LHS)>::value == ctv<(RHS)>::value, #LHS " != " #RHS)
#define STATIC_NE(LHS, RHS) static_assert(ctv<(LHS)>::value != ctv<(RHS)>::value, #LHS " == " #RHS)

#endif
