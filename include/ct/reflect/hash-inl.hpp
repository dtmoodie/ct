#include <ct/reflect.hpp>
#include "hash.hpp"

namespace ct
{
    template<class T>
    struct TypeHash<T, ct::enable_if_reflected<T>>
    {
        static constexpr const uint32_t value = hashType<T>();
    };


    template<class T>
    constexpr uint32_t hashTypeHelper(const uint32_t hash, Indexer<0U> idx)
    {
        return (hash ^ std::integral_constant<uint32_t, crc32(Reflect<T>::getName(Indexer<0U>{})>::value) ^ TypeHash<typename GetterType<T, 0U>::type>::value;
    }

    template<class T, index_t I>
    constexpr uint32_t hashTypeHelper(const uint32_t hash, Indexer<I> idx)
    {
        return hashTypeHelper<T>((hash ^ std::integral_constant<uint32_t, crc32(Reflect<T>::getName(Indexer<0U>{}))>::value) ^ TypeHash<typename GetterType<T, I>::type>::value, --idx);
    }

    template<class T>
    constexpr uint32_t hashType()
    {
        return crc32(Reflect<T>::getName()) ^ hashTypeHelper<T>(0U, Reflect<T>::end());
    }

    template<class T>
    uint32_t hashValues(const T& data)
    {

    }

    template<class T>
    constexpr uint32_t hashMembers()
    {

    }

    template<class T>
    uint32_t hashMemberValues(const T& data)
    {

    }
}
