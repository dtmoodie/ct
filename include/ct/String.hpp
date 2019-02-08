#pragma once
#include "StringImpl.hpp"
#include <array>
#include <cstddef>
#include <ostream>
#include <type_traits>
namespace ct
{
    struct StringView
    {
        constexpr static const auto npos = std::string::npos;

        inline StringView(const std::string& str);

        template <size_t N>
        constexpr StringView(const char (&data)[N]);

        constexpr StringView(const char* data, size_t n);
        constexpr StringView(const char* data);

        constexpr StringView slice(size_t begin, size_t end) const;

        constexpr StringView substr(size_t pos = 0, size_t count = 0) const;

        constexpr size_t find(char character, size_t n = 0, size_t pos = 0) const;

        constexpr size_t rfind(char character, size_t n = 0) const;

        constexpr size_t count(char character, size_t pos = 0, size_t cnt = 0) const;

        constexpr operator const char*() const;
        operator std::string() const;

        constexpr char operator[](const size_t idx) const;

        constexpr const char* data() const;
        constexpr size_t size() const;

        constexpr int toInteger() const;

        constexpr uint32_t hash() const;

        constexpr bool equal(const StringView other) const;

        constexpr bool operator==(const StringView other) const;
        constexpr bool operator!=(const StringView other) const;

      private:
        constexpr bool equalImpl(const StringView other, size_t pos) const;

        constexpr size_t rfindImpl(char character, size_t n, size_t pos) const;

        const char* m_data;
        size_t m_size;
    };

    std::ostream& operator<<(std::ostream& os, const StringView& view)
    {
        for (size_t i = 0; i < view.size(); ++i)
        {
            os << view[i];
        }
        return os;
    }

    template <size_t N>
    constexpr size_t strLen(const char (&str)[N])
    {
        return N;
    }

    constexpr size_t strLen(const StringView view) { return view.size(); }

    constexpr size_t strLen(const char* str, size_t len = 0) { return *str ? strLen(str + 1, len + 1) : len; }

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

    constexpr size_t findNthFromEnd(const char* str, const size_t N, const char character = ':', const size_t pos = 0)
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
        return detail::findDeliminator(str, N - 1, character);
    }

    constexpr std::size_t findLast(const StringView view, char character = ':')
    {
        return detail::findDeliminator(view.data(), view.size() - 1, character);
    }

    constexpr std::size_t findLast(const char* str, char character = ':')
    {
        return detail::findDeliminator(str, ct::strLen(str) - 1, character);
    }

    // https://stackoverflow.com/questions/25195176/how-do-i-convert-a-c-string-to-a-int-at-compile-time
    constexpr bool isDigit(char c) { return c <= '9' && c >= '0'; }

    constexpr int stoiImplRange(const char* str, const char* end, int value = 0, bool negative = false)
    {
        return str != end
                   ? isDigit(*str) ? stoiImplRange(str + 1, end, (*str - '0') + value * 10, negative)
                                   : *str == '-' ? stoiImplRange(str + 1, end, value, true)
                                                 : throw "compile-time-error: not a digit"
                   : negative ? -value : value;
    }

    constexpr int stoiImpl(const char* str, int value = 0, bool negative = false)
    {
        return *str
                   ? isDigit(*str)
                         ? stoiImpl(str + 1, (*str - '0') + value * 10, negative)
                         : *str == '-' ? stoiImpl(str + 1, value, true) : throw "compile-time-error: not a digit"
                   : value;
    }

    constexpr int stoi(const char* str) { return stoiImpl(str); }

    constexpr int stoiRange(const char* start, const char* end) { return stoiImplRange(start, end); }

    constexpr int stoiRange(const char* start, int len) { return stoiImplRange(start, start + len); }

    ///// Implementation

    StringView::StringView(const std::string& str) : m_data(str.c_str()), m_size(str.size()) {}

    template <size_t N>
    constexpr StringView::StringView(const char (&data)[N]) : m_data(data), m_size(N)
    {
    }

    constexpr StringView::StringView(const char* data, size_t n) : m_data(data), m_size(n) {}
    constexpr StringView::StringView(const char* data) : m_data(data), m_size(strLen(data)) {}

    constexpr StringView StringView::slice(size_t begin, size_t end) const
    {
        return StringView(m_data + begin, end - begin);
    }

    constexpr StringView StringView::substr(size_t pos, size_t count) const
    {
        return count == 0 ? slice(pos, m_size - pos) : slice(pos, pos + count);
    }

    constexpr size_t StringView::find(char character, size_t n, size_t pos) const
    {
        return m_data[pos] == character ? (n == 0 ? pos : find(character, n - 1, pos + 1))
                                        : (pos == m_size ? npos : find(character, n, pos + 1));
    }

    constexpr size_t StringView::rfindImpl(char character, size_t n, size_t pos) const
    {
        return (pos == 0 ? (m_data[pos] == character ? (n == 0 ? pos : npos) : npos)
                         : (m_data[pos] == character ? (n == 0 ? pos : rfindImpl(character, n - 1, pos - 1))
                                                     : rfindImpl(character, n, pos - 1)));
    }

    constexpr size_t StringView::rfind(char character, size_t n) const { return rfindImpl(character, n, m_size); }

    constexpr size_t StringView::count(char character, size_t pos, size_t cnt) const
    {
        return pos == m_size ? cnt : (m_data[pos] == character ? cnt + 1 : count(character, pos + 1, cnt));
    }

    constexpr StringView::operator const char*() const { return m_data; }

    StringView::operator std::string() const { return std::string(m_data, m_data + m_size); }

    constexpr int StringView::toInteger() const { return stoiRange(m_data, m_size); }

    constexpr char StringView::operator[](const size_t idx) const
    {
        return idx < m_size ? m_data[idx] : throw "compile-time-error: out of range";
    }

    constexpr const char* StringView::data() const { return m_data; }

    constexpr size_t StringView::size() const { return m_size; }

    constexpr bool StringView::equal(const StringView other) const
    {
        return other.size() == size() ? (size() == other.size() && equalImpl(other, 0)) : false;
    }

    constexpr bool StringView::operator==(const StringView other) const { return equal(other); }

    constexpr bool StringView::operator!=(const StringView other) const { return !(*this == other); }

    constexpr bool StringView::equalImpl(const StringView other, size_t pos) const
    {
        return (pos == (m_size - 1) || pos == (other.size() - 1))
                   ? m_data[pos] == other[pos]
                   : (m_data[pos] == other[pos] ? equalImpl(other, pos + 1) : false);
    }

} // namespace ct
