#include <ct/String.hpp>
#include <type_traits>
#include <iostream>

static constexpr const char* getString() { return "asdf"; }

template<int I> struct ctv
{ 
    enum{value = I}; 
};

#ifdef DEBUG_CONSTEXPR_OUTPUT
    #define STATIC_EQUAL(LHS, RHS) if(ctv<(LHS)>::value != ctv<(RHS)>::value) {std::cout << #LHS << " (" << LHS << ")"  << " != " << #RHS << " (" << RHS << ")";}
#else
    #define STATIC_EQUAL(LHS, RHS) static_assert(ctv<(LHS)>::value == ctv<(RHS)>::value, #LHS " != " #RHS)
#endif

int main()
{
    STATIC_EQUAL(ct::strLen("asdf"), 4);
    STATIC_EQUAL(ct::strLen(getString()), 4);

    STATIC_EQUAL(ct::findFirst("asdf:asdf"), 4);
    STATIC_EQUAL(ct::findLast("asdf:asdf"), 4);
    STATIC_EQUAL(ct::findLast("asdf:asdf", 'a'), 5);
    STATIC_EQUAL("asdf:asdf"[4],  ':');
    STATIC_EQUAL(ct::classNameIdx("TestHash::getName()"), 8);
    
    return 0;
}