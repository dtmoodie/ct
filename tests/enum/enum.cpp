#include "gtest/gtest.h"

#include "ct/EnumBitset.hpp"
#include "enum.hpp"
#include <ct/static_asserts.hpp>
#include <iostream>
#include <sstream>

// foo has to use value semantics in < c++17, if it uses const & semantics we try to take a reference to a constexpr
// static variable that is not inlined :/
#if __cplusplus < 201703L
template <class T>
void foo(T)
{
}
#else
template <class T>
void foo(const T&)
{
}
#endif

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

BITSET_START(Bitset)
    ENUM_BITVALUE(v0, 0)
    ENUM_BITVALUE(v1, 1)
    ENUM_BITVALUE(v2, 2)
    ENUM_BITVALUE(v3, 3)
    ENUM_BITVALUE(v4, 4)
    ENUM_BITVALUE(v5, 5)
ENUM_END;

template <uint64_t V>
void bitsetFoo(const std::string& expected)
{
    auto bitset = Bitset(V);
    std::stringstream ss;
    ss << bitset;
    ASSERT_EQ(expected, ss.str());
}

template <class T>
void checkPrint(T v, const std::string& expected)
{
    std::stringstream ss;
    ss << v;
    ASSERT_EQ(expected, ss.str());
}

template <class T>
void checkPrintEnums(const std::string& expected)
{
    std::stringstream ss;
    ct::printEnums<T>(ss);
    auto result = ss.str();
    ASSERT_EQ(expected, result);
}

TEST(enumerate, MyEnum)
{
    checkPrintEnums<MyClass::MyEnum>("kVALUE0 1\nkVALUE1 2\nkVALUE2 3\nkVALUE3 4\n");
}

TEST(enumerate, SecondEnum)
{
    checkPrintEnums<MyClass::SecondEnum>("kBGR 0\nkRGB 1\nkHSV 2\nkYUV 3\nkHSL 4\n");
}

TEST(enumerate, StandardEnum)
{
    checkPrintEnums<MyClass::StandardEnum>("k0 0\nk1 1\nk2 2\nk3 3\n");
}

TEST(enumerate, StandardEnum2)
{
    checkPrintEnums<MyClass::StandardEnum2>("kASDF 0\nk1234 1\n");
}

TEST(enumerate, BitwiseEnum)
{
    checkPrintEnums<MyClass::BitwiseEnum>("kVALUE0 0\nkVALUE1 1\nkVALUE2 2\n");
}

TEST(enumerate, MixedBitwise)
{
    checkPrintEnums<MyClass::MixedBitwise>(
        "kVALUE0 1\nkVALUE1 2\nkVALUE2 3\nkVALUE3 4\nkVALUE4 5\nkVALUE5 6\nkVALUE6 7\nkFLAG0 4\nkFLAG1 5\nkFLAG2 6\n");
}

TEST(enum_to_string, MyEnum)
{
    checkPrint(MyClass::MyEnum::kVALUE0, "kVALUE0 1");
    checkPrint(MyClass::MyEnum::kVALUE1, "kVALUE1 2");
    checkPrint(MyClass::MyEnum::kVALUE2, "kVALUE2 3");
    checkPrint(MyClass::MyEnum::kVALUE3, "kVALUE3 4");
}

TEST(enum_to_string, SecondEnum)
{
    checkPrint(MyClass::SecondEnum::kBGR, "kBGR 0");
    checkPrint(MyClass::SecondEnum::kRGB, "kRGB 1");
    checkPrint(MyClass::SecondEnum::kHSV, "kHSV 2");
    checkPrint(MyClass::SecondEnum::kYUV, "kYUV 3");
    checkPrint(MyClass::SecondEnum::kHSL, "kHSL 4");
}

TEST(enum_to_string, BitwiseEnum)
{
    checkPrint(MyClass::BitwiseEnum::kVALUE0, "kVALUE0 0");
    checkPrint(MyClass::BitwiseEnum::kVALUE1, "kVALUE1 1");
    checkPrint(MyClass::BitwiseEnum::kVALUE2, "kVALUE2 2");
}

