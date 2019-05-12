#include "enum.hpp"

#include <iostream>

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
}
