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
        struct HashOptions
        {
            // Include the name of the struct in the hash
            static constexpr bool hash_struct_name = false;
            static constexpr bool hash_member_types = true;
            static constexpr bool hash_member_offsets = true;
            static constexpr bool hash_member_names = false;
        };

        struct HashMembers
        {
            // Include the name of the struct in the hash
            static constexpr bool hash_struct_name = false;
            static constexpr bool hash_member_types = true;
            static constexpr bool hash_member_offsets = true;
            static constexpr bool hash_member_names = true;
        };

        template <class T, class OPTS = HashOptions>
        constexpr auto hashStructName() -> EnableIf<OPTS::hash_struct_name, uint32_t>
        {
            return crc32(Reflect<T>::getName());
        }

        template <class T, class OPTS = HashOptions>
        constexpr auto hashStructName() -> DisableIf<OPTS::hash_struct_name, uint32_t>
        {
            return 0;
        }

        template <class T, index_t I, class OPTS = HashOptions>
        constexpr uint32_t hashMemberName()
        {
            return OPTS::hash_member_names
                       ? std::integral_constant<uint32_t, crc32(Reflect<T>::getPtr(Indexer<I>{}).getName())>::value
                       : 0;
        }

        template <class T, index_t I, class OPTS = HashOptions>
        constexpr uint32_t hashMemberType()
        {
            return OPTS::hash_member_types
                       ? TypeHash<typename std::decay<typename FieldGetType<T, I>::type>::type>::value
                       : 0;
        }

        template <class T, index_t I, class OPTS = HashOptions>
        constexpr uint32_t hashMemberOffset()
        {
            return OPTS::hash_member_offsets ? memberOffset(Reflect<T>::getPtr(Indexer<I>{}).m_ptr) : 0;
        }

        template <class T, index_t I, class OPTS = HashOptions>
        constexpr auto hashMember(uint32_t seed) -> EnableIfIsMemberObject<T, I, uint32_t>
        {
            return combineHash(seed,
                               combineHash(combineHash(hashMemberType<T, I, OPTS>(), hashMemberOffset<T, I, OPTS>()),
                                           hashMemberName<T, I, OPTS>()));
        }

        template <class T, index_t I, class OPTS = HashOptions>
        constexpr auto hashMember(uint32_t seed) -> DisableIfIsMemberObject<T, I, uint32_t>
        {
            return seed;
        }

        template <class T, class OPTS = HashOptions>
        constexpr uint32_t hashMembersHelper(Indexer<0>)
        {
            return hashMember<T, 0, OPTS>(0);
        }

        template <class T, class OPTS, index_t I>
        constexpr uint32_t hashMembersHelper(Indexer<I> idx)
        {
            return hashMember<T, I, OPTS>(hashMembersHelper<T, OPTS>(--idx));
        }

        template <class T, class OPTS>
        constexpr uint32_t hashMembers()
        {
            return hashMembersHelper<T, OPTS>(ct::Reflect<T>::end());
        }

        template <class T, class OPTS = HashOptions>
        constexpr uint32_t hash()
        {
            return combineHash(hashStructName<T, OPTS>(), hashMembers<T, OPTS>());
        }
    }

    template <class T>
    struct TypeHash<T, EnableIfReflected<T>>
    {
        static constexpr const uint32_t value = hashStruct<T>();
    };

    template <class T>
    constexpr uint32_t hashStruct()
    {
        return crc32(Reflect<T>::getName()) ^ hashMembers<T>();
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
        // using name_hash_t = typename std::integral_constant<uint32_t, crc32(Reflect<T>::getName(Indexer<0U>{}))>;
        return (hash ^ hashMember<T, 0U>());
    }

    template <class T, index_t I>
    constexpr uint32_t hashMemberHelper(const uint32_t hash, const Indexer<I> idx)
    {
        // using name_hash_t = typename std::integral_constant<uint32_t, crc32(Reflect<T>::getName(Indexer<I>{}))>;
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
}

#endif // CT_REFLECT_HASH_HPP
