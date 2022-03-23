#ifndef CT_COMPILE_TIME_STRING_HPP
#define CT_COMPILE_TIME_STRING_HPP
#include "Indexer.hpp"
#include "StringView.hpp"

#include <cstring>

namespace ct
{
    template <size_t N>
    struct CompileTimeString
    {
        template <class... T>
        constexpr CompileTimeString(T... vals) : data{vals...}
        {
        }

        constexpr CompileTimeString() = default;

        template <size_t N1>
        constexpr CompileTimeString<N + N1 - 1> operator+(CompileTimeString<N1> other) const
        {
            return appendImpl(other, makeIndexSequence<N - 1>{}, makeIndexSequence<N1>{});
        }

        constexpr size_t size() const { return N; }

        char data[N] = {};

        template <size_t N1, size_t... I1, size_t... I2>
        constexpr CompileTimeString<N + N1 - 1>
        appendImpl(CompileTimeString<N1> other, IndexSequence<I1...>, IndexSequence<I2...>) const
        {
            return {data[I1]..., other.data[I2]...};
        }
    };

    template <size_t N, size_t... I>
    constexpr CompileTimeString<N> makeCTS(const char (&str)[N], IndexSequence<I...>)
    {
        return {str[I]...};
    }

    template <size_t N, size_t... I>
    constexpr CompileTimeString<N> makeCTS(StringView str, IndexSequence<I...>)
    {
        return {str[I]...};
    }

    template <size_t N>
    constexpr CompileTimeString<N> makeCTS(const char (&str)[N])
    {
        return makeCTS(str, makeIndexSequence<N>{});
    }

    template <size_t N>
    constexpr CompileTimeString<N> makeCTS(StringView str)
    {
        return makeCTS<N>(str, makeIndexSequence<N - 1>{});
    }
} // namespace ct

#endif