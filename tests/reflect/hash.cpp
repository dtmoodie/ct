#include "../common.hpp"
#include "common.hpp"
#include <ct/reflect/hash.hpp>

using namespace ct;
using namespace ct::detail;
int main()
{
    ct::StaticEquality<uint32_t, TypeHash<float>::value, ct::crc32("float")>{};
    static_assert(ct::getName<0, ReflectedStruct>() == ct::StringView("x"), "asdf");
#if !(defined(_MSC_VER) && _MSC_VER == 1900)
    ct::StaticEquality<uint32_t, crc32(ct::StringView("xy")), ct::crc32("xy")>{};
    ct::StaticEquality<uint32_t, crc32(ct::StringView("x")), ct::crc32("x")>{};
    ct::StaticEquality<uint32_t, crc32(ct::getName<0, ReflectedStruct>()), ct::crc32("x")>{};

    ct::StaticEquality<uint32_t, (ct::hashMember<ReflectedStruct, 0>()), ct::crc32("x") ^ ct::crc32("float")>{};

    ct::StaticEquality<uint32_t, ct::hashMember<ReflectedStruct, 1>(), ct::crc32("y") ^ ct::crc32("float")>{};

    ct::StaticEquality<uint32_t, ct::hashMember<ReflectedStruct, 2>(), ct::crc32("z") ^ ct::crc32("float")>{};
    static_assert(std::integral_constant<uint32_t, ct::hashStruct<ReflectedStruct>()>::value != 0, "asdf");

    ct::StaticInequality<uint32_t, ct::crc32(ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<0>{}).m_name), 0>{};
#endif

    // member names
    {
        ct::StaticEquality<uint32_t,
                           hashMemberName<TestA, 0>({
                               hash_struct_name : false,
                               hash_member_types : true,
                               hash_member_offsets : true,
                               hash_member_names : true
                           }),
                           hashMemberName<TestB, 0>({false, true, true, true})>{};
        ct::StaticEquality<uint32_t,
                           hashMemberName<TestA, 1>({false, true, true, true}),
                           hashMemberName<TestB, 1>({false, true, true, true})>{};
        ct::StaticEquality<uint32_t,
                           hashMemberName<TestA, 2>({false, true, true, true}),
                           hashMemberName<TestB, 2>({false, true, true, true})>{};

        ct::StaticInequality<uint32_t,
                             hashMemberName<TestA, 0>({false, true, true, true}),
                             hashMemberName<TestB, 1>({false, true, true, true})>{};
        ct::StaticInequality<uint32_t,
                             hashMemberName<TestA, 1>({false, true, true, true}),
                             hashMemberName<TestB, 2>({false, true, true, true})>{};

        ct::StaticInequality<uint32_t,
                             hashMemberName<TestA, 2>({false, true, true, true}),
                             hashMemberName<TestB, 0>({false, true, true, true})>{};
    }

    // member types
    {
        ct::StaticEquality<uint32_t,
                           hashMemberType<TestA, 0>({false, true, true, true}),
                           hashMemberType<TestB, 0>({false, true, true, true})>{};
        ct::StaticEquality<uint32_t,
                           hashMemberType<TestA, 1>({false, true, true, true}),
                           hashMemberType<TestB, 1>({false, true, true, true})>{};
        ct::StaticEquality<uint32_t,
                           hashMemberType<TestA, 2>({false, true, true, true}),
                           hashMemberType<TestB, 2>({false, true, true, true})>{};
    }

    // member offsets
    {
        ct::StaticEquality<uint32_t,
                           hashMemberOffset<TestA, 0>({false, true, true, true}),
                           hashMemberOffset<TestB, 0>({false, true, true, true})>{};
        ct::StaticEquality<uint32_t,
                           hashMemberOffset<TestA, 1>({false, true, true, true}),
                           hashMemberOffset<TestB, 1>({false, true, true, true})>{};
        ct::StaticEquality<uint32_t,
                           hashMemberOffset<TestA, 2>({false, true, true, true}),
                           hashMemberOffset<TestB, 2>({false, true, true, true})>{};

        ct::StaticInequality<uint32_t,
                             hashMemberOffset<TestA, 0>({false, true, true, true}),
                             hashMemberOffset<TestB, 1>({false, true, true, true})>{};
        ct::StaticInequality<uint32_t,
                             hashMemberOffset<TestA, 1>({false, true, true, true}),
                             hashMemberOffset<TestB, 2>({false, true, true, true})>{};

        ct::StaticInequality<uint32_t,
                             hashMemberOffset<TestA, 2>({false, true, true, true}),
                             hashMemberOffset<TestB, 0>({false, true, true, true})>{};
    }

    ct::StaticInequality<uint32_t, detail::hash<ReflectedStruct>(), ct::detail::hash<Inherited>()>{};

    // This tests to see if two structs are mem copy compatible.  IE they have the same members, with the same
    // datatypes, with the same offsets
    // An additonal check ontop of this should be to do sizeof(A) == sizeof(B) in case there are members that are not
    // reflected
    ct::StaticEquality<uint32_t,
                       ct::detail::hash<TestA>({false, true, true, true}),
                       ct::detail::hash<TestB>({false, true, true, true})>{};
}
