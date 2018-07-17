#pragma once
#include <ct/detail/TypeTraits.hpp>

namespace ct
{
    template<class T>
    using OperatorType = typename std::conditional<
        std::is_array<typename std::remove_reference<T>::type>::value,
        typename std::remove_cv<T>::type,
        typename std::conditional<std::is_lvalue_reference<T>::value,
        T,
        typename std::decay<T>::type>::type>::type;


}