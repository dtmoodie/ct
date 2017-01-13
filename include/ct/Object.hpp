#pragma once
#include "String.hpp"
#include <type_traits>

namespace ct
{
    template<class T, uint32_t N>
    struct HashedObject
    {
        DECLARE_MODULE_HASH(N)
        enum
        {
            ConstSize = 1,
            Dims = 0,
            IsIndexable = 0
        };
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
        
        Type value;
    };
    template<class T, uint32_t N>
    struct HashedObject<std::vector<T>&, N>
    {
        DECLARE_MODULE_HASH(N)
        enum
        {
            ConstSize = 0,
            Dims = 1,
            IsIndexable = 1
        };
        using Type = std::vector<T>&;

        HashedObject(std::vector<T>& value_) :
            value(std::forward<std::vector<T>>(value_))
        {
        }

        HashedObject & operator=(HashedObject const &) = delete;

        Type operator()(int64_t idx)
        {
            return value[idx];
        }
        int64_t getSize(int dim)
        {
            return value.size();
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
