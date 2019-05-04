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
};

int main()
{
    static_assert(ct::IsEnumValue<ct::PtrType<MyClass::MyEnum, 0>>::value, "asdf");
    ct::printEnums<MyClass::MyEnum>(std::cout);

    static_assert(MyClass::MyEnum::kVALUE0::value == 0, "asdf");
    static_assert(MyClass::MyEnum::kVALUE1::value == 1, "asdf");
    static_assert(MyClass::MyEnum::kVALUE2::value == 2, "asdf");

    std::cout << MyClass::MyEnum::kVALUE0{} << std::endl;
    std::cout << MyClass::MyEnum::kVALUE1{} << std::endl;
    std::cout << MyClass::MyEnum::kVALUE2{} << std::endl;
    std::cout << MyClass::MyEnum::kVALUE3{} << std::endl;
    MyClass::MyEnum val = MyClass::MyEnum::kVALUE0::value;
    std::cout << val << std::endl;
    val = MyClass::MyEnum::kVALUE1::value;
    std::cout << val << std::endl;

    static_assert(ct::fromString<MyClass::MyEnum>("kVALUE0").value == MyClass::MyEnum::kVALUE0::value, "asdf");
    static_assert(ct::fromString<MyClass::MyEnum>("kVALUE1").value == MyClass::MyEnum::kVALUE1::value, "asdf");
    static_assert(ct::fromString<MyClass::MyEnum>("kVALUE2").value == MyClass::MyEnum::kVALUE2::value, "asdf");
    static_assert(ct::fromString<MyClass::MyEnum>("kVALUE3").value == MyClass::MyEnum::kVALUE3::value, "asdf");

    auto from_string_val = ct::fromString<MyClass::MyEnum>("kVALUE0");
    if (from_string_val.value != MyClass::MyEnum::kVALUE0::value)
    {
        std::cout << "Failed to read kVALUE0 from string" << std::endl;
    }
}
