#pragma once
#include <ct/reflect.hpp>
namespace ct
{
struct Equal
{
    template <class T>
    bool test(const char* /*name*/, const T& lhs, const T& rhs) const
    {
        return lhs == rhs;
    }
};

template <class T, class Comparator = Equal>
auto compare(const T& lhs, const T& rhs, const Comparator& cmp = Comparator()) -> ct::enable_if_reflected<T, bool>;
template <class T, class Comparator = Equal>
auto compare(const T& lhs, const T& rhs, const Comparator& cmp = Comparator()) -> ct::enable_if_not_reflected<T, bool>;
}
#include "compare-inl.hpp"
