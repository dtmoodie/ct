#include "../common.hpp"
#include "common.hpp"
#include <ct/reflect/hash.hpp>
int main()
{
    STATIC_EQUAL(ct::TypeHash<float>::value, ct::crc32("float"));
    STATIC_EQUAL((ct::hashMember<ReflectedStruct, 0>()), ct::crc32("x") ^ ct::crc32("float"));
    STATIC_EQUAL((ct::hashMember<ReflectedStruct, 1>()), ct::crc32("y") ^ ct::crc32("float"));
    STATIC_EQUAL((ct::hashMember<ReflectedStruct, 2>()), ct::crc32("z") ^ ct::crc32("float"));
    std::integral_constant<uint32_t, ct::hashStruct<ReflectedStruct>()>::value;

    ct::StaticInequality<uint32_t, ct::crc32(ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<0>{}).m_name), 0>{};
}
