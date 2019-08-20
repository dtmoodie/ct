#include "enum.hpp"
#include "ct/EnumBitset.hpp"
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

BITSET_START(Bitset)
    ENUM_VALUE(v0, 0)
    ENUM_VALUE(v1, 1)
    ENUM_VALUE(v2, 2)
    ENUM_VALUE(v3, 3)
    ENUM_VALUE(v4, 4)
    ENUM_VALUE(v5, 5)
ENUM_END;

#define REQUIRE(test)                                                                                                  \
    if (!(test))                                                                                                       \
    {                                                                                                                  \
        std::cout << __FILE__ << ":" << __LINE__ << " " << #test << " not working as expected" << std::endl;           \
        std::abort();                                                                                                  \
    }

template <uint64_t V>
void bitsetFoo(const std::string& expected)
{
    auto bitset = Bitset(V);
    std::stringstream ss;
    ss << bitset;
    if (ss.str() != expected)
    {
        std::cout << "Incorrect stringification from template argument, expected: '" << expected << "' but got: '"
                  << ss.str() << "'" << std::endl;
        std::abort();
    }
}

template <class T>
void checkPrint(T v, const std::string& expected)
{
    std::stringstream ss;
    ss << v;
    if (expected != ss.str())
    {
        std::cout << "Incorrect stringification of an enum, expected: '" << expected << "' but got '" << ss.str() << "'"
                  << std::endl;
        std::abort();
    }
}

template <class T>
void checkPrintEnums(const std::string& expected)
{
    std::stringstream ss;
    ct::printEnums<T>(ss);
    auto result = ss.str();
    if (expected != result)
    {
        std::cout << "Incorrect enumeration of enum values, expected: \n'" << expected << "' but got \n'" << result
                  << "'" << std::endl;
        std::abort();
    }
}

