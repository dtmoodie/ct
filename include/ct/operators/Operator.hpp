#pragma once
#include <type_traits>

namespace ct
{
    template< bool B, class T, class F >
    using conditional_t = typename std::conditional<B, T, F>::type;

    template<class T>
    using remove_reference_t = typename std::remove_reference<T>::type;

    template<class T>
    using decay_t = typename std::decay<T>::type;

    template<class T>
    using remove_cv_t = typename std::remove_cv<T>::type;

    template<class T>
    using is_array_t = typename std::is_array<T>::value;

    template<class T>
    using is_lvalue_reference_t = typename std::is_lvalue_reference<T>::value;

    template<class T>
    using is_rvalue_reference_t = typename std::is_rvalue_reference<T>::value;

    template<class T> 
    using OperatorType = typename std::conditional<
        std::is_array<typename std::remove_reference<T>::type>::value,
        typename std::remove_cv<T>::type,
        typename std::conditional<std::is_lvalue_reference<T>::value,
        T,
        typename std::decay<T>::type>::type>::type;


}