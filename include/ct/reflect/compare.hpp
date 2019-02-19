#ifndef CT_REFLECT_COMPARE_HPP
#define CT_REFLECT_COMPARE_HPP
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
    auto compare(const T& lhs, const T& rhs, const Comparator& cmp = Comparator()) -> ct::EnableIfReflected<T, bool>;
    template <class T, class Comparator = Equal>
    auto compare(const T& lhs, const T& rhs, const Comparator& cmp = Comparator()) -> ct::DisableIfReflected<T, bool>;
}
#include "compare-inl.hpp"
#endif // CT_REFLECT_COMPARE_HPP
