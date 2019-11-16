#include <ct/reflect.hpp>
#include <ct/reflect_macros.hpp>

struct TestStruct2
{
    REFLECT_INTERNAL_BEGIN(TestStruct2)
        REFLECT_INTERNAL_MEMBER(float, x)
        REFLECT_INTERNAL_MEMBER(float, y)
        REFLECT_INTERNAL_MEMBER(float, z)
    REFLECT_INTERNAL_END;
};

int main() {}