TEST(enum_to_string, MixedBitwise)
{
    checkPrint(MyClass::MixedBitwise::kVALUE0, "kVALUE0 1");
    checkPrint(MyClass::MixedBitwise::kVALUE1, "kVALUE1 2");
    checkPrint(MyClass::MixedBitwise::kVALUE2, "kVALUE2 3");
    checkPrint(MyClass::MixedBitwise::kVALUE3, "kVALUE3 4");
    checkPrint(MyClass::MixedBitwise::kVALUE4, "kVALUE4 5");
    checkPrint(MyClass::MixedBitwise::kVALUE5, "kVALUE5 6");
    checkPrint(MyClass::MixedBitwise::kVALUE6, "kVALUE6 7");

    checkPrint(MyClass::MixedBitwise::kFLAG0, "kFLAG0 4");
    checkPrint(MyClass::MixedBitwise::kFLAG1, "kFLAG1 5");
    checkPrint(MyClass::MixedBitwise::kFLAG2, "kFLAG2 6");
}

TEST(enum_to_string, StandardEnum)
{
    checkPrint(MyClass::StandardEnum::k0, "k0");
    checkPrint(MyClass::StandardEnum::k1, "k1");
    checkPrint(MyClass::StandardEnum::k2, "k2");
    checkPrint(MyClass::StandardEnum::k3, "k3");
}

TEST(enum_to_string, StandardEnum2)
{
    checkPrint(MyClass::StandardEnum2::kASDF, "kASDF");
    checkPrint(MyClass::StandardEnum2::k1234, "k1234");
}

TEST(enum_value_to_string, MyEnum)
{
    static_assert(MyClass::MyEnum::kVALUE0 == 1, "asdf");
    static_assert(MyClass::MyEnum::kVALUE1 == 2, "asdf");
    static_assert(MyClass::MyEnum::kVALUE2 == 3, "asdf");

    MyClass::MyEnum val = MyClass::MyEnum::kVALUE0;
    checkPrint(val, "kVALUE0");
    val = MyClass::MyEnum::kVALUE1;
    checkPrint(val, "kVALUE1");
    val = MyClass::MyEnum::kVALUE2;
    checkPrint(val, "kVALUE2");
    val = MyClass::MyEnum::kVALUE3;
    checkPrint(val, "kVALUE3");
}

TEST(enum_value_to_string, SecondEnum)
{
    static_assert(MyClass::SecondEnum::kBGR == 0, "asdf");
    static_assert(MyClass::SecondEnum::kRGB == 1, "asdf");
    static_assert(MyClass::SecondEnum::kHSV == 2, "asdf");
    static_assert(MyClass::SecondEnum::kYUV == 3, "asdf");
    static_assert(MyClass::SecondEnum::kHSL == 4, "asdf");

    MyClass::SecondEnum val = MyClass::SecondEnum::kBGR;
    checkPrint(val, "kBGR");
    val = MyClass::SecondEnum::kRGB;
    checkPrint(val, "kRGB");
    val = MyClass::SecondEnum::kHSV;
    checkPrint(val, "kHSV");
    val = MyClass::SecondEnum::kYUV;
    checkPrint(val, "kYUV");
    val = MyClass::SecondEnum::kHSL;
    checkPrint(val, "kHSL");
}

TEST(enum_value_to_string, BitwiseEnum)
{
    ct::StaticEquality<int, MyClass::BitwiseEnum::kVALUE0, 1>{};
    ct::StaticEquality<int, MyClass::BitwiseEnum::kVALUE1, 2>{};
    ct::StaticEquality<int, MyClass::BitwiseEnum::kVALUE2, 4>{};

    MyClass::BitwiseEnum val = MyClass::BitwiseEnum::kVALUE0;
    checkPrint(val, "kVALUE0");
    val = MyClass::BitwiseEnum::kVALUE1;
    checkPrint(val, "kVALUE1");
    val = MyClass::BitwiseEnum::kVALUE2;
    checkPrint(val, "kVALUE2");
}

