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
    EXPECT_EQ(ct::strLen("asdf"), 4);
    EXPECT_EQ(ct::strLen(getString()), 4);
}


TEST(string_search, find_first)
{
    EXPECT_EQ(ct::findFirst("asdf:asdf"), 4);
    EXPECT_EQ(ct::findFirst("asdf:asdf", 'a'), 0);
    EXPECT_EQ(ct::findFirst("asdf:asdf", 'f'), 3);
}

TEST(string_search, find_last)
{
    EXPECT_EQ(ct::findLast("asdf:asdf"), 4);
    EXPECT_EQ(ct::findLast("asdf:asdf", 'a'), 5);
}

TEST(string_search, count)
{
    EXPECT_EQ(StringView("asdfasdf").count('a'), 2);
    EXPECT_EQ(StringView("asdfasdf").count('a', 1), 1);
    EXPECT_EQ(StringView("asdfasdf").count('g'), 0);
}

TEST(string_equality, case_sensitive)
{
    EXPECT_EQ(StringView("asdf"), StringView("asdf"));
    EXPECT_NE(StringView("asdf"), StringView("Asdf"));
    EXPECT_NE(StringView("asdf"), StringView("asdff"));
    EXPECT_NE(StringView("asdfg"), StringView("asdff"));
}


TEST(string_equality, case_insensitive)
{
    EXPECT_EQ(StringView("asdf"), StringView("asdf"));
    EXPECT_EQ(StringView("asdf").equal(StringView("Asdf"), false), true);
    EXPECT_NE(StringView("asdf"), StringView("asdff"));
    EXPECT_NE(StringView("asdfg"), StringView("asdff"));
}


constexpr const char* str1 = "asdfasdfasdf";
TEST(string_slicing, substr)
{
    EXPECT_EQ(StringView(str1).substr(0, 4), StringView("asdf"));
    EXPECT_EQ(StringView(str1).substr(1, 4), StringView("sdfa"));
    EXPECT_EQ(StringView(str1).substr(-2), StringView("df"));
}

TEST(string_slicing, slice)
{
    EXPECT_EQ(StringView(str1).slice(0, 4), StringView("asdf"));
    EXPECT_EQ(StringView(str1).slice(1, 4), StringView("sdf"));
    EXPECT_EQ(StringView(str1).slice(-5, -1), StringView("fasd"));
    EXPECT_EQ(StringView(str1).slice(-5, 0), StringView("fasdf"));

    EXPECT_THROW(StringView(str1).slice(-5, -6), std::runtime_error);
}


TEST(string_conversion, integer)
{
    EXPECT_EQ(StringView("54321").toInteger(), 54321);
    EXPECT_THROW(StringView("asdf").toInteger(), std::runtime_error);
}

TEST(string_hash, full_string)
{
    EXPECT_EQ(crc32(StringView(str1).substr(0, 4)), crc32("asdf"));
    EXPECT_EQ(crc32(StringView(str1).substr(1, 4)), crc32("sdfa"));
}
