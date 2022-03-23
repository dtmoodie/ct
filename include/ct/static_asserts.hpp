#ifndef CT_STATIC_ASSERTS_HPP
#define CT_STATIC_ASSERTS_HPP
#include "Indexer.hpp"
#include "type_traits.hpp"

#include <type_traits>

namespace ct
{

    template <class LHS, class RHS>
    struct StaticEqualTypes
    {
        constexpr StaticEqualTypes() { static_assert(std::is_same<LHS, RHS>::value, "Types not equal"); }
    };

    template <class T, T LHS, T RHS>
    struct StaticEquality
    {
        constexpr StaticEquality() { static_assert(LHS == RHS, "values not equal"); }
    };

    template <class T, T LHS, T RHS>
    struct StaticInequality
    {
        constexpr StaticInequality() { static_assert(LHS != RHS, "values are equal"); }
    };

    template <class T, T LHS, T RHS>
    struct StaticGreater
    {
        constexpr StaticGreater() { static_assert(LHS > RHS, "values not greater"); }
    };

    template <index_t I, index_t START, index_t END>
    struct StaticInRange
    {
        static_assert(I >= START && I < END, "I must be in range");
    };

    template <class T, class U>
    struct StaticIsBase;

    template <class T, class U>
    struct StaticIsBase<Base<T>, Derived<U>>
    {
        static_assert(IsBase<Base<T>, Derived<U>>::value, "Must be a base");
    };
} // namespace ct
#endif
