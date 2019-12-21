//#include "enum.hpp"
#include "enum.hpp"
#include <ct/enum.hpp>
#include <iostream>
#include <ct/static_asserts.hpp>

ENUM_BEGIN(Test, uint32_t)

static constexpr const EnumValueType k0 = 0;
static constexpr const EnumValueType k1 = 1;

ENUM_END
;

int main()
{
    // we're pretty limited on what we can do with nvcc since it just kinda dies when handling real c++,
    // so we just test if value semantics work as expected
    static_assert(MyClass::MyEnum::kVALUE0 == 1, "asdf");
    static_assert(MyClass::MyEnum::kVALUE1 == 2, "asdf");
    static_assert(MyClass::MyEnum::kVALUE2 == 3, "asdf");

    ct::StaticEquality<uint64_t, MyClass::BitwiseEnum::kVALUE0, 1>{};
    ct::StaticEquality<uint64_t, MyClass::BitwiseEnum::kVALUE1, (1 << 1)>{};
    ct::StaticEquality<uint64_t, MyClass::BitwiseEnum::kVALUE2, (1 << 2)>{};
    ct::StaticEquality<uint64_t, MyClass::ExtendedEnum::kVALUE3, (1 << 3)>{};
    ct::StaticEquality<uint64_t, MyClass::ExtendedEnum::kVALUE4, (1 << 4)>{};
}
