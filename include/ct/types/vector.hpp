#ifndef CT_TYPES_VECTOR_HPP
#define CT_TYPES_VECTOR_HPP
#include "../reflect.hpp"
#include "TArrayView.hpp"

#include <vector>

namespace ct
{
    template <class T, class A>
    struct ReflectImpl<std::vector<T, A>, void>
    {
        using DataType = std::vector<T, A>;
        using this_t = ReflectImpl<DataType, void>;
        static constexpr StringView getTypeName() { return GetName<DataType>::getName(); }

        static TArrayView<const T> getData(const DataType& data)
        {
            return TArrayView<const T>(data.data(), data.size());
        }

        static TArrayView<T> getDataMutable(DataType& data) { return TArrayView<T>(data.data(), data.size()); }

        static void resize(DataType& data, size_t sz) { data.resize(sz); }

        REFLECT_STUB
            PROPERTY(size, &DataType::size, &this_t::resize)
            PROPERTY(data, &this_t::getData, &this_t::getDataMutable)
        REFLECT_INTERNAL_END;
        static constexpr Indexer<NUM_FIELDS - 1> end() { return Indexer<NUM_FIELDS - 1>(); }
    };
} // namespace ct

#endif // CT_TYPES_VECTOR_HPP