int main()
{
    static_assert(ct::IsEnumField<ct::PtrType<MyClass::MyEnum, 0>>::value, "asdf");
    std::cout << "Print all enum values with name\n";
    checkPrintEnums<MyClass::MyEnum>("kVALUE0 1\nkVALUE1 2\nkVALUE2 3\nkVALUE3 4\n");

    static_assert(MyClass::MyEnum::kVALUE0 == 1, "asdf");
    static_assert(MyClass::MyEnum::kVALUE1 == 2, "asdf");
    static_assert(MyClass::MyEnum::kVALUE2 == 3, "asdf");

    std::cout << "Print each individual enum value manually\n";
    checkPrint(MyClass::MyEnum::kVALUE0, "kVALUE0 1");
    checkPrint(MyClass::MyEnum::kVALUE1, "kVALUE1 2");
    checkPrint(MyClass::MyEnum::kVALUE2, "kVALUE2 3");
    checkPrint(MyClass::MyEnum::kVALUE3, "kVALUE3 4");

    std::cout << "Print a runtime value\n";
    MyClass::MyEnum val = MyClass::MyEnum::kVALUE0;
    checkPrint(val, "kVALUE0");

    val = MyClass::MyEnum::kVALUE1;
    checkPrint(val, "kVALUE1");

    static_assert(ct::fromString<MyClass::MyEnum>("kVALUE0") == MyClass::MyEnum::kVALUE0, "asdf");
    static_assert(ct::fromString<MyClass::MyEnum>("kVALUE1") == MyClass::MyEnum::kVALUE1, "asdf");
    static_assert(ct::fromString<MyClass::MyEnum>("kVALUE2") == MyClass::MyEnum::kVALUE2, "asdf");
    static_assert(ct::fromString<MyClass::MyEnum>("kVALUE3") == MyClass::MyEnum::kVALUE3, "asdf");

    auto from_string_val = ct::fromString<MyClass::MyEnum>("kVALUE0");
    if (from_string_val != MyClass::MyEnum::kVALUE0)
    {
        std::cout << "Failed to read kVALUE0 from string" << std::endl;
        return -1;
    }
    if (!(from_string_val != MyClass::MyEnum::kVALUE1))
    {
        std::cout << "Equality comparisons not working" << std::endl;
        return -1;
    }

    switch (from_string_val.value())
    {
    case MyClass::MyEnum::kVALUE0:
        std::cout << "Switch working as expected" << std::endl;
        break;
    case MyClass::MyEnum::kVALUE1:
        std::cout << "Switch not working as expected" << std::endl;
        return -1;
    case MyClass::MyEnum::kVALUE2:
        std::cout << "Switch not working as expected" << std::endl;
        return -1;
    case MyClass::MyEnum::kVALUE3:
        std::cout << "Switch not working as expected" << std::endl;
        return -1;
    default:
        std::cout << "Switch not working as expected" << std::endl;
        return -1;
    }

    // Here we check to make sure certain operators work correctly
    {
        MyClass::MyEnum val = MyClass::MyEnum::kVALUE0;
        if (!(val & MyClass::MyEnum::kVALUE0))
        {
        }

        if (!(MyClass::MyEnum::kVALUE0 & val))
        {
        }
    }

    std::cout << "Enum working as expected" << std::endl;
    std::cout << "==========================" << std::endl;

    checkPrintEnums<MyClass::SecondEnum>("kBGR 0\nkRGB 1\nkHSV 2\nkYUV 3\nkHSL 4\n");
    checkPrintEnums<MyClass::StandardEnum>("k0 0\nk1 1\nk2 2\nk3 3\n");
    checkPrintEnums<MyClass::StandardEnum2>("kASDF 0\nk1234 1\n");

    MyClass::BitwiseEnum bitwise_enum;
    bitwise_enum = MyClass::BitwiseEnum::kVALUE0 | MyClass::BitwiseEnum::kVALUE2;
    checkPrintEnums<MyClass::BitwiseEnum>("kVALUE0 0\nkVALUE1 1\nkVALUE2 2\n");

    checkPrint(bitwise_enum, "kVALUE2|kVALUE0");

    foo(MyClass::MyEnum::kVALUE0);

    // Enum bitset stuffs
    Bitset bitset(Bitset::v0);
    for (Bitset i = Bitset::v0; i <= Bitset::v5; ++i)
    {
        ct::EnumBitset<Bitset> bitset;
        REQUIRE(!bitset.test(i));
        bitset.set(i);
        REQUIRE(bitset.test(i));
        for (Bitset j = Bitset::v1; j <= Bitset::v5; ++j)
        {
            if (i != j)
            {
                REQUIRE(!bitset.test(j));
                bitset.set(j);
                REQUIRE(bitset.test(j));

                std::stringstream ss;
                ss << bitset;
                std::cout << ss.str() << std::endl;

                auto from_str = ct::bitsetFromString<Bitset>(ss.str());
                REQUIRE(from_str.test(i));
                REQUIRE(from_str.test(j));

                bitset.flip(j);
                REQUIRE(!bitset.test(j));
                REQUIRE(bitset.test(i));
            }
        }
        bitset.flip(i);
        REQUIRE(!bitset.test(i));
    }

    // just checking some operators are correctly overloaded
    MyClass::SecondEnum e;
    auto b0 = e | MyClass::SecondEnum::kBGR;
    REQUIRE(b0.value == MyClass::SecondEnum::kBGR);
    auto b1 = MyClass::SecondEnum::kBGR | e;
    b0 = e & MyClass::SecondEnum::kBGR;
    b1 = MyClass::SecondEnum::kBGR & e;

    auto bits = ct::EnumBitset<Bitset>(Bitset::v0 | Bitset::v1);
    REQUIRE(bits.test(Bitset::v0));
    REQUIRE(bits.test(Bitset::v1));
    bitsetFoo<Bitset::v0 | Bitset::v1>("v1|v0");
    bitsetFoo<Bitset::v0>("v0");
    bitsetFoo<Bitset::v1>("v1");
    bitsetFoo<Bitset::v2>("v2");
    bitsetFoo<Bitset::v3>("v3");

    ct::EnumBitset<Bitset> bset;
    for (Bitset i = Bitset::v0; i <= Bitset::v5; ++i)
    {
        REQUIRE(!bset.test(i));
    }
    for (Bitset i = Bitset::v0; i <= Bitset::v5; ++i)
    {
        bset.set(i);
        std::cout << bset << std::endl;
        auto j = i;
        ++j;
        for (; j <= Bitset::v5; ++j)
        {
            REQUIRE(!bset.test(j))
        }

        for (Bitset j = Bitset::v0; j <= i; ++j)
        {
            REQUIRE(bset.test(j));
        }
    }

    std::cout << "Mixed enum + bit flags" << std::endl;
    MyClass::MixedBitwise mixed = MyClass::MixedBitwise::kVALUE4 | MyClass::MixedBitwise::kFLAG0;
    checkPrint(mixed, "kFLAG0|kVALUE4");
}
