#pragma once
#include "StringImpl.hpp"
#include <array>
#include <cstddef>
#include <ostream>
#include <type_traits>
namespace ct
{

    constexpr size_t strLen(const char* str, size_t len = 0);
    struct StringView
    {
        StringView(const std::string& str) : m_data(str.c_str()), m_size(str.size()) {}

        template <size_t N>
        constexpr StringView(const char (&data)[N]) : m_data(data), m_size(N)
        {
        }

        constexpr StringView(const char* data, const size_t n) : m_data(data), m_size(n) {}
        constexpr StringView(const char* data) : m_data(data), m_size(strLen(data)) {}

        constexpr operator const char*() const { return m_data; }
        operator std::string() const { return std::string(m_data, m_data + m_size); }
        const char* m_data;
        size_t m_size;
    };

    std::ostream& operator<<(std::ostream& os, const StringView& view)
    {
        for (size_t i = 0; i < view.m_size; ++i)
        {
            os << view.m_data[i];
        }
        return os;
    }

    template <size_t N>
    constexpr size_t strLen(const char (&str)[N])
    {
        return N;
    }

    constexpr size_t strLen(const StringView view) { return view.m_size; }

    constexpr size_t strLen(const char* str, size_t len) { return *str ? strLen(str + 1, len + 1) : len; }

    template <size_t N>
    constexpr size_t findFirst(const char (&str)[N], char character = ':')
    {
        return detail::findFirst(str, N, 0, character);
    }

    constexpr size_t findFirst(const char* str, char character = ':', size_t pos = 0)
    {
        return detail::findFirst(str, strLen(str), pos, character);
    }

    constexpr size_t
    findNthFromBeginning(const char* str, const size_t N, const char character = ':', const size_t pos = 0)
    {
        return N == 0 ? findFirst(str, character, pos)
                      : findNthFromBeginning(str, N - 1, character, findFirst(str, character, pos) + 1);
    }

    constexpr size_t countOccurances(const char* str, char character = ':', size_t count = 0)
    {
        return *str ? countOccurances(str + 1, character, *str == character ? count + 1 : count) : count;
    }

    template <std::size_t N>
    constexpr std::size_t findLast(const char (&str)[N], char character = ':')
    {
        return detail::ctFindDeliminator(str, N - 1, character);
    }

    constexpr std::size_t findLast(const StringView view, char character = ':')
    {
        return detail::ctFindDeliminator(view.m_data, view.m_size - 1, character);
    }

    constexpr std::size_t findLast(const char* str, char character = ':')
    {
        return detail::ctFindDeliminator(str, ct::strLen(str) - 1, character);
    }

    template <std::size_t Tlen>
    constexpr std::size_t classNameIdx(const char (&str)[Tlen])
    {
        return detail::ctFindDeliminator(str, Tlen - 1) - 2;
    }

    constexpr std::size_t classNameIdx(const char* str)
    {
#ifdef _MSC_VER
        return detail::ctFindDeliminator(str, strLen(str) - 1) - 1;
#else
        return detail::ctFindDeliminator(str, strLen(str) - 1) - 1;
#endif
    }

    // https://stackoverflow.com/questions/25195176/how-do-i-convert-a-c-string-to-a-int-at-compile-time
    constexpr bool isDigit(char c) { return c <= '9' && c >= '0'; }

    constexpr int stoiImplRange(const char* str, const char* end, int value = 0)
    {
        return str != end
                   ? isDigit(*str) ? stoiImplRange(str + 1, end, (*str - '0') + value * 10)
                                   : throw "compile-time-error: not a digit"
                   : value;
    }

    constexpr int stoiImpl(const char* str, int value = 0)
    {
        return *str
                   ? isDigit(*str) ? stoiImpl(str + 1, (*str - '0') + value * 10)
                                   : throw "compile-time-error: not a digit"
                   : value;
    }

    constexpr int stoi(const char* str) { return stoiImpl(str); }

    constexpr int stoiRange(const char* start, const char* end) { return stoiImplRange(start, end); }

    constexpr int stoiRange(const char* start, int len) { return stoiImplRange(start, start + len); }
} // namespace ct
