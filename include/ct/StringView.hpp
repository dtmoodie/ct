#ifndef CT_STRING_VIEW_HPP
#define CT_STRING_VIEW_HPP
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
    template<class T>
    struct CharTable;

    template<>
    struct CharTable<char>
    {
        static constexpr const char space = ' ';
        static constexpr const char first_non_whitespace = 33;
        static constexpr const char last_non_whitespace = 126;
        static constexpr const char newline = '\n';
    };


    template <class T = char>
    struct BasicStringView
    {
        constexpr static const size_t npos = static_cast<size_t>(-1);

        inline BasicStringView(const std::basic_string<T>& str);

        template <size_t N>
        constexpr BasicStringView(const T (&data)[N]);

        constexpr BasicStringView(const T* data, size_t n);
        constexpr BasicStringView(const T* data);
        constexpr BasicStringView() = default;

        constexpr BasicStringView slice(ssize_t begin, ssize_t end) const;

        constexpr BasicStringView substr(ssize_t pos = 0, size_t count = 0) const;

        constexpr size_t find(T character, size_t n = 0, size_t pos = 0) const;
        constexpr size_t find(BasicStringView substring, size_t n = 0, size_t pos = 0) const;

        constexpr size_t findInRange(T lower, T upper, size_t n = 0, size_t pos = 0) const;

        constexpr size_t findFirstChar(size_t pos = 0) const;

        constexpr size_t rfind(T character, size_t n = 0) const;
        constexpr size_t rfind(BasicStringView substring, size_t n = 0) const;

        constexpr size_t rfindInRange(T lower, T upper, size_t n = 0) const;

        constexpr size_t findLastChar() const;

        constexpr size_t count(T character, size_t pos = 0, size_t cnt = 0) const;

        constexpr BasicStringView strip() const;

        //constexpr operator const T*() const;
        constexpr const T* cStr() const;
        operator std::basic_string<T>() const;
        std::basic_string<T> toString() const;

        constexpr char operator[](const size_t idx) const;

        constexpr const T* data() const;
        constexpr size_t size() const;

        constexpr bool empty() const;

        constexpr int toInteger() const;

        constexpr uint32_t hash() const;

        constexpr bool equal(const BasicStringView other, bool case_sensitive = true) const;

        constexpr bool operator==(const BasicStringView other) const;
        constexpr bool operator!=(const BasicStringView other) const;

        constexpr size_t revIndex(const ssize_t idx) const;

      private:
        constexpr BasicStringView sliceHelper(size_t begin, size_t end) const;
        constexpr bool equalImpl(const BasicStringView other, size_t pos, bool case_sensitive = true) const;

        constexpr size_t rfindImpl(T character, size_t n, size_t pos) const;
        constexpr size_t rfindImpl(BasicStringView substring, size_t n, size_t pos) const;
        constexpr size_t rfindInRangeImpl(T lower, T upper, size_t n, size_t pos) const;

        const T* m_data = nullptr;
        size_t m_size = 0;
    };

    using StringView = BasicStringView<>;

    namespace detail
    {
        constexpr size_t findFirst(const char* str, size_t N, size_t idx, char del);

        template <size_t idx, size_t N>
        constexpr size_t findFirst(const char* str, char del);

        constexpr size_t findDeliminator(const char* str, size_t len);

        constexpr size_t findDeliminator(const char* str, size_t len, char del);

        constexpr size_t findLast(const char* str, size_t len, char Del);
    }

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
    constexpr size_t strLen(const char (&)[N])
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
                                                 : throw std::runtime_error("compile-time-error: not a digit")
                   : negative ? -value : value;
    }

    constexpr int stoiImpl(const char* str, int value = 0, bool negative = false)
    {
        return *str
                   ? isDigit(*str) ? stoiImpl(str + 1, (*str - '0') + value * 10, negative)
                                   : *str == '-' ? stoiImpl(str + 1, value, true)
                                                 : throw std::runtime_error("compile-time-error: not a digit")
                   : value;
    }

    constexpr int stoi(const char* str) { return stoiImpl(str); }

    constexpr int stoiRange(const char* start, const char* end) { return stoiImplRange(start, end); }

    constexpr int stoiRange(const char* start, int len) { return stoiImplRange(start, start + len); }

    ///// Implementation
    namespace detail
    {
        constexpr size_t findFirst(const char* str, size_t N, size_t idx, char del)
        {
            return idx == N ? N : str[idx] == del ? idx : findFirst(str, N, idx + 1, del);
        }

        template <size_t idx, size_t N>
        constexpr size_t findFirst(const char* str, char del)
        {
            return str[idx] == del ? idx : idx == N - 1 ? N : findFirst<idx + 1, N>(str, del);
        }

        // len needs to be length of string - 1
        constexpr size_t findDeliminator(const char* str, size_t len)
        {
            return str[len] == ':' ? len : len == 0 ? 0 : findDeliminator(str, len - 1);
        }

        constexpr size_t findDeliminator(const char* str, size_t len, char del)
        {
            return str[len] == del ? len : len == 0 ? 0 : findDeliminator(str, len - 1, del);
        }

        constexpr size_t findLast(const char* str, size_t len, char Del)
        {
            return str[len] == Del ? len : len == 0 ? 0 : findLast(str, len - 1, Del);
        }
    }

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
        return sliceHelper(revIndex(begin), (end == 0 && begin < 0) ? m_size : revIndex(end));
    }

    template <class T>
    constexpr BasicStringView<T> BasicStringView<T>::sliceHelper(size_t begin, size_t end) const
    {
        return begin < end ? BasicStringView(m_data + begin, end - begin)
                           : throw std::runtime_error(std::string("invalid range begin=") + std::to_string(begin) +
                                                      std::string(" end=") + std::to_string(end) +
                                                      std::string(" when slicing string '") + toString() + "'");
    }

    template <class T>
    constexpr BasicStringView<T> BasicStringView<T>::substr(ssize_t pos, size_t count) const
    {
        return count == 0 ? slice(revIndex(pos), m_size) : slice(revIndex(pos), revIndex(pos) + count);
    }

    template <class T>
    constexpr size_t BasicStringView<T>::find(T character, size_t n, size_t pos) const
    {
        return m_data[pos] == character ? (n == 0 ? pos : find(character, n - 1, pos + 1))
                                        : (pos == m_size ? npos : find(character, n, pos + 1));
    }

    template <class T>
    constexpr size_t BasicStringView<T>::find(BasicStringView substring, size_t n, size_t pos) const
    {
        return substring.size() < size() ? (substr(pos, substring.size()) == substring ? (n == 0 ? pos : find(substring, n - 1, pos + 1))
                                        : ((pos + substring.size()) == m_size ? npos : find(substring, n, pos + 1))) : npos;
    }

    template <class T>
    constexpr size_t BasicStringView<T>::findInRange(T lower, T upper, size_t n, size_t pos) const
    {
        return m_data[pos] >= lower && m_data[pos] < upper ? (n == 0 ? pos : findInRange(lower, upper, n - 1, pos + 1))
                                                           : (pos == m_size ? npos : findInRange(lower, upper, n, pos + 1));
    }

    template <class T>
    constexpr size_t BasicStringView<T>::findFirstChar(size_t pos) const
    {
        return findInRange(CharTable<T>::first_non_whitespace, CharTable<T>::last_non_whitespace + 1, 0, pos);
    }

    template <class T>
    constexpr size_t BasicStringView<T>::rfindImpl(T character, size_t n, size_t pos) const
    {
        return (pos == 0 ? (m_data[pos] == character ? (n == 0 ? pos : npos) : npos)
                         : (m_data[pos] == character ? (n == 0 ? pos : rfindImpl(character, n - 1, pos - 1))
                                                     : rfindImpl(character, n, pos - 1)));
    }

    template <class T>
    constexpr size_t BasicStringView<T>::rfindImpl(BasicStringView substring, size_t n, size_t pos) const
    {
        return (pos == 0 ? ((substr(pos, substring.size()) == substring) ? (n == 0 ? pos : npos) : npos)
                         : ((substr(pos, substring.size()) == substring) ? (n == 0 ? pos : rfindImpl(substring, n - 1, pos - 1))
                                                     : rfindImpl(substring, n, pos - 1)));
    }

    template <class T>
    constexpr size_t BasicStringView<T>::rfind(T character, size_t n) const
    {
        return rfindImpl(character, n, m_size);
    }

    template <class T>
    constexpr size_t BasicStringView<T>::rfind(BasicStringView substring, size_t n) const
    {
        return substring.size() < size() ? rfindImpl(substring, n, m_size - substring.size()) : npos;
    }

    template <class T>
    constexpr size_t BasicStringView<T>::rfindInRangeImpl(T lower, T upper, size_t n, size_t pos) const
    {
        return (pos == 0 ? ((m_data[pos] >= lower && m_data[pos] < upper) ? (n == 0 ? pos : npos) : npos)
                         : ((m_data[pos] >= lower && m_data[pos] < upper) ? (n == 0 ? pos : rfindInRangeImpl(lower, upper, n - 1, pos - 1))
                                                     : rfindInRangeImpl(lower, upper, n, pos - 1)));
    }

    template<class T>
    constexpr size_t BasicStringView<T>::rfindInRange(T lower, T upper, size_t n) const
    {
        return rfindInRangeImpl(lower, upper, n, m_size);
    }

    template<class T>
    constexpr size_t BasicStringView<T>::findLastChar() const
    {
        return rfindInRange(CharTable<T>::first_non_whitespace, CharTable<T>::last_non_whitespace + 1);
    }

    template<class T>
    constexpr BasicStringView<T> BasicStringView<T>::strip() const
    {
        return slice(findFirstChar(), findLastChar() + 1);
    }

    template <class T>
    constexpr size_t BasicStringView<T>::count(T character, size_t pos, size_t cnt) const
    {
        return pos == m_size ? cnt : (m_data[pos] == character ? count(character, pos + 1, cnt + 1)
                                                               : count(character, pos + 1, cnt));
    }

	template <class T>
    constexpr const T* BasicStringView<T>::cStr() const
	{
        return m_data;
	}

    /*template <class T>
    constexpr BasicStringView<T>::operator const T*() const
    {
        return m_data;
    }*/

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
    constexpr bool BasicStringView<T>::empty() const
    {
        return m_size == 0;
    }

    template <class T>
    constexpr bool BasicStringView<T>::equal(const BasicStringView other, bool case_sensitive) const
    {
        return other.size() == size() ? equalImpl(other, 0, case_sensitive) : false;
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

    constexpr char lower(const char c) { return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c; }

    constexpr bool compare(char a, char b, bool case_sensitive = true)
    {
        return case_sensitive ? (a == b) : lower(a) == lower(b);
    }

    template <class T>
    constexpr bool BasicStringView<T>::equalImpl(const BasicStringView other, size_t pos, bool case_sensitive) const
    {
        return (pos == (m_size - 1))
                   ? compare(m_data[pos], other[pos], case_sensitive)
                   : (compare(m_data[pos], other[pos], case_sensitive) ? equalImpl(other, pos + 1, case_sensitive)
                                                                       : false);
    }

    template <class T>
    constexpr size_t BasicStringView<T>::revIndex(const ssize_t idx) const
    {
        return idx >= 0 ? idx : m_size + idx;
    }

} // namespace ct
#endif // CT_STRING_VIEW_HPP
