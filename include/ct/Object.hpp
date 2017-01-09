#pragma once
#include "String.hpp"
#include <type_traits>

namespace ct
{
    template<class T, uint32_t N>
    struct HashedObject
    {
        DECLARE_MODULE_HASH(N)
        using Type = typename std::conditional<
            std::is_array<typename std::remove_reference<T>::type>::value,
            typename std::remove_cv<T>::type,
            typename std::conditional<std::is_lvalue_reference<T>::value,
            T,
            typename std::decay<T>::type>::type>::type;

        HashedObject(T&& value_):
            value(std::forward<T>(value_))
        {
        }

        HashedObject & operator=(HashedObject const &) = delete;

        Type operator()()
        {
            return value;
        }
        
        static constexpr Type outputType()
        {
            return {};
        }
        Type value;
        
    };
    template <uint32_t N, class T> inline
        HashedObject<T, N> makeHashed(T && value)
    {
        return{ std::forward<T>(value) };
    }
}
#define CT_MAKE_HASHED(var) makeHashed<ctcrc32(#var)>(var)