#pragma once
#include <cstdint>
namespace ct
{
    using index_t = uint8_t;
    template <index_t N>
    struct Indexer
    {
        static constexpr const index_t index = N;
        constexpr Indexer() = default;
        constexpr Indexer<N - 1> operator--() const { return Indexer<N - 1>{}; }

        constexpr Indexer<N + 1> operator++() const { return Indexer<N + 1>{}; }
    };

    template<>
    struct Indexer<0U>
    {
        static constexpr const index_t index = 0;
        constexpr Indexer() = default;
        constexpr Indexer<1U> operator++() const { return Indexer<1U>{}; }
    };
}
