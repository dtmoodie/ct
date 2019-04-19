#include "../common.hpp"
#include "common.hpp"
#include <ct/reflect/hash.hpp>
int main()
{
    ct::StaticEquality<uint32_t, ct::TypeHash<float>::value, ct::crc32("float")>{};
    static_assert(ct::getName<0, ReflectedStruct>() == ct::StringView("x"), "asdf");
#if !(defined(_MSC_VER) && _MSC_VER == 1900)
    ct::StaticEquality<uint32_t, ct::crc32(ct::StringView("xy")), ct::crc32("xy")>{};
    ct::StaticEquality<uint32_t, ct::crc32(ct::StringView("x")), ct::crc32("x")>{};
    ct::StaticEquality<uint32_t, ct::crc32(ct::getName<0, ReflectedStruct>()), ct::crc32("x")>{};

    ct::StaticEquality<uint32_t, (ct::hashMember<ReflectedStruct, 0>()), ct::crc32("x") ^ ct::crc32("float")>{};

    ct::StaticEquality<uint32_t, ct::hashMember<ReflectedStruct, 1>(), ct::crc32("y") ^ ct::crc32("float")>{};

    ct::StaticEquality<uint32_t, ct::hashMember<ReflectedStruct, 2>(), ct::crc32("z") ^ ct::crc32("float")>{};
#ifdef CT_HAVE_CONSTEXPR_NAME
    static_assert(std::integral_constant<uint32_t, ct::hashStruct<ReflectedStruct>()>::value != 0, "asdf");
#endif

    ct::StaticInequality<uint32_t, ct::crc32(ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<0>{}).m_name), 0>{};
#endif
}
