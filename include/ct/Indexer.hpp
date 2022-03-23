#ifndef CT_INDEXER_HPP
#define CT_INDEXER_HPP
#include "types.hpp"
#include <cstring>

namespace ct
{
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

    template <size_t... I>
    struct IndexSequence
    {
        template <size_t N>
        using append = IndexSequence<I..., N>;

        template <size_t N>
        using preppend = IndexSequence<N, I...>;
    };

    template <size_t N, size_t START = 0, size_t IDX = N>
    struct MakeIndexSequence : MakeIndexSequence<N - 1, START, IDX - 1>
    {
        using Super_t = MakeIndexSequence<N - 1, START, IDX - 1>;
        using type = typename Super_t::type::template append<Super_t::NEXT>;
        static constexpr const size_t NEXT = Super_t::NEXT + 1;
    };

    template <size_t N, size_t START>
    struct MakeIndexSequence<N, START, 0>
    {
        using type = IndexSequence<START>;
        static constexpr const size_t NEXT = START + 1;
    };

    template <size_t N, size_t START = 0>
    using makeIndexSequence = typename MakeIndexSequence<N - 1, START>::type;
} // namespace ct

#endif // CT_INDEXER_HPP
