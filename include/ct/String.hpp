#ifndef CT_STRING_HPP
#define CT_STRING_HPP
#include "StringImpl.hpp"
#include <array>
#include <cstddef>
#include <ostream>
#include <type_traits>

#if defined(_MSC_VER)
#include <BaseTsd.h>
using ssize_t = SSIZE_T;
#endif

namespace ct
{

    template <class T = char>
    struct BasicStringView
    {
        static const auto npos = std::string::npos;

        inline BasicStringView(const std::basic_string<T>& str);

        template <size_t N>
        constexpr BasicStringView(const T (&data)[N]);

        constexpr BasicStringView(const T* data, size_t n);
        constexpr BasicStringView(const T* data);

        constexpr BasicStringView slice(ssize_t begin, ssize_t end) const;

        constexpr BasicStringView substr(size_t pos = 0, size_t count = 0) const;

        constexpr size_t find(T character, size_t n = 0, size_t pos = 0) const;

        constexpr size_t rfind(T character, size_t n = 0) const;

        constexpr size_t count(T character, size_t pos = 0, size_t cnt = 0) const;

        constexpr operator const T*() const;
        operator std::basic_string<T>() const;
        std::basic_string<T> toString() const;

        constexpr char operator[](const size_t idx) const;

        constexpr const T* data() const;
        constexpr size_t size() const;

        constexpr int toInteger() const;

        constexpr uint32_t hash() const;

        constexpr bool equal(const BasicStringView other) const;

        constexpr bool operator==(const BasicStringView other) const;
        constexpr bool operator!=(const BasicStringView other) const;

        constexpr size_t revIndex(const ssize_t idx) const;

      private:
        constexpr bool equalImpl(const BasicStringView other, size_t pos) const;

        constexpr size_t rfindImpl(T character, size_t n, size_t pos) const;

        const T* m_data;
        size_t m_size;
    };

    using StringView = BasicStringView<>;

    template <class T>
    inline std::ostream& operator<<(std::ostream& os, const BasicStringView<T>& view)
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

    template <class T>
    constexpr size_t strLen(const BasicStringView<T> view)
    {
        return view.size();
    }

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

    template <class T>
    constexpr std::size_t findLast(const BasicStringView<T> view, char character = ':')
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

    template <class T>
    BasicStringView<T>::BasicStringView(const std::basic_string<T>& str) : m_data(str.c_str()), m_size(str.size())
    {
    }

    template <class T>
    template <size_t N>
    constexpr BasicStringView<T>::BasicStringView(const T (&data)[N]) : m_data(data), m_size(N)
    {
    }

    template <class T>
    constexpr BasicStringView<T>::BasicStringView(const T* data, size_t n) : m_data(data), m_size(n)
    {
    }

    template <class T>
    constexpr BasicStringView<T>::BasicStringView(const T* data) : m_data(data), m_size(strLen(data))
    {
    }

    template <class T>
    constexpr BasicStringView<T> BasicStringView<T>::slice(ssize_t begin, ssize_t end) const
    {
        return BasicStringView(m_data + revIndex(begin), revIndex(end) - revIndex(begin));
    }

    template <class T>
    constexpr BasicStringView<T> BasicStringView<T>::substr(size_t pos, size_t count) const
    {
        return count == 0 ? slice(pos, m_size - pos) : slice(pos, pos + count);
    }

    template <class T>
    constexpr size_t BasicStringView<T>::find(T character, size_t n, size_t pos) const
    {
        return m_data[pos] == character ? (n == 0 ? pos : find(character, n - 1, pos + 1))
                                        : (pos == m_size ? npos : find(character, n, pos + 1));
    }

    template <class T>
    constexpr size_t BasicStringView<T>::rfindImpl(T character, size_t n, size_t pos) const
    {
        return (pos == 0 ? (m_data[pos] == character ? (n == 0 ? pos : npos) : npos)
                         : (m_data[pos] == character ? (n == 0 ? pos : rfindImpl(character, n - 1, pos - 1))
                                                     : rfindImpl(character, n, pos - 1)));
    }

    template <class T>
    constexpr size_t BasicStringView<T>::rfind(T character, size_t n) const
    {
        return rfindImpl(character, n, m_size);
    }

    template <class T>
    constexpr size_t BasicStringView<T>::count(T character, size_t pos, size_t cnt) const
    {
        return pos == m_size ? cnt : (m_data[pos] == character ? cnt + 1 : count(character, pos + 1, cnt));
    }

    template <class T>
    constexpr BasicStringView<T>::operator const T*() const
    {
        return m_data;
    }

    template <class T>
    BasicStringView<T>::operator std::basic_string<T>() const
    {
        return std::string(m_data, m_data + m_size);
    }

    template <class T>
    std::basic_string<T> BasicStringView<T>::toString() const
    {
        return std::string(m_data, m_data + m_size);
    }

    template <class T>
    constexpr int BasicStringView<T>::toInteger() const
    {
        return stoiRange(m_data, m_size);
    }

    template <class T>
    constexpr char BasicStringView<T>::operator[](const size_t idx) const
    {
        return idx < m_size ? m_data[idx] : throw "compile-time-error: out of range";
    }

    template <class T>
    constexpr const T* BasicStringView<T>::data() const
    {
        return m_data;
    }

    template <class T>
    constexpr size_t BasicStringView<T>::size() const
    {
        return m_size;
    }

    template <class T>
    constexpr bool BasicStringView<T>::equal(const BasicStringView other) const
    {
        return other.size() == size() ? (size() == other.size() && equalImpl(other, 0)) : false;
    }

    template <class T>
    constexpr bool BasicStringView<T>::operator==(const BasicStringView other) const
    {
        return equal(other);
    }

    template <class T>
    constexpr bool BasicStringView<T>::operator!=(const BasicStringView other) const
    {
        return !(*this == other);
    }

    template <class T>
    constexpr bool BasicStringView<T>::equalImpl(const BasicStringView other, size_t pos) const
    {
        return (pos == (m_size - 1) || pos == (other.size() - 1))
                   ? m_data[pos] == other[pos]
                   : (m_data[pos] == other[pos] ? equalImpl(other, pos + 1) : false);
    }

    template <class T>
    constexpr size_t BasicStringView<T>::revIndex(const ssize_t idx) const
    {
        return idx >= 0 ? idx : m_size + idx;
    }

} // namespace ct
#endif // CT_STRING_HPP
