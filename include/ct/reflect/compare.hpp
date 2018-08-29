#pragma once

namespace ct
{
    struct Equal
    {
        template<class T>
        bool test(const char* /*name*/, const T& lhs, const T& rhs) const
        {
            return lhs == rhs;
        }
    };
    template<class T, class Comparator = Equal>
    bool compare(const T& lhs, const T& rhs, const Comparator& = Comparator());

}
#include "compare-inl.hpp"
