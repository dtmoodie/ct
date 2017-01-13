#pragma once
#include "String.hpp"
#include <ct/detail/TypeTraits.hpp>
#include <type_traits>

namespace ct
{
    template<class T, uint32_t N, typename enable = void>
    struct HashedObject;

    template<class T, uint32_t N>
    struct HashedObject<T, N, typename std::enable_if<std::is_pod<remove_reference_t<T>>::value>::type>
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
    struct HashedObject<T, N, typename std::enable_if<Indexable<decay_t<T>>::IsIndexable>::type>
    {
        DECLARE_MODULE_HASH(N)
        enum
        {
            ConstSize = Indexable<decay_t<T>>::ConstSize || std::is_const<remove_reference_t<T>>::value,
            Dims = Indexable<decay_t<T>>::Dims,
            IsIndexable = Indexable<decay_t<T>>::IsIndexable
        };
        using Type = conditional_t<std::is_array<remove_reference_t<T>>::value,std::remove_cv_t<T>,
                                    conditional_t<std::is_lvalue_reference<T>::value,T,decay_t<T>>>;
        using Elem_t = typename Indexable<decay_t<T>>::Elem_t;


        HashedObject(T&& value_) :
            value(std::forward<T>(value_))
        {
        }

        HashedObject & operator=(HashedObject const &) = delete;

        /*ElemType  operator()(int64_t idx)
        {
            return value[idx];
        }*/

        template<class ... U>
        typename std::enable_if<sizeof...(U) == Indexable<decay_t<T>>::Dims && sizeof...(U) != 1, Elem_t>::type operator()(U... dims)
        {
            return value[dims...];
        }

        template<class U>
        typename std::enable_if<Indexable<decay_t<T>>::Dims == 1, Elem_t>::type operator()(U dim)
        {
            return value[dim];
        }

        size_t getSize(int dim = 0)
        {
            return value.size();
        }

        template<class U>
        typename std::enable_if<
            !Indexable<decay_t<T>>::ConstSize &&
            Indexable<decay_t<T>>::Dims == 1 &&
            !std::is_const<remove_reference_t<T>>::value &&
            std::is_integral<U>::value>::type resize(U size)
        {
            value.resize(size);
        }

        template<class ...U>
        typename std::enable_if<
            !Indexable<decay_t<T>>::ConstSize &&
            Indexable<decay_t<T>>::Dims == sizeof...(U) &&
            !std::is_const<remove_reference_t<T>>::value>::type resize(U... size)
        {
            value.resize(size...);
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
