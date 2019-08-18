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

BITSET_START(Bitset, int)
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
        std::cout << #test << " not working as expected";                                                              \
        std::abort();                                                                                                  \
    }

template <size_t V>
void bitsetFoo()
{
    auto bitset = ct::EnumBitset<Bitset>::fromBitValue<V>();
    std::cout << "From template parameter" << std::endl;
    std::cout << bitset << std::endl;
}

int main()
{
    static_assert(ct::IsEnumField<ct::PtrType<MyClass::MyEnum, 0>>::value, "asdf");
    std::cout << "Print all enum values with name\n";
    ct::printEnums<MyClass::MyEnum>(std::cout);

    static_assert(MyClass::MyEnum::kVALUE0 == 0, "asdf");
    static_assert(MyClass::MyEnum::kVALUE1 == 1, "asdf");
    static_assert(MyClass::MyEnum::kVALUE2 == 2, "asdf");

    std::cout << "Print each individual enum value manually\n";
    std::cout << MyClass::MyEnum::kVALUE0 << std::endl;
    std::cout << MyClass::MyEnum::kVALUE1 << std::endl;
    std::cout << MyClass::MyEnum::kVALUE2 << std::endl;
    std::cout << MyClass::MyEnum::kVALUE3 << std::endl;

    std::cout << "Print a runtime value\n";
    MyClass::MyEnum val = MyClass::MyEnum::kVALUE0;
    std::cout << val << std::endl;

    val = MyClass::MyEnum::kVALUE1;
    std::cout << val << std::endl;

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

    ct::printEnums<MyClass::SecondEnum>(std::cout);

    ct::printEnums<MyClass::StandardEnum>(std::cout);
    ct::printEnums<MyClass::StandardEnum2>(std::cout);

    MyClass::BitwiseEnum bitwise_enum;
    bitwise_enum = MyClass::BitwiseEnum::kVALUE0 | MyClass::BitwiseEnum::kVALUE2;
    ct::printEnums<MyClass::BitwiseEnum>(std::cout);

    std::cout << bitwise_enum << std::endl;

    foo(MyClass::MyEnum::kVALUE0);

    // Enum bitset stuffs
    ct::EnumBitset<Bitset> bitset;
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
    bitsetFoo<Bitset::v0 | Bitset::v1>();

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
}
