#ifndef CT_INDEXER_HPP
#define CT_INDEXER_HPP
#include "types.hpp"

namespace ct
{
    using index_t = int16_t;
    template <index_t N>
    struct Indexer
    {
        static constexpr const index_t index = N;
        constexpr Indexer() = default;
        constexpr Indexer<N - 1> operator--() const { return Indexer<N - 1>{}; }

        constexpr Indexer<N + 1> operator++() const { return Indexer<N + 1>{}; }
        constexpr operator index_t() const { return N; }
    };

    template <>
    struct Indexer<0U>
    {
        static constexpr const index_t index = 0;
        constexpr Indexer() = default;
        constexpr Indexer<1U> operator++() const { return Indexer<1U>{}; }
        constexpr operator index_t() const { return 0U; }
    };
}

#endif // CT_INDEXER_HPP
