#include "gtest/gtest.h"
#include <ct/StringView.hpp>
#include <ct/reflect.hpp>


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}

using namespace ct;
static constexpr const char* getString()
{
    return "asdf";
}

TEST(string_utils, len)
{
    ASSERT_EQ(ct::strLen("asdf"), 4);
    ASSERT_EQ(ct::strLen(getString()), 4);
}


TEST(string_search, find_first)
{
    ASSERT_EQ(ct::findFirst("asdf:asdf"), 4);
    ASSERT_EQ(ct::findFirst("asdf:asdf", 'a'), 0);
    ASSERT_EQ(ct::findFirst("asdf:asdf", 'f'), 3);
}

TEST(string_search, find_last)
{
    ASSERT_EQ(ct::findLast("asdf:asdf"), 4);
    ASSERT_EQ(ct::findLast("asdf:asdf", 'a'), 5);
}

TEST(string_search, count)
{
    ASSERT_EQ(StringView("asdfasdf").count('a'), 2);
    ASSERT_EQ(StringView("asdfasdf").count('a', 1), 1);
    ASSERT_EQ(StringView("asdfasdf").count('g'), 0);
}

TEST(string_equality, case_sensitive)
{
    ASSERT_EQ(StringView("asdf"), StringView("asdf"));
    ASSERT_NE(StringView("asdf"), StringView("Asdf"));
    ASSERT_NE(StringView("asdf"), StringView("asdff"));
    ASSERT_NE(StringView("asdfg"), StringView("asdff"));
}


TEST(string_equality, case_insensitive)
{
    ASSERT_EQ(StringView("asdf"), StringView("asdf"));
    ASSERT_EQ(StringView("asdf").equal(StringView("Asdf"), false), true);
    ASSERT_NE(StringView("asdf"), StringView("asdff"));
    ASSERT_NE(StringView("asdfg"), StringView("asdff"));
}


constexpr const char* str1 = "asdfasdfasdf";
TEST(string_slicing, substr)
{
    ASSERT_EQ(StringView(str1).substr(0, 4), StringView("asdf"));
    ASSERT_EQ(StringView(str1).substr(1, 4), StringView("sdfa"));
    ASSERT_EQ(StringView(str1).substr(-2), StringView("df"));
}

TEST(string_slicing, slice)
{
    ASSERT_EQ(StringView(str1).slice(0, 4), StringView("asdf"));
    ASSERT_EQ(StringView(str1).slice(1, 4), StringView("sdf"));
    ASSERT_EQ(StringView(str1).slice(-5, -1), StringView("fasd"));
    ASSERT_EQ(StringView(str1).slice(-5, 0), StringView("fasdf"));

    ASSERT_THROW(StringView(str1).slice(-5, -6), std::runtime_error);
}


TEST(string_conversion, integer)
{
    ASSERT_EQ(StringView("54321").toInteger(), 54321);
    ASSERT_THROW(StringView("asdf").toInteger(), std::runtime_error);
}

TEST(string_hash, full_string)
{
    ASSERT_EQ(crc32(StringView(str1).substr(0, 4)), crc32("asdf"));
    ASSERT_EQ(crc32(StringView(str1).substr(1, 4)), crc32("sdfa"));
}
