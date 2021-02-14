#ifndef CT_REFLECT_HASH_HPP
#define CT_REFLECT_HASH_HPP
#include <cstdint>
#include <ct/hash.hpp>
#include <ct/reflect.hpp>

#define DECL_HASHED_TYPE(TYPE)                                                                                         \
    template <>                                                                                                        \
    struct TypeHash<TYPE, void>                                                                                        \
    {                                                                                                                  \
        static constexpr const char* name = #TYPE;                                                                     \
        static constexpr const uint32_t value = crc32(#TYPE);                                                          \
    }

namespace ct
{

    template <class T, class Enable = void>
    struct TypeHash
    {
    };

    // same for this type so long as the structure is the same
    template <class T>
    constexpr uint32_t hashStruct();

    // same for this type if the values of all accessible data members is the same
    template <class T>
    uint32_t hashValues(const T& data);

    // hash of a types data member datatypes and names, such that two types can be checked for equivalence.
    // IE struct Vec{float x,y,z;};
    //    struct Point{float x,y,z;};
    //    hashMembers<Vec>() == hashMembers<Point>();
    template <class T>
    constexpr uint32_t hashMembers();

    template <class T>
    uint32_t hashMemberValues(const T& data);

    DECL_HASHED_TYPE(float);
    DECL_HASHED_TYPE(int32_t);
    DECL_HASHED_TYPE(double);
    DECL_HASHED_TYPE(uint32_t);
    DECL_HASHED_TYPE(int16_t);
    DECL_HASHED_TYPE(uint16_t);
    DECL_HASHED_TYPE(uint8_t);
    DECL_HASHED_TYPE(int8_t);

    ///////////////////////////////////////////////////////////////////////////////////

    namespace detail
    {
#ifdef __GNUC__
#if __GNUC__ < 9
        constexpr const bool can_constexpr_hash_member_offset = false;
#else
        constexpr const bool can_constexpr_hash_member_offset = true;
#endif
#else
        constexpr const bool can_constexpr_hash_member_offset = true;
#endif

        struct HashOptions
        {
            // Include the name of the struct in the hash
            const bool hash_struct_name;
            const bool hash_member_types;
            const bool hash_member_offsets;
            const bool hash_member_indicies;
            const bool hash_member_names;

            constexpr HashOptions(const bool struct_name = false,
                                  const bool member_type = true,
                                  const bool member_offset = can_constexpr_hash_member_offset,
                                  const bool member_index = !can_constexpr_hash_member_offset,
                                  const bool member_names = false)
                : hash_struct_name(struct_name), hash_member_types(member_type), hash_member_offsets(member_offset),
                  hash_member_indicies(member_index), hash_member_names(member_names)

            {
            }
        };

        template <class T>
        constexpr uint32_t hashStructName(HashOptions options = HashOptions())
        {
            return options.hash_struct_name ? crc32(Reflect<T>::getTypeName()) : 0;
        }

        template <class T, index_t I>
        constexpr uint32_t hashMemberName(HashOptions options = HashOptions())
        {
            return options.hash_member_names
                       ? std::integral_constant<uint32_t, crc32(Reflect<T>::getPtr(Indexer<I>{}).getName())>::value
                       : 0;
        }

        template <class T, index_t I>
        constexpr uint32_t hashMemberType(HashOptions options = HashOptions())
        {
            return options.hash_member_types
                       ? TypeHash<typename std::decay<typename FieldGetType<T, I>::type>::type>::value
                       : 0;
        }

        template <class T, index_t I>
        constexpr uint32_t hashMemberOffset(HashOptions options = HashOptions())
        {
            return options.hash_member_offsets ? memberOffset(Reflect<T>::getPtr(Indexer<I>{}).m_ptr)
                                               : (options.hash_member_indicies ? uint32_t(I) : 0);
        }

        template <class T, index_t I>
        constexpr auto hashMember(uint32_t seed, HashOptions options = HashOptions())
            -> EnableIfIsMemberObject<T, I, uint32_t>
        {
            return combineHash(seed,
                               combineHash(combineHash(hashMemberType<T, I>(options), hashMemberOffset<T, I>(options)),
                                           hashMemberName<T, I>(options)));
        }

        template <class T, index_t I>
        constexpr auto hashMember(uint32_t seed, HashOptions options = HashOptions())
            -> DisableIfIsMemberObject<T, I, uint32_t>
        {
            return seed;
        }

        template <class T>
        constexpr uint32_t hashMembersHelper(Indexer<0>, HashOptions options = HashOptions())
        {
            return hashMember<T, 0>(0, options);
        }

        template <class T, index_t I>
        constexpr uint32_t hashMembersHelper(Indexer<I> idx, HashOptions options = HashOptions())
        {
            return hashMember<T, I>(hashMembersHelper<T>(--idx, options), options);
        }

        template <class T>
        constexpr uint32_t hashMembers(HashOptions options = HashOptions())
        {
            return hashMembersHelper<T>(ct::Reflect<T>::end(), options);
        }

        template <class T>
        constexpr uint32_t hash(HashOptions options = HashOptions())
        {
            return combineHash(hashStructName<T>(options), hashMembers<T>(options));
        }
    } // namespace detail

    template <class T>
    struct TypeHash<T, EnableIfReflected<T>>
    {
        static constexpr const uint32_t value = hashStruct<T>();
    };

    template <class T>
    constexpr uint32_t hashStruct()
    {
        return crc32(Reflect<T>::getTypeName()) ^ hashMembers<T>();
    }

    template <class T>
    uint32_t hashValues(const T& data)
    {
        // TODO
        return 0;
    }

    template <class T, index_t I>
    constexpr uint32_t hashMember()
    {
        return std::integral_constant<uint32_t, crc32(Reflect<T>::getPtr(Indexer<I>{}).m_name)>::value ^
               TypeHash<typename std::decay<typename FieldGetType<T, I>::type>::type>::value;
    }

    template <class T>
    constexpr uint32_t hashMemberHelper(const uint32_t hash, const Indexer<0U> /*idx*/)
    {
        // using name_hash_t = typename std::integral_constant<uint32_t, crc32(Reflect<T>::name(Indexer<0U>{}))>;
        return (hash ^ hashMember<T, 0U>());
    }

    template <class T, index_t I>
    constexpr uint32_t hashMemberHelper(const uint32_t hash, const Indexer<I> idx)
    {
        // using name_hash_t = typename std::integral_constant<uint32_t, crc32(Reflect<T>::name(Indexer<I>{}))>;
        return hashMemberHelper<T>((hash ^ hashMember<T, I>()), --idx);
    }

    template <class T>
    constexpr uint32_t hashMembers()
    {
        return hashMemberHelper<T>(0U, Reflect<T>::end());
    }

    template <class T>
    uint32_t hashMemberValues(const T& data)
    {
        // TODO
        return 0;
    }
} // namespace ct

#endif // CT_REFLECT_HASH_HPP
