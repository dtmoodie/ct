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

    static_assert(MyClass::MyEnum::kVALUE0::value() == 0, "asdf");
}
