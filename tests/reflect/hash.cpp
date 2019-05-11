#include "../common.hpp"
#include "common.hpp"
#include <ct/reflect/hash.hpp>

using namespace ct;
using namespace ct::detail;

struct StructHash
{
    // Include the name of the struct in the hash
    static constexpr bool hash_struct_name = true;
    static constexpr bool hash_member_types = true;
    static constexpr bool hash_member_offsets = true;
    static constexpr bool hash_member_names = true;
};

int main()
{
    using HashOptions = ct::detail::HashOptions;
    using HashMembers = ct::detail::HashMembers;
    ct::StaticEquality<uint32_t, TypeHash<float>::value, ct::crc32("float")>{};
    static_assert(ct::getName<0, ReflectedStruct>() == ct::StringView("x"), "asdf");
#if !(defined(_MSC_VER) && _MSC_VER == 1900)
    ct::StaticEquality<uint32_t, crc32(ct::StringView("xy")), ct::crc32("xy")>{};
    ct::StaticEquality<uint32_t, crc32(ct::StringView("x")), ct::crc32("x")>{};
    ct::StaticEquality<uint32_t, crc32(ct::getName<0, ReflectedStruct>()), ct::crc32("x")>{};

    ct::StaticEquality<uint32_t, (ct::hashMember<ReflectedStruct, 0>()), ct::crc32("x") ^ ct::crc32("float")>{};

    ct::StaticEquality<uint32_t, ct::hashMember<ReflectedStruct, 1>(), ct::crc32("y") ^ ct::crc32("float")>{};

    ct::StaticEquality<uint32_t, ct::hashMember<ReflectedStruct, 2>(), ct::crc32("z") ^ ct::crc32("float")>{};
#ifdef CT_HAVE_CONSTEXPR_NAME
    static_assert(std::integral_constant<uint32_t, ct::hashStruct<ReflectedStruct>()>::value != 0, "asdf");
#endif
    ct::StaticInequality<uint32_t, ct::crc32(ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<0>{}).m_name), 0>{};
#endif

    // member types
    {
        ct::StaticEquality<uint32_t, hashMemberType<TestA, 0>(), hashMemberType<TestB, 0>()>{};
        ct::StaticEquality<uint32_t, hashMemberType<TestA, 1>(), hashMemberType<TestB, 1>()>{};
        ct::StaticEquality<uint32_t, hashMemberType<TestA, 2>(), hashMemberType<TestB, 2>()>{};
    }
#ifndef _MSC_VER
    // member offsets
    {
	    // member names
        {
            ct::StaticEquality<uint32_t, hashMemberName<TestA, 0>(), hashMemberName<TestB, 0>()>{};
            ct::StaticEquality<uint32_t, hashMemberName<TestA, 1>(), hashMemberName<TestB, 1>()>{};
            ct::StaticEquality<uint32_t, hashMemberName<TestA, 2>(), hashMemberName<TestB, 2>()>{};

            ct::StaticInequality<uint32_t,
                                 hashMemberName<TestA, 0, HashMembers>(),
                                 hashMemberName<TestB, 1, HashMembers>()>{};
            ct::StaticInequality<uint32_t,
                                 hashMemberName<TestA, 1, HashMembers>(),
                                 hashMemberName<TestB, 2, HashMembers>()>{};

            ct::StaticInequality<uint32_t,
                                 hashMemberName<TestA, 2, HashMembers>(),
                                 hashMemberName<TestB, 0, HashMembers>()>{};
        }

		// https://developercommunity.visualstudio.com/content/problem/22196/static-assert-cannot-compile-constexprs-method-tha.html
        ct::StaticEquality<uint32_t, hashMemberOffset<TestA, 0>(), hashMemberOffset<TestB, 0>()>{};
        ct::StaticEquality<uint32_t, hashMemberOffset<TestA, 1>(), hashMemberOffset<TestB, 1>()>{};
        ct::StaticEquality<uint32_t, hashMemberOffset<TestA, 2>(), hashMemberOffset<TestB, 2>()>{};

        ct::StaticInequality<uint32_t, hashMemberOffset<TestA, 0>(), hashMemberOffset<TestB, 1>()>{};
        ct::StaticInequality<uint32_t, hashMemberOffset<TestA, 1>(), hashMemberOffset<TestB, 2>()>{};

        ct::StaticInequality<uint32_t, hashMemberOffset<TestA, 2>(), hashMemberOffset<TestB, 0>()>{};

    }

    ct::StaticInequality<uint32_t, detail::hash<ReflectedStruct>(), ct::detail::hash<Inherited>()>{};

    // This tests to see if two structs are mem copy compatible.  IE they have the same members, with the same
    // datatypes, with the same offsets
    // An additonal check ontop of this should be to do sizeof(A) == sizeof(B) in case there are members that are not
    // reflected
    ct::StaticEquality<uint32_t, ct::detail::hash<TestA>(), ct::detail::hash<TestB>()>{};
#ifdef CT_HAVE_CONSTEXPR_NAME
    ct::StaticInequality<uint32_t, ct::detail::hash<TestA, StructHash>(), ct::detail::hash<TestB, StructHash>()>{};
#endif

#endif  // _MSC_VER
}
