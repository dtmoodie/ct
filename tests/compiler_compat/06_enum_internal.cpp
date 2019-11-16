#include <ct/enum.hpp>
#include <ct/reflect_macros.hpp>

ENUM_BEGIN(MyEnum, uint32_t)
    ENUM_VALUE(kVALUE0, 1)
    ENUM_VALUE(kVALUE1, 2)
    ENUM_VALUE(kVALUE2, 3)
    ENUM_VALUE(kVALUE3, 4)
ENUM_END;

int main() {}