TEST(enum_value_to_string, MixedBitwise)
{
    ct::StaticEquality<uint64_t, MyClass::MixedBitwise::kVALUE0, 1>{};
    ct::StaticEquality<uint64_t, MyClass::MixedBitwise::kVALUE1, 2>{};
    ct::StaticEquality<uint64_t, MyClass::MixedBitwise::kVALUE2, 3>{};
    ct::StaticEquality<uint64_t, MyClass::MixedBitwise::kVALUE3, 4>{};
    ct::StaticEquality<uint64_t, MyClass::MixedBitwise::kVALUE4, 5>{};
    ct::StaticEquality<uint64_t, MyClass::MixedBitwise::kVALUE5, 6>{};
    ct::StaticEquality<uint64_t, MyClass::MixedBitwise::kVALUE6, 7>{};
    ct::StaticEquality<uint64_t, MyClass::MixedBitwise::kFLAG0, 16>{};
    ct::StaticEquality<uint64_t, MyClass::MixedBitwise::kFLAG1, 32>{};
    ct::StaticEquality<uint64_t, MyClass::MixedBitwise::kFLAG2, 64>{};

    MyClass::MixedBitwise val = MyClass::MixedBitwise::kVALUE0;
    checkPrint(val, "kVALUE0");
    val = MyClass::MixedBitwise::kVALUE1;
    checkPrint(val, "kVALUE1");
    val = MyClass::MixedBitwise::kVALUE2;
    checkPrint(val, "kVALUE2");
    val = MyClass::MixedBitwise::kVALUE3;
    checkPrint(val, "kVALUE3");
    val = MyClass::MixedBitwise::kVALUE4;
    checkPrint(val, "kVALUE4");
    val = MyClass::MixedBitwise::kVALUE5;
    checkPrint(val, "kVALUE5");
    val = MyClass::MixedBitwise::kVALUE6;
    checkPrint(val, "kVALUE6");

    val = MyClass::MixedBitwise::kVALUE0 | MyClass::MixedBitwise::kFLAG0;
    checkPrint(val, "kFLAG0|kVALUE0");
    val = MyClass::MixedBitwise::kVALUE0 | MyClass::MixedBitwise::kFLAG1;
    checkPrint(val, "kFLAG1|kVALUE0");
    val = MyClass::MixedBitwise::kVALUE0 | MyClass::MixedBitwise::kFLAG2;
    checkPrint(val, "kFLAG2|kVALUE0");

    val = MyClass::MixedBitwise::kVALUE4 | MyClass::MixedBitwise::kFLAG2;
    checkPrint(val, "kFLAG2|kVALUE4");
    val = MyClass::MixedBitwise::kVALUE4 | MyClass::MixedBitwise::kFLAG1;
    checkPrint(val, "kFLAG1|kVALUE4");
    val = MyClass::MixedBitwise::kVALUE4 | MyClass::MixedBitwise::kFLAG0;
    checkPrint(val, "kFLAG0|kVALUE4");
}

TEST(enum_value_to_string, StandardEnum)
{
    MyClass::StandardEnum val = MyClass::StandardEnum::k0;
    checkPrint(val, "k0");
    val = MyClass::StandardEnum::k1;
    checkPrint(val, "k1");
    val = MyClass::StandardEnum::k2;
    checkPrint(val, "k2");
    val = MyClass::StandardEnum::k3;
    checkPrint(val, "k3");
}

TEST(enum_value_to_string, StandardEnum2)
{
    MyClass::StandardEnum2 val = MyClass::StandardEnum2::kASDF;
    checkPrint(val, "kASDF");
    val = MyClass::StandardEnum2::k1234;
    checkPrint(val, "k1234");
}

TEST(enum_value_from_string, MyEnum)
{
    static_assert(ct::fromString<MyClass::MyEnum>("kVALUE0") == MyClass::MyEnum::kVALUE0, "asdf");
    static_assert(ct::fromString<MyClass::MyEnum>("kVALUE1") == MyClass::MyEnum::kVALUE1, "asdf");
    static_assert(ct::fromString<MyClass::MyEnum>("kVALUE2") == MyClass::MyEnum::kVALUE2, "asdf");
    static_assert(ct::fromString<MyClass::MyEnum>("kVALUE3") == MyClass::MyEnum::kVALUE3, "asdf");
    auto from_string = ct::fromString<MyClass::MyEnum>("kVALUE0");
    ASSERT_EQ(from_string, ct::value(MyClass::MyEnum::kVALUE0));
    from_string = ct::fromString<MyClass::MyEnum>("kVALUE1");
    ASSERT_EQ(from_string, ct::value(MyClass::MyEnum::kVALUE1));
    from_string = ct::fromString<MyClass::MyEnum>("kVALUE2");
    ASSERT_EQ(from_string, ct::value(MyClass::MyEnum::kVALUE2));
    from_string = ct::fromString<MyClass::MyEnum>("kVALUE3");
    ASSERT_EQ(from_string, ct::value(MyClass::MyEnum::kVALUE3));
}

