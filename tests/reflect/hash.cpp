#include <ct/reflect/hash.hpp>
#include "common.hpp"
int main()
{
    std::integral_constant<uint32_t, ct::hashType<ReflectedStruct>()>::value;
}
