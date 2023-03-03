#ifndef CT_TYPES_STL_PAIR_HPP
#define CT_TYPES_STL_PAIR_HPP

#include "../reflect.hpp"
#include "TArrayView.hpp"

#include <utility>

namespace ct
{
    template <class A, class B>
    struct ReflectImpl<std::pair<A, B>, void>
    {
        using DataType = std::pair<A, B>;
        using this_t = ReflectImpl<DataType, void>;
        static constexpr StringView getTypeName() { return GetName<DataType>::getName(); }

        REFLECT_STUB
            PUBLIC_ACCESS(first)
            PUBLIC_ACCESS(second)
        REFLECT_INTERNAL_END;
        static constexpr Indexer<NUM_FIELDS - 1> end() { return Indexer<NUM_FIELDS - 1>(); }
    };
} // namespace ct

#endif // CT_TYPES_STL_PAIR_HPP
