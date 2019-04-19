#ifndef CT_REFLECT_HASH_INL_HPP
#define CT_REFLECT_HASH_INL_HPP
#include "hash.hpp"
#include <ct/reflect.hpp>

namespace ct
{
    template <class T>
    struct TypeHash<T, ct::EnableIfReflected<T>>
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
#endif // CT_REFLECT_HASH_INL_HPP
