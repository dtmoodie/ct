#include <ct/Indexer.hpp>
#include <ct/enum.hpp>
#include <ct/reflect.hpp>
#include <iostream>

struct MyClass
{
    ENUM_START(MyEnum, uint32_t)
        ENUM_VALUE(kVALUE0, 0)
        ENUM_VALUE(kVALUE1, 1)
        ENUM_VALUE(kVALUE2, 2)
        ENUM_VALUE(kVALUE3, 3)
    ENUM_END;

    ENUM_START(SecondEnum, uint8_t)
        ENUM_VALUE(kBGR, 0)
        ENUM_VALUE(kRGB, kBGR + 1)
        ENUM_VALUE(kHSV, kRGB + 1)
        ENUM_VALUE(kYUV, kHSV + 1)
        ENUM_VALUE(kHSL, kYUV + 1)
    ENUM_END;
};

template <class E, class T, T V>
struct EnumValue
{
    static constexpr T value = V;
    using value_type = T;
    constexpr value_type operator()() const { return value; }
    constexpr bool operator==(T v) const { return V == v; }
};

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

    switch (from_string_val.value)
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

    std::cout << "Enum working as expected" << std::endl;

    std::cout << "==========================" << std::endl;

    ct::printEnums<MyClass::SecondEnum>(std::cout);
}
