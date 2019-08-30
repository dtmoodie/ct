#include "../common.hpp"

#include <ct/StringView.hpp>
#include <ct/reflect.hpp>
#include <ct/static_asserts.hpp>

#include <iostream>
#include <iterator>
#include <type_traits>

using namespace ct;

namespace ct
{
    DECL_NAME_1(float);
}

static constexpr const char* getString()
{
    return "asdf";
}

template <size_t N>
struct CompileTimeString;

template <size_t N, size_t... I1, size_t... I2>
constexpr CompileTimeString<N>
insertImpl(CompileTimeString<N> str, StringView insert_string, IndexSequence<I1...>, IndexSequence<I2...>)
{
    return {str.data[I1]..., insert_string[I2]...};
}

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

template <size_t N, size_t... I>
CompileTimeString<N> expand(CompileTimeString<N> str, IndexSequence<I...>)
{
    return {str.data[I]...};
}

template <size_t N, size_t... I>
constexpr CompileTimeString<N> generateString(StringView str, IndexSequence<I...>)
{
    return {str[I]...};
}

// Goal is to generate the name std::vector<T> where T is filled in with the name of T
// ie: std::vector<float>
// Why?  Because there isn't a great cross platform way of having consistent names, for most stuff it works correctly
// but for containers it's still iffy, so if we just make specializations of ct::GetName we can consistently generate
// strings with the following.
template <class T>
struct GenerateName
{
    static constexpr const auto prefix = makeCTS("std::vector<");
    static constexpr const auto prefix_len = prefix.size();
    static constexpr const auto postfix = makeCTS(">");
    static constexpr const auto postfix_len = postfix.size();

    static constexpr const auto substring_ = ct::GetName<T>::getName();
    static constexpr const auto substring_len = substring_.size();
    static constexpr const auto substring = makeCTS<substring_len + 1>(substring_);

    static constexpr const size_t total_len = prefix_len + substring_len + postfix_len;
    static constexpr const CompileTimeString<total_len - 1> name = (prefix + substring) + postfix;
    static constexpr const auto test = (prefix + substring);
    static constexpr const StringView getName()
    {
        return StringView(name.data, prefix_len + substring_len + postfix_len - 2);
    }
};

template <class T>
constexpr const CompileTimeString<GenerateName<T>::total_len - 1> GenerateName<T>::name;

int main()
{
    constexpr const auto generated_str = GenerateName<float>::getName();
    constexpr const auto expected_str = StringView("std::vector<float>");
    static_assert(generated_str == expected_str, "asdf");

    static_assert(StringView(CompileTimeString<5>().data, 5) == StringView("\0\0\0\0\0", 5), "asdf");

    STATIC_EQUAL(ct::strLen("asdf"), 4);
    STATIC_EQUAL(ct::strLen(getString()), 4);

    STATIC_EQUAL(ct::findFirst("asdf:asdf"), 4);
    STATIC_EQUAL(ct::findLast("asdf:asdf"), 4);
    STATIC_EQUAL(ct::findLast("asdf:asdf", 'a'), 5);
    STATIC_EQUAL("asdf:asdf"[4], ':');

    constexpr const char* str1 = "asdfasdfasdf";
    StaticEquality<uint32_t, crc32(StringView(str1).substr(0, 4)), crc32("asdf")>{};
    StaticEquality<uint32_t, crc32(StringView(str1).substr(1, 4)), crc32("sdfa")>{};
    StaticEquality<uint32_t, crc32(StringView(str1).slice(2, 4)), crc32("df")>{};
    StaticEquality<uint32_t, crc32(StringView(str1).slice(1, 4)), crc32("sdf")>{};
    StaticEquality<uint32_t, crc32(StringView(str1).slice(3, 5)), crc32("fa")>{};

    StaticEquality<size_t, findFirst(str1, 'f'), 3>{};
    StaticEquality<size_t, StringView(str1).find('s'), 1>{};
    StaticEquality<size_t, StringView(str1).find('s', 1), 5>{};
    StaticEquality<size_t, StringView(str1).find('a', 0, 1), 4>{};

    StaticEquality<size_t, findLast(str1, 'd'), 10>{};
    StaticEquality<size_t, StringView(str1).rfind('d'), 10>{};
    StaticEquality<size_t, StringView(str1).rfind('d', 1), 6>{};
    StaticEquality<size_t, findNthFromBeginning(str1, 2, 'a'), 8>{};
    StaticEquality<size_t, StringView(str1).find('a', 2), 8>{};

    StaticEquality<int, StringView("10").toInteger(), 10>{};
    StaticEquality<int, StringView("-10").toInteger(), -10>{};
    StaticEquality<int, StringView("-5001").substr(0, 4).toInteger(), -500>{};
    StaticEquality<int, StringView("5001").substr(0, 2).toInteger(), 50>{};

    StaticEquality<int, StringView(str1).size(), 12>{};
    StaticEquality<int, StringView(str1).substr(0, 4).size(), 4>{};
    StaticEquality<int, StringView(str1).slice(0, 4).size(), 4>{};

    static_assert(StringView(str1) == StringView(str1), "asdf");
    static_assert(StringView(str1).substr(0, 4) == StringView("asdf"), "asdf");

    static_assert(StringView(str1).substr(0, 4) != StringView("asdfa"), "asdf");
    static_assert(StringView(str1).substr(1, 5) != StringView("asdfa"), "asdf");

    StaticEquality<uint32_t, crc32(StringView(str1)), crc32(str1)>{};

    static_assert(StringView("asdf") == StringView("asdf"), "asdf");

    static_assert(StringView("asdf") != StringView("asdfg"), "asdf");

    static_assert(StringView("asdf") != StringView("asdg"), "asdf");

    static_assert(StringView("asdf") != StringView("ssdf"), "asdf");

    static_assert(ct::compare('a', 'A', false), "asdf");

    static_assert(StringView("asdf").equal(StringView("ASDF"), false), "asdf");

    StaticEquality<size_t, StringView("  asdf  ").findFirstChar(), 2>{};
    StaticEquality<size_t, StringView("  asdf  ").findLastChar(), 5>{};
    static_assert(StringView("  asdf  ").strip() == StringView("asdf"), "asdf");
    static_assert(StringView("  asdf  asdf  ").strip() == StringView("asdf  asdf"), "asdf");

    StaticEquality<size_t, StringView("asdfadfa").find("fadf"), 3>{};
    StaticEquality<size_t, StringView("asdfadfa").rfind("fadf"), 3>{};
    StaticEquality<size_t, StringView("asdfasdf").find("bgr"), StringView::npos>{};
    StaticEquality<size_t, StringView("bgr").find("asdfasdf"), StringView::npos>{};
    return 0;
}
