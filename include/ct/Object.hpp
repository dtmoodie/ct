#pragma once
#include "Hash.hpp"
#include "String.hpp"
#include <ct/detail/TypeTraits.hpp>
#include <type_traits>

namespace ct
{
    template<std::size_t Tlen>
    constexpr uint32_t hashClassName(const char(&str)[Tlen]) {
        return ctcrc32Range(str, classNameIdx(str) + 1);
    }
    constexpr uint32_t hashClassNameHelper(const char* str)
    {
        return ctcrc32Range(str, classNameIdx(str));
    }
    constexpr uint32_t hashClassName(const char* str)
    {
#ifdef _MSC_VER
        return ctcrc32Range(str, classNameIdx(str));
#else
        return hashClassNameHelper(str + ct::findLast(str, ' ') + 1);
#endif
    }
}

#ifdef _MSC_VER
#define __CT_STRUCT_MAGIC_FUNCTION__ __FUNCTION__
#else
#define __CT_STRUCT_MAGIC_FUNCTION__ __PRETTY_FUNCTION__
#endif

#define DECLARE_CLASS_HASH \
static constexpr uint32_t getHash() {return ct::hashClassName(__CT_STRUCT_MAGIC_FUNCTION__);}

#define DECLARE_MODULE_HASH(N) \
static constexpr uint32_t getHash() {return ct::hashClassName(__CT_STRUCT_MAGIC_FUNCTION__) ^ N;} \
enum : uint32_t {hash = getHash()};

namespace ct
{


    template<class T, uint32_t N, typename enable = void>
    struct HashedObject;

    template<class T, uint32_t N>
    struct HashedObject<T, N, typename std::enable_if<std::is_pod<remove_reference_t<T>>::value>::type>
    {
        DECLARE_MODULE_HASH(N);
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

    template <uint32_t N, class T> inline
        HashedObject<T, N> makeHashed(T && value)
    {
        return{ std::forward<T>(value) };
    }
}
#define CT_MAKE_HASHED(var) makeHashed<ctcrc32(#var)>(var)
