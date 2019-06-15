//#include "enum.hpp"
#include <ct/enum.hpp>
#include <iostream>
#include "enum.hpp"
ENUM_START(Test, uint32_t)

static constexpr const EnumValueType k0 = 0;
static constexpr const EnumValueType k1 = 1;

ENUM_END;

int main()
{
    // we're pretty limited on what we can do with nvcc since it just kinda dies when handling real c++,
    // so we just test if value semantics work as expected
    static_assert(MyClass::MyEnum::kVALUE0 == 0, "asdf");
    static_assert(MyClass::MyEnum::kVALUE1 == 1, "asdf");
    static_assert(MyClass::MyEnum::kVALUE2 == 2, "asdf");
}
