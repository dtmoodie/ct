#pragma once

#include <ct/operators/Operator.hpp>

namespace ct
{
    template<class T> struct Square
    {
        DECLARE_CLASS_HASH
        typedef OperatorType<T> Input_t;
        typedef typename remove_reference_t<typename remove_reference_t<T>::Type> Output_t;
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

    template<class T> struct Sqrt
    {
        DECLARE_CLASS_HASH
        typedef OperatorType<T> Input_t;
        typedef typename remove_reference_t<typename remove_reference_t<T>::Type> Output_t;
        typedef Output_t Type;

        Sqrt(T&& value_) :
            value(std::forward<T>(value_))
        {
        }

        Output_t operator()()
        {
            return sqrt(value());
        }

        Input_t value;
    };

    template<template<class> class OP, class IN, class... Args>
    OP<IN> makeUnary(IN&& input, Args... args)
    {
        return {std::forward<IN>(input), args...};
    }

} // namespace ct