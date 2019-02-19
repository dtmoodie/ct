#include "../common.hpp"

#include <ct/StringView.hpp>
#include <ct/reflect.hpp>
#include <ct/static_asserts.hpp>

#include <iostream>
#include <iterator>
#include <type_traits>

namespace test
{
    class TestClass;
}

struct TestStruct
{
    static constexpr auto getTypeHelper() -> typename std::remove_reference<decltype(*this)>::type;
    using DataType = decltype(getTypeHelper());
};

using namespace ct;
static constexpr const char* getString()
{
    return "asdf";
}

int main()
{
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

    // StaticEquality < size_t, StringView("asdf").

    // The following is not portable to gcc 4.8 :(
    // GCC's output ct::GetName<test::TestClass>::name();
    /*constexpr const char* class_name1 =
        "static constexpr ct::StringView ct::GetNameGCC<T>::name() [with T = test::TestClass]";
    // MSVC's output
    constexpr const char* class_name2 = "GetNameMSVC<class test::TestClass>::getName";
    // Just to be able to double check
    std::cout << ct::GetName<test::TestClass>::name() << std::endl;

    std::cout << class_name1 << std::endl;
    std::cout << class_name2 << std::endl;

    // StaticEquality < size_t,

    static_assert(
        ct::crc32(ct::detail::parseClassNameMSVC(
            "GetName<class std::vector<struct TestStruct,class std::allocator<struct TestStruct> > >::getName")) > 0,
        "asdf");

    using type = TestStruct::DataType;
    std::cout << std::integral_constant<uint32_t, ct::crc32(ct::GetName<type>::getName())>::value << std::endl;
    ct::StaticEquality<uint32_t,
                       std::integral_constant<uint32_t, ct::crc32(ct::GetName<type>::getName())>::value,
                       ct::crc32("TestStruct")>();
    std::cout << ct::GetName<type>::getName() << std::endl;

    std::cout << ct::findNthFromBeginning("asdfasdfasdf", 2, 'a') << std::endl;

    std::cout << ct::detail::parseClassNameMSVC(
                     "GetName<class std::vector<struct TestStruct,class std::allocator<struct TestStruct> > >::getName")
              << std::endl;*/

    return 0;
}
