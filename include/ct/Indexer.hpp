#pragma once
#include <cstdint>
namespace ct
{
    template <uint8_t N>
    struct Indexer
    {
        static constexpr const uint8_t index = N;
        constexpr Indexer<N - 1> operator--() const { return Indexer<N - 1>{}; }

        constexpr Indexer<N + 1> operator++() const { return Indexer<N + 1>{}; }
    };

    template<>
    struct Indexer<0>
    {
        static constexpr const uint8_t index = 0;
        constexpr Indexer<1> operator++() const { return Indexer<1>{}; }
    };
}
