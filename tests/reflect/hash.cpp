#include "../common.hpp"
#include "common.hpp"
#include <ct/reflect/hash.hpp>

using namespace ct;
using namespace ct::detail;

struct A
{
    float x, y, z;
};

struct B
{
    float x, y, z;
};

struct C
{
    float y, x, z;
};

namespace ct
{
    REFLECT_BEGIN(A)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(z)
    REFLECT_END;

    REFLECT_BEGIN(B)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(z)
    REFLECT_END;

    REFLECT_BEGIN(C)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(z)
    REFLECT_END;
} // namespace ct

int main()
{
    constexpr const ct::detail::HashOptions hash_members(false, true, false, true, true);

    ct::StaticEquality<uint32_t, ct::detail::hash<A>(hash_members), ct::detail::hash<B>(hash_members)>{};
    ct::StaticInequality<uint32_t, ct::detail::hash<A>(hash_members), ct::detail::hash<C>(hash_members)>{};

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
                                 hashMemberName<TestA, 0>(hash_members),
                                 hashMemberName<TestB, 1>(hash_members)>{};
            ct::StaticInequality<uint32_t,
                                 hashMemberName<TestA, 1>(hash_members),
                                 hashMemberName<TestB, 2>(hash_members)>{};

            ct::StaticInequality<uint32_t,
                                 hashMemberName<TestA, 2>(hash_members),
                                 hashMemberName<TestB, 0>(hash_members)>{};
        }

// memberOffset needs a reinterpret_cast bridge between unrelated pointer
// types, which GCC 9+ (having fixed disallowing UB in constexpr) and Clang
// both reject in a constant expression (see MemberObjectPointer.hpp). Only
// ancient GCC can evaluate it at compile time. Clang defines __GNUC__ as 4,
// so it must be excluded explicitly.
#if __GNUC__ < 9 && !defined(__clang__)
        ct::StaticEquality<size_t, memberOffset(&TestA::x), 0>{};
        ct::StaticEquality<size_t, memberOffset(&TestA::y), 4>{};
        ct::StaticEquality<size_t, memberOffset(&TestA::z), 8>{};
        // https://developercommunity.visualstudio.com/content/problem/22196/static-assert-cannot-compile-constexprs-method-tha.html
        ct::StaticEquality<uint32_t, hashMemberOffset<TestA, 0>(), hashMemberOffset<TestB, 0>()>{};
        ct::StaticEquality<uint32_t, hashMemberOffset<TestA, 1>(), hashMemberOffset<TestB, 1>()>{};
        ct::StaticEquality<uint32_t, hashMemberOffset<TestA, 2>(), hashMemberOffset<TestB, 2>()>{};

        ct::StaticInequality<uint32_t, hashMemberOffset<TestA, 0>(), hashMemberOffset<TestB, 1>()>{};
        ct::StaticInequality<uint32_t, hashMemberOffset<TestA, 1>(), hashMemberOffset<TestB, 2>()>{};

        ct::StaticInequality<uint32_t, hashMemberOffset<TestA, 2>(), hashMemberOffset<TestB, 0>()>{};
#else
        assert(memberOffset(&TestA::x) == 0);
        assert(memberOffset(&TestA::y) == 4);
        assert(memberOffset(&TestA::z) == 8);

        // Runtime counterpart of the constexpr checks in the #if branch
        // above. On GCC 9+ memberOffset is not a constant expression, so
        // the default-options detail::hash (which folds in the member
        // offsets there) can only be verified at runtime. Clang defines
        // __GNUC__ as 4, so on Clang can_constexpr_hash_member_offset is
        // false and the default hash folds in member indices instead; the
        // asserts hold under either choice.
        assert(ct::detail::hash<A>() == ct::detail::hash<B>());
        assert(ct::detail::hash<TestA>() == ct::detail::hash<TestB>());
        assert(ct::detail::hash<ReflectedStruct>() != ct::detail::hash<Inherited>());

        // With offsets explicitly forced into the hash (something the #if
        // branch above cannot do on these compilers): the equivalent
        // layouts A and B still hash equal, while C, which has the same
        // member types in the same offset slots but a different name in
        // each slot, does not.
        {
            const ct::detail::HashOptions offsets_on(false, true, true, false, true);
            assert(ct::detail::hash<A>(offsets_on) == ct::detail::hash<B>(offsets_on));
            assert(ct::detail::hash<A>(offsets_on) != ct::detail::hash<C>(offsets_on));
        }
#endif
    }
#if (__GNUC__ < 9 && !defined(__clang__)) || _MSC_VER
    ct::StaticInequality<uint32_t, detail::hash<ReflectedStruct>(), ct::detail::hash<Inherited>()>{};

    // This tests to see if two structs are mem copy compatible.  IE they have the same members, with the same
    // datatypes, with the same offsets
    // An additonal check ontop of this should be to do sizeof(A) == sizeof(B) in case there are members that are not
    // reflected
    ct::StaticEquality<uint32_t, ct::detail::hash<TestA>(), ct::detail::hash<TestB>()>{};
#endif
#ifdef CT_HAVE_CONSTEXPR_NAME
    ct::StaticInequality<uint32_t, ct::detail::hash<TestA, StructHash>(), ct::detail::hash<TestB, StructHash>()>{};
#endif

#endif // _MSC_VER
}