TEST(enum_value_from_string, BitwiseEnum)
{
    auto from_string = ct::bitsetFromString<MyClass::BitwiseEnum>("kVALUE0|kVALUE1");
    ASSERT_EQ(from_string.test(MyClass::BitwiseEnum::kVALUE0), true);
    ASSERT_EQ(from_string.test(MyClass::BitwiseEnum::kVALUE1), true);
}

// TODO, figure out this case
/*TEST(enum_value_from_string, MixedBitwise)
{
    auto from_string = ct::bitsetFromString<MyClass::MixedBitwise>("kVALUE0|kFLAG0");
    ASSERT_EQ(from_string & MyClass::MixedBitwise::kFLAG0, true);
    ASSERT_EQ(from_string & MyClass::MixedBitwise::kVALUE0, true);
}*/

TEST(enum_bitwise, bitwise)
{
    Bitset bitset(Bitset::v0);
    for (Bitset i = Bitset::v0; i <= Bitset::v5; ++i)
    {
        ct::EnumBitset<Bitset> bitset;
        ASSERT_EQ(bitset.test(i), false);
        bitset.set(i);
        ASSERT_EQ(bitset.test(i), true);
        for (Bitset j = Bitset::v1; j <= Bitset::v5; ++j)
        {
            if (i != j)
            {
                ASSERT_EQ(bitset.test(j), false);
                bitset.set(j);
                ASSERT_EQ(bitset.test(j), true);

                std::stringstream ss;
                ss << bitset;

                auto from_str = ct::bitsetFromString<Bitset>(ss.str());
                ASSERT_EQ(from_str.test(i), true);
                ASSERT_EQ(from_str.test(j), true);

                bitset.flip(j);
                ASSERT_EQ(bitset.test(j), false);
                ASSERT_EQ(bitset.test(i), true);
            }
        }
        bitset.flip(i);
        ASSERT_EQ(bitset.test(i), false);
    }

    ct::EnumBitset<Bitset> bset;
    for (Bitset i = Bitset::v0; i <= Bitset::v5; ++i)
    {
        ASSERT_EQ(bset.test(i), false);
    }
    for (Bitset i = Bitset::v0; i <= Bitset::v5; ++i)
    {
        bset.set(i);
        auto j = i;
        ++j;
        for (; j <= Bitset::v5; ++j)
        {
            ASSERT_EQ(bset.test(j), false);
        }

        for (Bitset j = Bitset::v0; j <= i; ++j)
        {
            ASSERT_EQ(bset.test(j), true);
        }
    }
}

TEST(enum_bitwise, from_template)
{
    auto bits = ct::EnumBitset<Bitset>(Bitset::v0 | Bitset::v1);
    ASSERT_EQ(bits.test(Bitset::v0), true);
    ASSERT_EQ(bits.test(Bitset::v1), true);
    constexpr uint64_t v = Bitset::v0 | Bitset::v1;
    bitsetFoo<v>("v1|v0");
    bitsetFoo<uint64_t(Bitset::v0)>("v0");
    bitsetFoo<uint64_t(Bitset::v1)>("v1");
    bitsetFoo<uint64_t(Bitset::v2)>("v2");
    bitsetFoo<uint64_t(Bitset::v3)>("v3");
}

// This is mostly to ensure certain operators are overloaded such that there are no undefined references in < c++17
TEST(enum_bitwise, operators)
{
    MyClass::SecondEnum e;
    ASSERT_EQ(e, 0);
    auto b0 = e | MyClass::SecondEnum::kBGR;
    ASSERT_EQ(b0.value, ct::value(MyClass::SecondEnum::kBGR));
    auto b1 = MyClass::SecondEnum::kBGR | e;
    ASSERT_EQ(b1, ct::value(MyClass::SecondEnum::kBGR));
    b1 = MyClass::SecondEnum::kRGB | e;
    ASSERT_EQ(b1, ct::value(MyClass::SecondEnum::kRGB));
    b0 = e & MyClass::SecondEnum::kBGR;
    ASSERT_EQ(b0, 0);
    b1 = MyClass::SecondEnum::kBGR & e;
    ASSERT_EQ(b1, 0);
}
