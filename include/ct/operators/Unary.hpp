#pragma once

#include <ct/operators/Operator.hpp>
#include <ct/String.hpp>

namespace ct
{
    template<class T, typename enable = void>
    struct Square;
    template<class T>
    using enable_if_pod = typename std::enable_if<
        std::is_pod<remove_reference_t<typename remove_reference_t<T>::Type>>::value>::type;

    template<class T>
    using data_type = typename remove_reference_t<T>::Type;

    template<class T>
    struct Square<T, enable_if_pod<T>>
    {
        DECLARE_CLASS_HASH;
        typedef OperatorType<T> Input_t;
        typedef remove_reference_t<typename remove_reference_t<T>::Type> Output_t;
        typedef typename remove_reference_t<T>::Type OutputReference_t;
        typedef Output_t Type;
        
        Square(T&& value_):
            value(std::forward<T>(value_))
        {
        }

        Output_t operator()()
        {
            return value()*value();
        }
        
        void operator()(OutputReference_t val)
        {
            val = value() * value();
        }
        Input_t value;
    };

    template<class T, typename enable = void>
    struct Sqrt;

    template<class T>
    struct Sqrt<T, enable_if_pod<T>>
    {
        DECLARE_CLASS_HASH;
        typedef OperatorType<T> Input_t;
        typedef remove_reference_t<typename remove_reference_t<T>::Type> Output_t;
        typedef typename remove_reference_t<T>::Type OutputReference_t;
        typedef Output_t Type;

        Sqrt(T&& value_) :
            value(std::forward<T>(value_))
        {
        }

        Output_t operator()()
        {
            return std::sqrt(value());
        }

        Input_t value;
    };

    template<template<class, class> class OP, class IN, class... Args>
    OP<IN, void> makeUnary(IN&& input, Args... args)
    {
        return {std::forward<IN>(input), args...};
    }
} // namespace ct
